using System.Security.Cryptography;

namespace LoaderNameCompatibility;

public static class FileResolver
{
    public const string UiHash = "8730BEBE0F4B72D42D4DC6D6FD31A2F2193DD928ED971DDB353EEA775C79285C";
    public const long UiBytes = 15990784;

    public static string ResolveUiFileName() => Path.GetFileName(ResolveKnownFile(AppContext.BaseDirectory, UiHash, UiBytes));

    public static string ResolveKnownFile(string directory, string expectedHash, long expectedBytes)
    {
        string? selected = null;
        foreach (string path in Directory.GetFiles(directory, "*", SearchOption.TopDirectoryOnly))
        {
            if (!string.Equals(Path.GetExtension(path), ".exe", StringComparison.OrdinalIgnoreCase)) continue;
            if (new FileInfo(path).Length != expectedBytes) continue;
            string actual;
            try { actual = Convert.ToHexString(SHA256.HashData(File.ReadAllBytes(path))); }
            catch (IOException) { continue; }
            catch (UnauthorizedAccessException) { continue; }
            if (!string.Equals(actual, expectedHash, StringComparison.OrdinalIgnoreCase)) continue;
            if (selected != null) throw new InvalidOperationException("Ayni uygulamanin birden fazla kopyasi var. Klasorde tek bir ana uygulama EXE'si birakin.");
            selected = path;
        }
        if (selected == null) throw new FileNotFoundException("Ana uygulama bu klasorde bulunamadi veya dosya icerigi beklenen surumle eslesmiyor.");
        return selected;
    }
}
