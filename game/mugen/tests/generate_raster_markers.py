#!/usr/bin/env python3
"""Generate original patterned RGB and indexed SFF 2.01 raster controls."""
import argparse
from pathlib import Path
import struct


def archive(indexed=False, fine=False, lz5=False, power2=False):
    width, height, axis_x, axis_y = (32, 32, 16, 32) if power2 else (47, 106, 18, 105)
    colors = [(0, 0, 0), (220, 30, 40), (30, 210, 50),
              (40, 60, 230), (230, 200, 30)]
    indices = bytes(1 + (x // 3 + y // 5) % 4
                    for y in range(height) for x in range(width))
    if fine:
        indices = bytes(0 if (x * 7 + y * 11) % 13 < 3 else 1 + (x + y * 3) % 4
                        for y in range(height) for x in range(width))
    if indexed:
        # RLE8 values below 64 are literal bytes. No external compiler is used.
        payload = struct.pack('<I', len(indices)) + indices
        palette_count = 32 if lz5 else 256
        palette = bytes(v for i in range(palette_count)
                        for v in (*colors[i % len(colors)], 255))
        palette_node = struct.pack('<4H2I', 1, 1, palette_count, 0, 0, len(palette))
        sprite_format, depth = 2, 8
        if lz5:
            # Literal-only LZ5 packets: zero control bits, one-pixel RLE runs.
            payload = struct.pack('<I', len(indices)) + b''.join(
                b'\0' + bytes(32 | value for value in indices[i:i+8])
                for i in range(0, len(indices), 8))
            sprite_format, depth = 4, 5
    else:
        payload = bytes(v for i in indices for v in colors[i])
        palette, palette_node = b'', b''
        sprite_format, depth = 0, 24
    node = struct.pack('<7H2B2I2H', 0, 0, width, height, axis_x, axis_y, 0,
                       sprite_format, depth, len(palette), len(payload), 0, 0)
    header = bytearray(512)
    header[:12] = b'ElecbyteSpr\0'
    header[12:16] = bytes([0, 1, 0, 2])
    data_offset = len(header) + len(node) + len(palette_node)
    struct.pack_into('<8I', header, 36, len(header), 1,
                     len(header) + len(node), int(indexed),
                     data_offset, len(palette) + len(payload),
                     data_offset + len(palette) + len(payload), 0)
    return bytes(header) + node + palette_node + palette + payload


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', type=Path)
    parser.add_argument('--indexed', action='store_true')
    parser.add_argument('--fine', action='store_true')
    parser.add_argument('--lz5', action='store_true')
    parser.add_argument('--power2', action='store_true')
    args = parser.parse_args()
    if args.lz5 and not args.indexed:
        parser.error('LZ5 requires indexed pixels')
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(archive(args.indexed, args.fine, args.lz5, args.power2))
