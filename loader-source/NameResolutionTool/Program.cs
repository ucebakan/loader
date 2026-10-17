using System.IO.Compression;
using System.Security.Cryptography;
using System.Text.Json;
using dnlib.DotNet;
using dnlib.DotNet.Emit;
using dnlib.DotNet.Writer;
using Microsoft.NET.HostModel.AppHost;

record BundleEntry(long TablePosition, long Offset, long Size, long CompressedSize, byte Type, string Name);

static class Program
{
    const string OriginalBundleHash = "1790F51D79CFC55C488C95FA4F5F6BBC1F56E2CAB1368172793CD60D7537AD73";
    const string OldUiName = "4unityofflinetestsunucusu.exe";

    static List<BundleEntry> ReadBundle(string path)
    {
        if (!HostWriter.IsBundle(path, out long header)) throw new InvalidDataException("Not a .NET bundle");
        using var stream = File.OpenRead(path);
        stream.Position = header;
        using var reader = new BinaryReader(stream);
        uint major = reader.ReadUInt32(), minor = reader.ReadUInt32();
        int count = reader.ReadInt32();
        string id = reader.ReadString();
        if (major != 6 || minor != 0 || count < 1 || count > 5000) throw new InvalidDataException("Unsupported bundle manifest");
        for (int i = 0; i < 5; i++) reader.ReadInt64(); // deps/config locations and flags
        var entries = new List<BundleEntry>();
        for (int i = 0; i < count; i++)
        {
            long table = stream.Position;
            long offset = reader.ReadInt64(), size = reader.ReadInt64(), compressed = reader.ReadInt64();
            byte type = reader.ReadByte();
            string name = reader.ReadString();
            long stored = compressed == 0 ? size : compressed;
            if (offset < 0 || stored < 0 || offset > stream.Length - stored || name.Length > 1000) throw new InvalidDataException("Invalid bundle entry");
            entries.Add(new(table, offset, size, compressed, type, name));
        }
        Console.WriteLine($"Bundle version={major}.{minor}; entries={count}; header={header}");
        return entries;
    }

    static byte[] Payload(string path, BundleEntry entry)
    {
        using var stream = File.OpenRead(path);
        stream.Position = entry.Offset;
        byte[] raw = new byte[checked((int)(entry.CompressedSize == 0 ? entry.Size : entry.CompressedSize))];
        stream.ReadExactly(raw);
        if (entry.CompressedSize == 0) return raw;
        using var input = new MemoryStream(raw);
        using var inflater = new DeflateStream(input, CompressionMode.Decompress);
        using var result = new MemoryStream();
        inflater.CopyTo(result);
        if (result.Length != entry.Size) throw new InvalidDataException("Decompressed entry length mismatch");
        return result.ToArray();
    }

    static int Main(string[] args)
    {
        try
        {
            if (args.Length == 1 && args[0] == "--self-test") return SelfTest();
            if (args.Length == 2 && args[0] == "--resolve-ui")
            {
                Console.WriteLine(LoaderNameCompatibility.FileResolver.ResolveKnownFile(args[1], LoaderNameCompatibility.FileResolver.UiHash, LoaderNameCompatibility.FileResolver.UiBytes));
                return 0;
            }
            if (args.Length < 2) throw new ArgumentException("inspect <bundle> | patch <bundle> <new-output> | --self-test | --resolve-ui <directory>");
            string input = Path.GetFullPath(args[1]);
            string inputHash = Convert.ToHexString(SHA256.HashData(File.ReadAllBytes(input)));
            if (inputHash != OriginalBundleHash) throw new InvalidDataException("Unexpected input bundle hash; no edits permitted");
            var entries = ReadBundle(input);
            foreach(var config in entries.Where(e => e.Name.EndsWith(".runtimeconfig.json", StringComparison.Ordinal)))
            {
                using var json = JsonDocument.Parse(Payload(input, config));
                var options = json.RootElement.GetProperty("runtimeOptions");
                Console.WriteLine("Target framework=" + options.GetProperty("tfm").GetString());
            }
            BundleEntry? application = null;
            ModuleDefMD? module = null;
            foreach (var entry in entries.Where(e => e.Type == 1 && e.Size < 2000000))
            {
                var candidate = ModuleDefMD.Load(Payload(input, entry)); // metadata only, never Assembly.Load
                var matches = candidate.GetTypes().SelectMany(t => t.Methods).Where(m => m.HasBody && m.Body.Instructions.Any(i => i.OpCode == OpCodes.Ldstr && i.Operand is string s && s == OldUiName)).ToArray();
                if (matches.Length == 0) { candidate.Dispose(); continue; }
                if (application != null) throw new InvalidDataException("Multiple application modules match");
                application = entry; module = candidate;
                Console.WriteLine($"Application assembly={entry.Name}; bytes={entry.Size}; compressed={entry.CompressedSize}; runtime={candidate.RuntimeVersion}");
                foreach (var m in matches)
                {
                    Console.WriteLine($"Filename literal in {m.FullName}");
                    for(int index=0;index<m.Body.Instructions.Count;index++)
                        if(m.Body.Instructions[index].OpCode==OpCodes.Ldstr && Equals(m.Body.Instructions[index].Operand,OldUiName))
                            foreach(var instruction in m.Body.Instructions.Skip(Math.Max(0,index-2)).Take(6)) Console.WriteLine(instruction.ToString());
                }
                foreach (var m in candidate.GetTypes().SelectMany(t => t.Methods).Where(m => m.HasBody))
                    foreach (var i in m.Body.Instructions.Where(i => i.OpCode == OpCodes.Ldstr && i.Operand is string s && s == "4unityofflinetestsunucusu")) Console.WriteLine($"Process-name literal in {m.FullName}");
            }
            if (module == null || application == null) throw new InvalidDataException("Exact filename-bearing module not found");
            using (module)
            {
                if (args[0] == "inspect") return 0;
                if (args[0] != "patch" || args.Length != 3) throw new ArgumentException("Expected a new patch output");
                PatchBundle(input, Path.GetFullPath(args[2]), application, module, entries);
            }
            return 0;
        }
        catch (Exception error) { Console.Error.WriteLine(error.GetType().Name + ": " + error.Message); return 1; }
    }

    static TypeDef InjectResolver(ModuleDefMD target)
    {
        using var own = ModuleDefMD.Load(typeof(Program).Assembly.Location);
        var source = own.Find("LoaderNameCompatibility.FileResolver", false) ?? throw new InvalidDataException("Resolver type missing");
        var importer = new Importer(target);
        var added = new TypeDefUser(source.Namespace, source.Name, target.CorLibTypes.Object.TypeDefOrRef) { Attributes = source.Attributes };
        target.Types.Add(added);
        var methods = new Dictionary<MethodDef, MethodDef>();
        foreach (var method in source.Methods.Where(m => m.Name == "ResolveUiFileName" || m.Name == "ResolveKnownFile"))
        {
            var copy = new MethodDefUser(method.Name, importer.Import(method.MethodSig), method.ImplAttributes, method.Attributes);
            foreach (var parameter in method.ParamDefs) copy.ParamDefs.Add(new ParamDefUser(parameter.Name, parameter.Sequence, parameter.Attributes));
            added.Methods.Add(copy); methods.Add(method, copy);
        }
        foreach (var (sourceMethod, copy) in methods)
        {
            var body = new CilBody { InitLocals = sourceMethod.Body.InitLocals, MaxStack = sourceMethod.Body.MaxStack, KeepOldMaxStack = true };
            copy.Body = body;
            var locals = new Dictionary<Local, Local>();
            foreach (var local in sourceMethod.Body.Variables)
            {
                var cloned = new Local(importer.Import(local.Type)); locals.Add(local, cloned); body.Variables.Add(cloned);
            }
            var instructions = sourceMethod.Body.Instructions.ToDictionary(i => i, i => new Instruction(i.OpCode));
            foreach (var original in sourceMethod.Body.Instructions)
            {
                object? operand = original.Operand;
                operand = operand switch
                {
                    Instruction branch => instructions[branch],
                    IList<Instruction> branches => branches.Select(i => instructions[i]).ToArray(),
                    Local local => locals[local],
                    Parameter parameter => copy.Parameters[parameter.Index],
                    MethodDef method when methods.ContainsKey(method) => methods[method],
                    IMethod method => importer.Import(method),
                    IField field => importer.Import(field),
                    ITypeDefOrRef type => importer.Import(type),
                    TypeSig type => importer.Import(type),
                    _ => operand
                };
                instructions[original].Operand = operand;
                body.Instructions.Add(instructions[original]);
            }
            foreach (var handler in sourceMethod.Body.ExceptionHandlers)
                body.ExceptionHandlers.Add(new ExceptionHandler(handler.HandlerType)
                {
                    CatchType = handler.CatchType == null ? null : importer.Import(handler.CatchType),
                    TryStart = handler.TryStart == null ? null : instructions[handler.TryStart],
                    TryEnd = handler.TryEnd == null ? null : instructions[handler.TryEnd],
                    HandlerStart = handler.HandlerStart == null ? null : instructions[handler.HandlerStart],
                    HandlerEnd = handler.HandlerEnd == null ? null : instructions[handler.HandlerEnd],
                    FilterStart = handler.FilterStart == null ? null : instructions[handler.FilterStart]
                });
        }
        return added;
    }

    static string Pattern(MethodDef method)
    {
        if (!method.HasBody) return "no-body";
        string Operand(object? operand) => operand switch
        {
            Instruction branch => "branch:" + method.Body.Instructions.IndexOf(branch),
            IList<Instruction> branches => string.Join(",", branches.Select(b => method.Body.Instructions.IndexOf(b))),
            Local local => "local:" + local.Index,
            Parameter parameter => "arg:" + parameter.Index,
            _ => operand?.ToString() ?? ""
        };
        string handlers = string.Join(";", method.Body.ExceptionHandlers.Select(h => $"{h.HandlerType}:{Operand(h.TryStart)}:{Operand(h.TryEnd)}:{Operand(h.HandlerStart)}:{Operand(h.HandlerEnd)}:{h.CatchType?.FullName}"));
        return string.Join("\n", method.Body.Instructions.Select(i => i.OpCode.Code + " " + Operand(i.Operand))) + "\nEH:" + handlers;
    }

    static void PatchBundle(string input, string output, BundleEntry entry, ModuleDefMD module, List<BundleEntry> entries)
    {
        if (File.Exists(output) || string.Equals(input, output, StringComparison.OrdinalIgnoreCase)) throw new IOException("Refusing overwrite");
        if (!module.IsILOnly || module.IsStrongNameSigned) throw new InvalidDataException("Unsupported native/signed application module");
        var originalMethods = module.GetTypes().SelectMany(t => t.Methods).ToArray();
        var before = originalMethods.ToDictionary(m => m.FullName, Pattern);
        var sites = originalMethods.Where(m => m.HasBody).SelectMany(m => m.Body.Instructions.Where(i => i.OpCode == OpCodes.Ldstr && Equals(i.Operand, OldUiName)).Select(i => (Method:m, Instruction:i))).ToArray();
        if (sites.Length != 2 || sites.Any(s => s.Method.FullName != "System.Void FourUnityOfflineLauncher.Program::Main(System.String[])")) throw new InvalidDataException("Filename sites differ from inspected build");
        var resolver = InjectResolver(module).Methods.Single(m => m.Name == "ResolveUiFileName");
        foreach (var site in sites) { site.Instruction.OpCode = OpCodes.Call; site.Instruction.Operand = resolver; }
        foreach (var method in originalMethods)
            if (!sites.Any(s => s.Method == method) && before[method.FullName] != Pattern(method)) throw new InvalidDataException("Unrelated method changed before write");
        var expectedPatterns = originalMethods.ToDictionary(m => m.FullName, Pattern);
        var options = new ModuleWriterOptions(module);
        options.MetadataOptions.Flags |= MetadataFlags.PreserveAll;
        options.WritePdb = false;
        using var dll = new MemoryStream(); module.Write(dll, options);
        byte[] patched = dll.ToArray();
        using (var check = ModuleDefMD.Load(patched))
        {
            var all = check.GetTypes().SelectMany(t => t.Methods).ToDictionary(m => m.FullName);
            foreach (var expected in expectedPatterns)
                if (!all.TryGetValue(expected.Key, out var method) || Pattern(method) != expected.Value) throw new InvalidDataException("Original method instruction/branch/EH verification failed: " + expected.Key);
            if (check.GetAssemblyRefs().Any(a => a.Name == "NameResolutionTool" || a.Name == "dnlib")) throw new InvalidDataException("Unexpected tool dependency");
        }
        Directory.CreateDirectory(Path.GetDirectoryName(output)!);
        using (var source = File.OpenRead(input))
        using (var destination = new FileStream(output, FileMode.CreateNew, FileAccess.ReadWrite, FileShare.None))
        {
            source.CopyTo(destination);
            long newOffset = (destination.Length+4095)&~4095L;
            destination.Position = newOffset; destination.Write(patched);
            destination.Position = entry.TablePosition;
            using var writer = new BinaryWriter(destination, System.Text.Encoding.UTF8, true);
            writer.Write(newOffset); writer.Write((long)patched.Length); writer.Write(0L); writer.Flush();
        }
        byte[] oldBytes = File.ReadAllBytes(input), newBytes = File.ReadAllBytes(output);
        for (int i = 0; i < oldBytes.Length; i++)
            if ((i < entry.TablePosition || i >= entry.TablePosition+24) && oldBytes[i] != newBytes[i]) throw new InvalidDataException("Unexpected change outside one bundle table record");
        var reread = ReadBundle(output);
        if (reread.Count != entries.Count || !Payload(output, reread.Single(e => e.Name == entry.Name)).SequenceEqual(patched)) throw new InvalidDataException("Rewritten bundle verification failed");
        foreach (var other in entries.Where(e => e.Name != entry.Name))
            if (other != reread.Single(e => e.Name == other.Name)) throw new InvalidDataException("Unrelated bundle entry changed");
        var receipt = new { SourceSHA256=OriginalBundleHash, OutputSHA256=Convert.ToHexString(SHA256.HashData(newBytes)), OutputBytes=newBytes.Length, Assembly=entry.Name, FilenameSitesReplaced=sites.Length, OriginalMethodsVerified=originalMethods.Length, UnchangedBundleEntries=entries.Count-1, OuterHostChange="Only the selected entry's 24-byte offset/size record; replacement IL assembly appended", Scope="Filename resolution only; no authentication, UI, AMIDEWIN backend or protection logic changes" };
        File.WriteAllText(output+".receipt.json", JsonSerializer.Serialize(receipt, new JsonSerializerOptions { WriteIndented=true }));
        Console.WriteLine(JsonSerializer.Serialize(receipt));
    }

    static int SelfTest()
    {
        string root=Path.Combine(Path.GetTempPath(),"loader-name-tests-"+Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(root);
        byte[] fixture=System.Text.Encoding.UTF8.GetBytes("name-resolution-fixture");
        string hash=Convert.ToHexString(SHA256.HashData(fixture));
        string path=Path.Combine(root,"herhangi bir ad.EXE");
        File.WriteAllBytes(path,fixture);
        if(LoaderNameCompatibility.FileResolver.ResolveKnownFile(root,hash,fixture.Length)!=path) throw new Exception("Arbitrary name failed");
        File.WriteAllBytes(Path.Combine(root,"yanlis.exe"),new byte[fixture.Length]);
        if(LoaderNameCompatibility.FileResolver.ResolveKnownFile(root,hash,fixture.Length)!=path) throw new Exception("Wrong-content rejection failed");
        string renamed=Path.Combine(root,"baska-ad.exe"); File.Move(path,renamed);
        if(LoaderNameCompatibility.FileResolver.ResolveKnownFile(root,hash,fixture.Length)!=renamed) throw new Exception("Rename failed");
        File.WriteAllBytes(Path.Combine(root,"duplicate.exe"),fixture);
        try { LoaderNameCompatibility.FileResolver.ResolveKnownFile(root,hash,fixture.Length); throw new Exception("Duplicate accepted"); }
        catch(InvalidOperationException) { }
        string empty=Path.Combine(root,"empty");Directory.CreateDirectory(empty);
        try { LoaderNameCompatibility.FileResolver.ResolveKnownFile(empty,hash,fixture.Length); throw new Exception("Missing accepted"); }
        catch(FileNotFoundException) { }
        Console.WriteLine("5 resolver checks passed: arbitrary name/extension case, wrong content, rename, duplicate refusal, missing refusal. Fixture retained: "+root);
        return 0;
    }
}
