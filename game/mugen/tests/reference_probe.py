#!/usr/bin/env python3
"""Prepare/run a numeric animation probe against the supplied Elecbyte engine.

Requires Wine, DISPLAY, xdotool and ImageMagick import for capture. No keyboard,
focus or pointer changes are made. Capture only the spawned process's one visible
window. Numeric clipboards are saved as PNG evidence under the disposable copy's
parent; interpretation is deliberately separate from runtime assertions.
"""
import argparse
import hashlib
import io
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import struct
import time


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def prepare(reference, destination, fixture):
    shutil.copytree(reference, destination)
    char = destination / 'chars/kfm'
    definition = (char / 'kfm.def').read_text(encoding='utf-8-sig')
    for key, value in [('cns', 'probe.cns'), ('st', 'probe.cns'),
                       ('stcommon', ''), ('cmd', 'probe.cmd'), ('anim', 'probe.air')]:
        definition = re.sub(r'(?im)^\s*' + key + r'\s*=.*$',
                            key + ' = ' + value, definition)
    (char / 'kfm.def').write_text(definition)
    commands = re.split(r'(?im)^\s*\[statedef', (char / 'kfm.cmd').read_text())[0]
    commands += '\n[Statedef -1]\n[State -1, no input]\ntype=Null\ntrigger1=1\n'
    (char / 'probe.cmd').write_text(commands)
    shutil.copyfile(fixture / 'animation-loop.air', char / 'probe.air')
    constants = re.split(r'(?im)^\s*\[statedef', (char / 'kfm.cns').read_text())[0]
    source = constants + '\n' + (fixture / 'animation-loop.cns').read_text()
    config = destination / 'data/mugen.cfg'
    config.write_text(re.sub(r'(?im)^Debug\s*=\s*0\s*$', 'Debug = 1', config.read_text()))
    return source


def page_source(base, page):
    for chunk in range(2):
        start = page + chunk * 6
        base += ('\n[State 0, dump]\ntype=' +
                 ('DisplayToClipboard' if chunk == 0 else 'AppendToClipboard') +
                 '\ntrigger1=time>=30\ntext="P' + str(start) + ':' +
                 ','.join(['%d'] * 6) + '\\n"\nparams=' +
                 ','.join('var(' + str(i) + ')' for i in range(start, start + 6)) + '\n')
    return base


def capture(run, prefix, source, page, seconds):
    (run / 'chars/kfm/probe.cns').write_text(page_source(source, page))
    env = os.environ.copy()
    env.update(WINEARCH='win32', WINEPREFIX=str(prefix), WINEDEBUG='-all')
    output = run.parent / ('animation-page' + str(page) + '.png')
    with (run.parent / 'animation-probe.log').open('w') as log:
        process = subprocess.Popen(['wine', 'mugen.exe', 'kfm', 'kfm', '-s', 'kfm', '-nojoy'],
                                   cwd=run, env=env, stdout=log, stderr=log)
        try:
            time.sleep(seconds)
            if process.poll() is not None:
                raise RuntimeError('Reference exited early; inspect its status/probe logs')
            windows = subprocess.run(['xdotool', 'search', '--onlyvisible', '--pid', str(process.pid)],
                                     capture_output=True, text=True, check=False).stdout.split()
            if len(windows) != 1:
                raise RuntimeError('Expected exactly one visible window for the spawned reference process')
            subprocess.run(['import', '-window', windows[0], str(output)],
                           check=True, stdout=log, stderr=log, timeout=5)
            shutil.copyfile(run / 'mugen.log', run.parent / ('animation-page' + str(page) + '.log'))
        finally:
            subprocess.run(['wineserver', '-k'], env=env, stdout=log, stderr=log, check=False)
            try:
                process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
    return output


def read_clipboard(path, reference, page):
    """Read exact debug glyph masks, failing closed on any ambiguous capture.

    This is template matching against the bundled bitmap, not approximate OCR.
    It needs Pillow only for PNG/PCX decoding. Original captures remain available
    for human inspection independently of these decoded observations.
    """
    from PIL import Image
    data = (reference / 'font/f-4x6.fnt').read_bytes()
    po, ps, to, ts = struct.unpack_from('<4I', data, 16)
    font = Image.open(io.BytesIO(data[po:po + ps]))
    glyphs = {}
    for line in data[to:to + ts].decode('latin1').split('[Map]')[1].splitlines():
        fields = line.split()
        if len(fields) < 3 or fields[0] not in 'P0123456789:,':
            continue
        x, width = map(int, fields[1:3])
        glyphs[fields[0]] = (width, tuple(font.getpixel((x + dx, y)) != 0
                                       for y in range(6) for dx in range(width)))
    image = Image.open(path).convert('RGB')
    white = [[min(c) >= 230 and max(c) - min(c) < 5
              for c in [image.getpixel((x, y)) for x in range(image.width)]]
             for y in range(image.height)]

    def matches(char, x, y):
        width, mask = glyphs[char]
        return mask == tuple(white[y + dy][x + dx] for dy in range(6) for dx in range(width))

    lines = {}
    for y in range(max(0, image.height - 80), image.height - 5):
        for x in range(20):
            if not matches('P', x, y):
                continue
            text, column = '', x
            for _ in range(100):
                candidates = [c for c in glyphs if matches(c, column, y)]
                if len(candidates) != 1:
                    break
                char = candidates[0]
                text += char
                column += glyphs[char][0] + 1
                if column + 5 >= image.width:
                    break
            for first in [page, page + 6]:
                if re.fullmatch('P' + str(first) + r':\d+(,\d+){5}', text):
                    if first in lines:
                        raise RuntimeError('Ambiguous duplicate clipboard line')
                    lines[first] = [int(n) for n in text.split(':')[1].split(',')]
    if len(lines) != 2:
        raise RuntimeError('Could not read both numeric clipboard lines exactly; inspect capture')
    return {first + i: value for first, values in lines.items() for i, value in enumerate(values)}


def main():
    project = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--reference', type=Path, default=project / 'temp/mugen-1.1b1')
    parser.add_argument('--name', default='animation-loop')
    parser.add_argument('--seconds', type=float, default=6)
    parser.add_argument('--pages', type=int, nargs='+', default=[0, 12, 30, 42])
    args = parser.parse_args()
    if not re.fullmatch('[a-z0-9_-]+', args.name) or not 3 <= args.seconds <= 30:
        parser.error('Use a simple run name and a capture delay from 3 to 30 seconds')
    if any(page < 0 or page > 48 for page in args.pages):
        parser.error('Pages must begin from variable indices 0 to 48')
    reference = args.reference.resolve()
    work = project / 'temp/reference-runs' / args.name
    work.mkdir(parents=True, exist_ok=False)
    run = work / 'stock'
    fixture = project / 'tests/testdata/reference'
    base = prepare(reference, run, fixture)
    evidence = {'engine_sha256': digest(reference / 'mugen.exe'),
                'cns_sha256': digest(fixture / 'animation-loop.cns'),
                'air_sha256': digest(fixture / 'animation-loop.air'),
                'invocation': 'wine mugen.exe kfm kfm -s kfm -nojoy', 'captures': []}
    expected = json.loads((fixture / 'animation-loop.json').read_text())
    for key in ['engine_sha256', 'cns_sha256', 'air_sha256']:
        if evidence[key] != expected[key]:
            raise RuntimeError('Pinned reference/fixture hash mismatch: ' + key)
    expected_variables = {int(index): value for capture in expected['captures']
                          for index, value in capture['variables'].items()}
    for page in args.pages:
        path = capture(run, project / 'temp/wine-reference', base, page, args.seconds)
        variables = read_clipboard(path, reference, page)
        evidence['captures'].append({'page': page, 'path': path.name, 'sha256': digest(path),
                                     'variables': variables})
        for index, value in variables.items():
            if index not in expected_variables or expected_variables[index] != value:
                raise RuntimeError('Reference observation mismatch at variable ' + str(index))
        print('Captured', path, flush=True)
    (work / 'evidence.json').write_text(json.dumps(evidence, indent=2) + '\n')


if __name__ == '__main__':
    main()
