"""Generate ABI-preserving forwarding stubs from this machine's system DLL.

Generated files are build artifacts. The system DLL itself is never modified.
"""
from pathlib import Path
import json
import sys
import pefile

source, output = (Path(value).resolve() for value in sys.argv[1:])
image = pefile.PE(str(source))
assert image.FILE_HEADER.Machine == 0x8664
symbols = sorted(image.DIRECTORY_ENTRY_EXPORT.symbols, key=lambda item: item.ordinal)
custom = {
    'InternetOpenUrlA': 'HookInternetOpenUrlA',
    'InternetOpenUrlW': 'HookInternetOpenUrlW',
    'InternetReadFile': 'HookInternetReadFile',
    'InternetCloseHandle': 'HookInternetCloseHandle',
}
definitions = ['LIBRARY WININET', 'EXPORTS']
header = ['struct ForwardExport { const char* name; unsigned short ordinal; };', 'constexpr ForwardExport ForwardExports[] = {']
assembly = ['option casemap:none', 'EXTERN ResolveForward:PROC', '.code']
for index, symbol in enumerate(symbols):
    name = symbol.name.decode('ascii') if symbol.name else None
    label = custom.get(name, f'Forward_{index}')
    private = ' PRIVATE' if name in ('DllCanUnloadNow', 'DllGetClassObject', 'DllInstall', 'DllRegisterServer', 'DllUnregisterServer') else ''
    definitions.append(f'    {name or f"ordinal_{symbol.ordinal}"}={label} @{symbol.ordinal}' + ('' if name else ' NONAME') + private)
    header.append('    {' + (json.dumps(name) if name else 'nullptr') + f', {symbol.ordinal}' + '},')
    if name in custom:
        continue
    assembly += [f'PUBLIC {label}', f'{label} PROC FRAME', '    sub rsp, 0A8h', '    .allocstack 0A8h', '    .endprolog']
    for register, offset in [('rcx', '20'), ('rdx', '28'), ('r8', '30'), ('r9', '38')]:
        assembly.append(f'    mov [rsp+{offset}h], {register}')
    for register in range(4):
        assembly.append(f'    movdqu [rsp+{0x40 + register * 16:X}h], xmm{register}')
    assembly += [f'    mov ecx, {index}', '    call ResolveForward']
    for register in range(4):
        assembly.append(f'    movdqu xmm{register}, [rsp+{0x40 + register * 16:X}h]')
    for register, offset in [('rcx', '20'), ('rdx', '28'), ('r8', '30'), ('r9', '38')]:
        assembly.append(f'    mov {register}, [rsp+{offset}h]')
    assembly += ['    add rsp, 0A8h', '    jmp rax', f'{label} ENDP']
header.append('};')
assembly.append('END')
output.mkdir(parents=True, exist_ok=True)
for name, lines in [('WinInetProxy.def', definitions), ('WinInetExports.h', header), ('WinInetForwarders.asm', assembly)]:
    (output / name).write_text('\n'.join(lines) + '\n', encoding='ascii')
print(f'Generated {len(symbols)} exports; four URL/stream extensions, remaining exports forwarded unchanged.')
