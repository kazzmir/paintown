#!/usr/bin/env python3
"""Write an original raw RGB SFF v2 bank for actor draw-order probes.

No reference art or external sprite compiler is used. Each 32x32 marker has
axis (16,32). Items 0..3 are red, green, blue and yellow, respectively.
"""
import argparse
from pathlib import Path
import struct


def archive():
    colors = [(220, 30, 40), (30, 210, 50), (40, 60, 230), (230, 200, 30)]
    nodes, pixels = bytearray(), bytearray()
    for item, rgb in enumerate(colors):
        payload = bytes(rgb) * (32 * 32)
        nodes += struct.pack('<7H2B2I2H', 0, item, 32, 32, 16, 32, 0,
                             0, 24, len(pixels), len(payload), 0, 0)
        pixels += payload
    header = bytearray(512)
    header[:12] = b'ElecbyteSpr\0'
    header[12:16] = bytes([0, 1, 0, 2])
    data_offset = len(header) + len(nodes)
    struct.pack_into('<8I', header, 36, len(header), len(colors), data_offset, 0,
                     data_offset, len(pixels), data_offset + len(pixels), 0)
    return bytes(header + nodes + pixels)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(archive())
