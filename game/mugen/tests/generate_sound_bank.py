#!/usr/bin/env python3
"""Generate the original small tone bank used by native sound regressions."""
import argparse
import io
import math
from pathlib import Path
import struct
import wave


def generate():
    sounds = []
    for group, frames, channels in [(1,100,1),(2,100,1),(3,100,1),(4,44100,1),(5,100,2)]:
        data = bytearray()
        for frame in range(frames):
            for channel in range(channels):
                cycles = group if group <= 3 else (1 if channels == 1 else channel + 1)
                data += struct.pack('<h',round(10000 * math.sin(2 * math.pi * cycles * frame / 100)))
        output = io.BytesIO()
        with wave.open(output,'wb') as wav:
            wav.setnchannels(channels)
            wav.setsampwidth(2)
            wav.setframerate(44100)
            wav.writeframes(data)
        sounds.append((group,output.getvalue()))
    header = b'ElecbyteSnd\0' + bytes([4,0,0,0]) + struct.pack('<II',len(sounds),512)
    assert len(header) == 24
    bank = bytearray(header.ljust(512,b'\0'))
    for group,payload in sounds:
        bank += struct.pack('<IIII',len(bank)+16+len(payload),len(payload),group,0) + payload
    return bank


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('destination',type=Path)
    args = parser.parse_args()
    args.destination.write_bytes(generate())
