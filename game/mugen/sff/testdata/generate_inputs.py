"""Generate original PNG inputs for formats.def using only Python's stdlib.

Run in this directory, then invoke the supplied reference sprmake2.exe on
formats.def. PNG inputs are disposable; formats.sff contains only these authored
test patterns, not Elecbyte art. Reference tool: SprMaker 2.01beta (2010-06-16).
"""
import struct
import zlib
from pathlib import Path


def chunk(kind, data):
    return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data))


def png(name, w, h, kind, rows, palette=None):
    data = b'\x89PNG\r\n\x1a\n'
    data += chunk(b'IHDR', struct.pack('>2I5B', w, h, 8, kind, 0, 0, 0))
    if palette is not None:
        data += chunk(b'PLTE', palette)
    data += chunk(b'IDAT', zlib.compress(b''.join(b'\0' + row for row in rows)))
    data += chunk(b'IEND', b'')
    Path(name).write_bytes(data)


palette = bytes(v for i in range(256) for v in (i * 17 % 256, i * 53 % 256, i * 97 % 256))
png('indexed5.png', 64, 8, 3,
    [bytes((x // 4 + y) % 31 for x in range(64)) for y in range(8)], palette)
png('indexed8.png', 64, 8, 3,
    [bytes((x * 13 + y * 7) % 256 for x in range(64)) for y in range(8)], palette)
for channels, kind, name in [(3, 2, 'rgb'), (4, 6, 'rgba')]:
    rows = [bytes(v for x in range(8) for v in
                  ([x * 31 % 256, y * 67, 75] + ([x * 31] if channels == 4 else [])))
            for y in range(4)]
    png(name + '.png', 8, 4, kind, rows)
    png(name + '-raw.png', 8, 4, kind, rows)
