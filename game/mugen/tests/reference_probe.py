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


class ReferenceRejected(RuntimeError):
    def __init__(self, text):
        self.text = text
        super().__init__('Reference rejected fixture:\n' + text.split('Clipboard tail:')[0])


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def prepare(reference, destination, fixture, bare=False, version=None, files=None, omitted=None, state_order=None, localcoord=None, p2_localcoord=None):
    shutil.copytree(reference, destination)
    char = destination / 'chars/kfm'
    definition = (char / 'kfm.def').read_text(encoding='utf-8-sig')
    for key, value in [('cns', 'probe.cns'), ('st', 'probe.cns'),
                       ('stcommon', ''), ('cmd', 'probe.cmd'), ('anim', 'probe.air')]:
        definition = re.sub(r'(?im)^\s*' + key + r'\s*=.*$',
                            key + ' = ' + value, definition)
    if version is not None:
        definition = re.sub(r'(?im)^\s*mugenversion\s*=.*$', 'mugenversion = ' + version, definition)
    if localcoord is not None:
        definition, count = re.subn(r'(?im)^\s*localcoord\s*=.*$', 'localcoord = ' + localcoord, definition)
        if not count:
            definition = re.sub(r'(?im)^(\s*\[info\]\s*)$',
                                lambda match: match.group(1) + '\nlocalcoord = ' + localcoord, definition)
    for key, value in (files or {}).items():
        definition, count = re.subn(r'(?im)^\s*' + re.escape(key) + r'\s*=.*$',
                                    lambda match: key + ' = ' + value, definition)
        if not count:
            definition = re.sub(r'(?im)^(\s*\[files\]\s*)$',
                                lambda match: match.group(1) + '\n' + key + ' = ' + value, definition)
    for key in omitted or []:
        definition = re.sub(r'(?im)^\s*' + re.escape(key) + r'\s*=.*\n?', '', definition)
    if state_order:
        sections = re.split(r'(?im)(^\s*\[[^\r\n]+\]\s*$)', definition)
        for index in range(1, len(sections), 2):
            if sections[index].strip().lower() != '[files]':
                continue
            lines, state_lines = [], {}
            for line in sections[index + 1].splitlines():
                match = re.match(r'\s*(st\d*)\s*=', line, re.I)
                if match:
                    state_lines[match.group(1).lower()] = line
                else:
                    lines.append(line)
            keys = state_order + [key for key in state_lines if key not in state_order]
            sections[index + 1] = '\n'.join(lines + [state_lines[key] for key in keys if key in state_lines]) + '\n'
        definition = ''.join(sections)
    (char / 'kfm.def').write_text(definition)
    extras = fixture.with_suffix('.extra')
    if extras.exists():
        shutil.copytree(extras, char, dirs_exist_ok=True)
    commands = re.split(r'(?im)^\s*\[statedef', (char / 'kfm.cmd').read_text())[0]
    command_fixture = fixture.with_suffix('.cmd')
    commands += ('\n' + command_fixture.read_text() if command_fixture.exists() else
                 '\n[Statedef -1]\n[State -1, no input]\ntype=Null\ntrigger1=1\n')
    (char / 'probe.cmd').write_text(commands)
    shutil.copyfile(fixture.with_suffix('.air'), char / 'probe.air')
    constants = re.split(r'(?im)^\s*\[statedef', (char / 'kfm.cns').read_text())[0]
    if bare:
        constants = '[Data]\n[Size]\n[Velocity]\n[Movement]\n'
    source = constants + '\n' + fixture.with_suffix('.cns').read_text()
    if p2_localcoord is not None:
        opponent = destination / 'chars/probeopponent'
        shutil.copytree(char, opponent)
        opponent_definition, count = re.subn(r'(?im)^\s*localcoord\s*=.*$',
                                             'localcoord = ' + p2_localcoord, definition)
        if not count:
            opponent_definition = re.sub(r'(?im)^(\s*\[info\]\s*)$',
                                          lambda match: match.group(1) + '\nlocalcoord = ' + p2_localcoord,
                                          opponent_definition)
        (opponent / 'probeopponent.def').write_text(opponent_definition)
    config = destination / 'data/mugen.cfg'
    config.write_text(re.sub(r'(?im)^Debug\s*=\s*0\s*$', 'Debug = 1', config.read_text()))
    return source


def page_source(base, page, bank='var'):
    owner = re.findall(r'(?im)^\s*\[statedef\s+(-?\d+)\]', base)[-1]
    for chunk in range(2):
        start = page + chunk * 6
        base += ('\n[State ' + owner + ', dump]\ntype=' +
                 ('DisplayToClipboard' if chunk == 0 else 'AppendToClipboard') +
                 '\ntrigger1=time>=30\ntext="P' + str(start) + ':' +
                 ','.join(['%.9g' if bank == 'fvar' else '%d'] * 6) + '\\n"\nparams=' +
                 ','.join(bank + '(' + str(i) + ')' for i in range(start, start + 6)) + '\n')
    return base


def capture(run, prefix, source, page, seconds, bank='var', clipboard_in_cmd=False):
    if clipboard_in_cmd:
        commands = run / 'chars/kfm/probe.cmd'
        base = re.split(r'(?im)\n\[State -?\d+, dump\]', commands.read_text())[0]
        commands.write_text(page_source(base, page, bank))
        (run / 'chars/kfm/probe.cns').write_text(source)
    else:
        (run / 'chars/kfm/probe.cns').write_text(page_source(source, page, bank))
    opponent = run / 'chars/probeopponent'
    if opponent.exists():
        (opponent / 'probe.cns').write_text(source)
    env = os.environ.copy()
    env.update(WINEARCH='win32', WINEPREFIX=str(prefix), WINEDEBUG='-all')
    output = run.parent / ('animation-page' + str(page) + '.png')
    with (run.parent / 'animation-probe.log').open('w') as log:
        process = subprocess.Popen(['wine', 'mugen.exe', 'kfm', 'probeopponent' if opponent.exists() else 'kfm', '-s', 'kfm', '-nojoy'],
                                   cwd=run, env=env, stdout=log, stderr=log)
        try:
            time.sleep(seconds)
            if process.poll() is not None:
                raise RuntimeError('Reference exited early; inspect its status/probe logs')
            windows = subprocess.run(['xdotool', 'search', '--onlyvisible', '--pid', str(process.pid)],
                                     capture_output=True, text=True, check=False).stdout.split()
            if len(windows) != 1:
                helper = prefix.parent / 'reference-tools/wintext.exe'
                if helper.exists():
                    result = subprocess.run(['wine', str(helper)], env=env,
                                            capture_output=True, text=True, timeout=5)
                    (run.parent / 'load-error.txt').write_text(result.stdout + result.stderr)
                    if result.returncode == 0:
                        raise ReferenceRejected(result.stdout)
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
        if len(fields) < 3 or fields[0] not in 'PE0123456789:,.+-':
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
                number = r'[+-]?\d+(?:\.\d+)?(?:E[+-]?\d+)?'
                if re.fullmatch('P' + str(first) + ':' + number + '(' + ',' + number + '){5}', text):
                    if first in lines:
                        raise RuntimeError('Ambiguous duplicate clipboard line')
                    lines[first] = [float(n) if any(c in n for c in '.E') else int(n)
                                    for n in text.split(':')[1].split(',')]
    if len(lines) != 2:
        raise RuntimeError('Could not read both numeric clipboard lines exactly; inspect capture')
    return {first + i: value for first, values in lines.items() for i, value in enumerate(values)}


def main():
    project = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--reference', type=Path, default=project / 'temp/mugen-1.1b1')
    parser.add_argument('--name', default='animation-loop')
    parser.add_argument('--seconds', type=float, default=6)
    parser.add_argument('--fixture', default='animation-loop')
    parser.add_argument('--record', action='store_true', help='Record a new fixture without comparing prior observations')
    parser.add_argument('--bare-constants', action='store_true', help='Use empty constant groups for omission/load-error probes')
    parser.add_argument('--bank', choices=['var', 'fvar'], default='var')
    parser.add_argument('--mugenversion', choices=['1.0', '1.1'])
    parser.add_argument('--file', action='append', default=[], help='Override one character Files entry: key=value')
    parser.add_argument('--clipboard-in-cmd', action='store_true')
    parser.add_argument('--omit-file', action='append', default=[])
    parser.add_argument('--case', help='Use a pinned named case from the fixture JSON')
    parser.add_argument('--state-file-order', nargs='+')
    parser.add_argument('--localcoord', help='Override character localcoord: width,height')
    parser.add_argument('--p2-localcoord', help='Use a separate probe opponent with this localcoord')
    parser.add_argument('--pages', type=int, nargs='+', default=[0, 12, 30, 42])
    args = parser.parse_args()
    case_expected = None
    if args.case:
        if not re.fullmatch('[a-z0-9_-]+', args.fixture):
            parser.error('Invalid fixture name')
        catalogue = json.loads((project / 'tests/testdata/reference' / (args.fixture + '.json')).read_text())
        if args.case not in catalogue.get('cases', {}):
            parser.error('Unknown fixture case')
        case_expected = catalogue['cases'][args.case]
        args.file = [key + '=' + value for key, value in case_expected.get('files', {}).items()]
        args.omit_file = case_expected.get('omitted_files', [])
        args.clipboard_in_cmd = case_expected.get('clipboard_in_cmd', False)
        args.bank = case_expected.get('bank', 'var')
        args.mugenversion = case_expected.get('mugenversion')
        args.bare_constants = case_expected.get('bare_constants', False)
        args.pages = [capture['page'] for capture in case_expected.get('captures', [])] or [0]
        args.state_file_order = case_expected.get('state_file_order')
        args.localcoord = case_expected.get('localcoord')
        args.p2_localcoord = case_expected.get('p2_localcoord')
    if not all(re.fullmatch('[a-z0-9_-]+', value) for value in [args.name, args.fixture]) or not 3 <= args.seconds <= 30:
        parser.error('Use a simple run name and a capture delay from 3 to 30 seconds')
    if any(page < 0 or page > (28 if args.bank == 'fvar' else 48) for page in args.pages):
        parser.error('Page exceeds the variable bank')
    for profile in (args.localcoord, args.p2_localcoord):
        if profile is not None and (not re.fullmatch(r'[1-9][0-9]*(?:\.[0-9]+)?,[1-9][0-9]*(?:\.[0-9]+)?', profile) or
                                    any(float(value) > 2147483647 for value in profile.split(','))):
            parser.error('Localcoord requires positive numeric width,height')
    reference = args.reference.resolve()
    files = {}
    for item in args.file:
        key, separator, value = item.partition('=')
        if not separator or not re.fullmatch('[a-z0-9_.]+', key):
            parser.error('Files overrides use lowercase key=value')
        files[key] = value
    if any(not re.fullmatch('[a-z0-9_.]+', key) or key in files for key in args.omit_file):
        parser.error('Omitted file keys must be simple names and cannot also be overridden')
    work = project / 'temp/reference-runs' / args.name
    work.mkdir(parents=True, exist_ok=False)
    run = work / 'stock'
    fixture = project / 'tests/testdata/reference' / args.fixture
    if args.state_file_order and (len(set(args.state_file_order)) != len(args.state_file_order) or
                                 any(not re.fullmatch('st[0-9]*', key) for key in args.state_file_order)):
        parser.error('State-file order must contain unique st/stN keys')
    base = prepare(reference, run, fixture, args.bare_constants, args.mugenversion, files, args.omit_file, args.state_file_order, args.localcoord, args.p2_localcoord)
    evidence = {'engine_sha256': digest(reference / 'mugen.exe'),
                'cns_sha256': digest(fixture.with_suffix('.cns')),
                'air_sha256': digest(fixture.with_suffix('.air')),
                'invocation': 'wine mugen.exe kfm ' + ('probeopponent' if args.p2_localcoord else 'kfm') + ' -s kfm -nojoy', 'captures': [],
                'bank': args.bank, 'bare_constants': args.bare_constants}
    if files:
        evidence['files'] = files
    if args.clipboard_in_cmd:
        evidence['clipboard_in_cmd'] = True
    if args.omit_file:
        evidence['omitted_files'] = args.omit_file
    if args.state_file_order:
        evidence['state_file_order'] = args.state_file_order
    extras = fixture.with_suffix('.extra')
    if extras.exists():
        evidence['extra_sha256'] = {str(path.relative_to(extras)): digest(path)
                                    for path in sorted(extras.rglob('*')) if path.is_file()}
    if args.mugenversion is not None:
        evidence['mugenversion'] = args.mugenversion
    if args.localcoord is not None:
        evidence['localcoord'] = args.localcoord
    if args.p2_localcoord is not None:
        evidence['p2_localcoord'] = args.p2_localcoord
    if fixture.with_suffix('.cmd').exists():
        evidence['cmd_sha256'] = digest(fixture.with_suffix('.cmd'))
    expected_variables = {}
    expected = {}
    if not args.record:
        expected = case_expected or json.loads(fixture.with_suffix('.json').read_text())
        if expected.get('bank', 'var') != args.bank or expected.get('bare_constants', False) != args.bare_constants:
            raise RuntimeError('Variable bank/constants mode differs from pinned fixture')
        if expected.get('mugenversion') != args.mugenversion:
            raise RuntimeError('Character version differs from pinned fixture')
        if expected.get('localcoord') != args.localcoord:
            raise RuntimeError('Character localcoord differs from pinned fixture')
        if expected.get('p2_localcoord') != args.p2_localcoord:
            raise RuntimeError('Opponent localcoord differs from pinned fixture')
        if expected.get('files', {}) != files or expected.get('clipboard_in_cmd', False) != args.clipboard_in_cmd:
            raise RuntimeError('Files/clipboard mode differs from pinned fixture')
        if expected.get('omitted_files', []) != args.omit_file:
            raise RuntimeError('Omitted files differ from pinned fixture')
        if expected.get('state_file_order') != args.state_file_order:
            raise RuntimeError('State-file declaration order differs from pinned fixture')
        for key in [key for key in evidence if key.endswith('_sha256')]:
            if evidence[key] != expected[key]:
                raise RuntimeError('Pinned reference/fixture hash mismatch: ' + key)
        expected_variables = {int(index): value for capture in expected.get('captures', [])
                              for index, value in capture['variables'].items()}
    for page in args.pages:
        try:
            path = capture(run, project / 'temp/wine-reference', base, page, args.seconds, args.bank, args.clipboard_in_cmd)
        except ReferenceRejected as error:
            evidence['load_error'] = error.text
            (work / 'evidence.json').write_text(json.dumps(evidence, indent=2) + '\n')
            if args.record or (expected.get('expected_load_error') and
                               expected['expected_load_error'] in error.text):
                print('Recorded' if args.record else 'Verified', 'reference rejection', flush=True)
                return
            raise
        if expected.get('expected_load_error'):
            raise RuntimeError('Reference unexpectedly loaded a rejected fixture')
        variables = read_clipboard(path, reference, page)
        evidence['captures'].append({'page': page, 'path': path.name, 'sha256': digest(path),
                                     'variables': variables})
        for index, value in variables.items():
            if not args.record and (index not in expected_variables or expected_variables[index] != value):
                raise RuntimeError('Reference observation mismatch at variable ' + str(index))
        print('Captured', path, flush=True)
    (work / 'evidence.json').write_text(json.dumps(evidence, indent=2) + '\n')


if __name__ == '__main__':
    main()
