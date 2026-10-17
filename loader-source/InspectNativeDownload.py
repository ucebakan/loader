"""Read-only download-boundary inspection; no process or driver execution."""
from pathlib import Path
import hashlib
import struct
import sys
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_64

source = Path(r"C:\loader\ui-hook-project\evidence\runtime-analysis\loaderv2_runtime_analysis.exe")
data = source.read_bytes()
assert hashlib.sha256(data).hexdigest() == "77cd31a520e1956ac1606f4fdbf962afed6fdb8308b23d6473fd78e3d1ecb787"
decoder = Cs(CS_ARCH_X86, CS_MODE_64)
if len(sys.argv) == 3:
    start, end = (int(value, 0) for value in sys.argv[1:])
    for inst in decoder.disasm(data[start:end], start):
        print(f"{inst.address:#x}: {inst.mnemonic} {inst.op_str}")
    sys.exit(0)
targets = {0x132ad0, 0x134010, 0x134030, 0x134048}
for pos in range(0x1000, 0x10dbf0 - 7):
    if data[pos] in (0x48, 0x4c) and data[pos + 1] == 0x8d and data[pos + 2] & 0xc7 == 5:
        target = pos + 7 + struct.unpack_from("<i", data, pos + 3)[0]
        if target in targets:
            print(f"REFERENCE {pos:#x} -> {target:#x}")
            for inst in decoder.disasm(data[pos:pos + 200], pos):
                print(f"{inst.address:#x}: {inst.mnemonic} {inst.op_str}")
image = pefile.PE(data=data)
for entry in getattr(image, "DIRECTORY_ENTRY_IMPORT", []):
    if entry.dll.lower() in (b"wininet.dll", b"urlmon.dll", b"libcurl-x64.dll"):
        print("IMPORT", entry.dll.decode(), [(hex(i.address - image.OPTIONAL_HEADER.ImageBase), i.name) for i in entry.imports])
ami = Path(r"C:\loader\ui-hook-project\output\offline520-20261017\AMITools\AMIDEWINx64.EXE").read_bytes()
print("AMI banner bytes", repr(ami[0x4c730:0x4c820]))

# Reconstruct just the downloader's URL argument from the caller's constants.
# The reviewed XOR loop at 0x97fb0 starts at 0x45; 0x963c0 copies 61 bytes.
encrypted = (data[0x135840:0x135850] + data[0x135b20:0x135b30] +
             data[0x135b00:0x135b10] + struct.pack("<IIIH", 0x57011311, 0x18240912, 0xe509170f, 0xf3))
url = bytes(value ^ (0x45 + index) for index, value in enumerate(encrypted[:61])).decode("ascii")
assert url == "https://pub-fdaae9442b754a829739a8850fcc81a5.r2.dev/ks_driver"
print("Verified URL passed to download routine 0x84960:", url)
