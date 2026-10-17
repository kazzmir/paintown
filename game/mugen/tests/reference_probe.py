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


def prepare(reference, destination, fixture, bare=False, version=None, files=None, omitted=None, state_order=None, localcoord=None, p2_localcoord=None, game_width=None, fixture_constants=False, game_height=None, video=None, helper_max=None, p2_fixture=None):
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
    if fixture_constants:
        constants = ''
    source = constants + '\n' + fixture.with_suffix('.cns').read_text()
    if p2_localcoord is not None or p2_fixture is not None:
        opponent = destination / 'chars/probeopponent'
        shutil.copytree(char, opponent)
        opponent_definition = definition
        if p2_localcoord is not None:
            opponent_definition, count = re.subn(r'(?im)^\s*localcoord\s*=.*$',
                                                 'localcoord = ' + p2_localcoord, definition)
            if not count:
                opponent_definition = re.sub(r'(?im)^(\s*\[info\]\s*)$',
                                              lambda match: match.group(1) + '\nlocalcoord = ' + p2_localcoord,
                                              opponent_definition)
        (opponent / 'probeopponent.def').write_text(opponent_definition)
        if p2_fixture is not None:
            (opponent / 'probe.cns').write_text(constants + '\n' + p2_fixture.with_suffix('.cns').read_text())
            shutil.copyfile(p2_fixture.with_suffix('.air'), opponent / 'probe.air')
            p2_commands = re.split(r'(?im)^\s*\[statedef', (char / 'kfm.cmd').read_text())[0]
            p2_command_fixture = p2_fixture.with_suffix('.cmd')
            p2_commands += ('\n' + p2_command_fixture.read_text() if p2_command_fixture.exists() else
                            '\n[Statedef -1]\n[State -1, no input]\ntype=Null\ntrigger1=1\n')
            (opponent / 'probe.cmd').write_text(p2_commands)
            p2_extras = p2_fixture.with_suffix('.extra')
            if p2_extras.exists():
                shutil.copytree(p2_extras, opponent, dirs_exist_ok=True)
    config = destination / 'data/mugen.cfg'
    settings = re.sub(r'(?im)^Debug\s*=\s*0\s*$', 'Debug = 1', config.read_text())
    if game_width is not None:
        settings = re.sub(r'(?im)^GameWidth\s*=.*$', 'GameWidth = ' + str(game_width), settings)
    if game_height is not None:
        settings = re.sub(r'(?im)^GameHeight\s*=.*$', 'GameHeight = ' + str(game_height), settings)
    if helper_max is not None:
        settings = re.sub(r'(?im)^HelperMax\s*=.*$','HelperMax = '+str(helper_max),settings)
    for key, value in (video or {}).items():
        settings, count = re.subn(r'(?im)^\s*;?\s*' + re.escape(key) + r'\s*=.*$',
                                  lambda match: key + ' = ' + str(value), settings)
        if count != 1:
            raise RuntimeError('Expected one disposable video setting: ' + key)
    config.write_text(settings)
    return source


def page_source(base, page, bank='var', clipboard_delay=30):
    owner = re.findall(r'(?im)^\s*\[statedef\s+(-?\d+)\]', base)[-1]
    for chunk in range(2):
        start = page + chunk * 6
        base += ('\n[State ' + owner + ', dump]\ntype=' +
                 ('DisplayToClipboard' if chunk == 0 else 'AppendToClipboard') +
                 '\ntrigger1=time>='+str(clipboard_delay)+'\ntext="P' + str(start) + ':' +
                 ','.join(['%.9g' if bank == 'fvar' else '%d'] * 6) + '\\n"\nparams=' +
                 ','.join(bank + '(' + str(i) + ')' for i in range(start, start + 6)) + '\n')
    return base


def capture(run, prefix, source, page, seconds, bank='var', clipboard_in_cmd=False, stage='kfm', clipboard_delay=30, independent_opponent=False):
    if clipboard_in_cmd:
        commands = run / 'chars/kfm/probe.cmd'
        base = re.split(r'(?im)\n\[State -?\d+, dump\]', commands.read_text())[0]
        commands.write_text(page_source(base, page, bank, clipboard_delay))
        (run / 'chars/kfm/probe.cns').write_text(source)
    else:
        (run / 'chars/kfm/probe.cns').write_text(page_source(source, page, bank, clipboard_delay))
    opponent = run / 'chars/probeopponent'
    if opponent.exists() and not independent_opponent:
        (opponent / 'probe.cns').write_text(source)
    env = os.environ.copy()
    env.update(WINEARCH='win32', WINEPREFIX=str(prefix), WINEDEBUG='-all')
    output = run.parent / ('animation-page' + str(page) + '.png')
    with (run.parent / 'animation-probe.log').open('w') as log:
        process = subprocess.Popen(['wine', 'mugen.exe', 'kfm', 'probeopponent' if opponent.exists() else 'kfm', '-s', stage, '-nojoy'],
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
        if len(fields) < 3 or fields[0] not in 'PEINF#0123456789:,.+-':
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
                number = r'(?:[+-]?\d+(?:\.\d+)?(?:E[+-]?\d+)?|[+-]?1\.#INF)'
                if re.fullmatch('P' + str(first) + ':' + number + '(' + ',' + number + '){5}', text):
                    if first in lines:
                        raise RuntimeError('Ambiguous duplicate clipboard line')
                    lines[first] = [('negative_infinity' if n.startswith('-') else 'positive_infinity') if '#INF' in n
                                    else float(n) if any(c in n for c in '.E') else int(n)
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
    parser.add_argument('--fixture-constants', action='store_true', help='Use complete constant groups authored in the CNS fixture')
    parser.add_argument('--bank', choices=['var', 'fvar'], default='var')
    parser.add_argument('--mugenversion', choices=['1.0', '1.1'])
    parser.add_argument('--file', action='append', default=[], help='Override one character Files entry: key=value')
    parser.add_argument('--clipboard-in-cmd', action='store_true')
    parser.add_argument('--omit-file', action='append', default=[])
    parser.add_argument('--case', help='Use a pinned named case from the fixture JSON')
    parser.add_argument('--state-file-order', nargs='+')
    parser.add_argument('--localcoord', help='Override character localcoord: width,height')
    parser.add_argument('--p2-localcoord', help='Use a separate probe opponent with this localcoord')
    parser.add_argument('--p2-fixture', help='Use independently authored opponent CNS/AIR/CMD files')
    parser.add_argument('--game-width', type=int, help='Override common game-space width for coordinate probes')
    parser.add_argument('--game-height', type=int, help='Override configured game-space height for coordinate probes')
    parser.add_argument('--video', action='append', default=[], help='Disposable video override: StageFit/SystemFit/KeepAspect=0|1 or Width/Height=N')
    parser.add_argument('--hud-marker', action='store_true', help='Use the fixture-owned probe.sff for calibrated Face background markers in disposable fight files')
    parser.add_argument('--helper-max', type=int, help='Override only the copied HelperMax for entity identity/allocation probes')
    parser.add_argument('--stage', default='kfm', help='Stage name/path in the disposable distribution')
    parser.add_argument('--stage-version',choices=['1.0','1.1'],help='Override only the copied stage version for compatibility probes')
    parser.add_argument('--pages', type=int, nargs='+', default=[0, 12, 30, 42])
    parser.add_argument('--clipboard-delay',type=int,default=30,help='Minimum local Time for clipboard output; zero supports continuously frozen subjects')
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
        args.clipboard_delay = case_expected.get('clipboard_delay',30)
        args.bank = case_expected.get('bank', 'var')
        args.mugenversion = case_expected.get('mugenversion')
        args.bare_constants = case_expected.get('bare_constants', False)
        args.fixture_constants = case_expected.get('fixture_constants', False)
        args.pages = [capture['page'] for capture in case_expected.get('captures', [])] or [0]
        args.state_file_order = case_expected.get('state_file_order')
        args.localcoord = case_expected.get('localcoord')
        args.p2_localcoord = case_expected.get('p2_localcoord')
        args.p2_fixture = case_expected.get('p2_fixture')
        args.game_width = case_expected.get('game_width')
        args.game_height = case_expected.get('game_height')
        args.stage = case_expected.get('stage','kfm')
        args.stage_version = case_expected.get('stage_version')
        args.video = [key+'='+str(value) for key,value in case_expected.get('video',{}).items()]
        args.hud_marker = case_expected.get('hud_marker',False)
        args.helper_max = case_expected.get('helper_max')
    if not all(re.fullmatch('[a-z0-9_-]+', value) for value in [args.name, args.fixture]) or not 3 <= args.seconds <= 30:
        parser.error('Use a simple run name and a capture delay from 3 to 30 seconds')
    if args.p2_fixture is not None and not re.fullmatch('[a-z0-9_-]+', args.p2_fixture):
        parser.error('Use a simple opponent fixture name')
    if any(page < 0 or page > (28 if args.bank == 'fvar' else 48) for page in args.pages):
        parser.error('Page exceeds the variable bank')
    if not 0<=args.clipboard_delay<=600:
        parser.error('Clipboard delay must be 0..600 local ticks')
    if args.game_width is not None and not 1 <= args.game_width <= 8192:
        parser.error('Game width must be 1 through 8192 for bounded reference probes')
    if args.game_height is not None and not 1 <= args.game_height <= 8192:
        parser.error('Game height must be 1 through 8192 for bounded reference probes')
    if args.helper_max is not None and not 0 <= args.helper_max <= 256:
        parser.error('Helper allocation probe limit must be 0..256')
    if not re.fullmatch('[a-zA-Z0-9_./-]+',args.stage) or args.stage.startswith('/') or '..' in Path(args.stage).parts:
        parser.error('Stage must be a relative name/path in the disposable distribution')
    for profile in (args.localcoord, args.p2_localcoord):
        if profile is not None and (not re.fullmatch(r'[1-9][0-9]*(?:\.[0-9]+)?,[1-9][0-9]*(?:\.[0-9]+)?', profile) or
                                    any(float(value) > 2147483647 for value in profile.split(','))):
            parser.error('Localcoord requires positive numeric width,height')
    reference = args.reference.resolve()
    video = {}
    for item in args.video:
        key, separator, value = item.partition('=')
        if not separator or key not in ['StageFit','SystemFit','KeepAspect','Width','Height'] or not value.isdigit():
            parser.error('Unsupported video override')
        value = int(value)
        if key in video or (value not in [0,1] if key in ['StageFit','SystemFit','KeepAspect'] else not 64<=value<=4096):
            parser.error('Duplicate or out-of-range video override')
        video[key] = value
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
    p2_fixture = project / 'tests/testdata/reference' / args.p2_fixture if args.p2_fixture else None
    if args.state_file_order and (len(set(args.state_file_order)) != len(args.state_file_order) or
                                 any(not re.fullmatch('st[0-9]*', key) for key in args.state_file_order)):
        parser.error('State-file order must contain unique st/stN keys')
    if args.bare_constants and args.fixture_constants:
        parser.error('Bare and fixture-owned constants are mutually exclusive')
    base = prepare(reference, run, fixture, args.bare_constants, args.mugenversion, files, args.omit_file, args.state_file_order, args.localcoord, args.p2_localcoord, args.game_width, args.fixture_constants,args.game_height,video,args.helper_max,p2_fixture)
    hud_hashes = {}
    if args.hud_marker:
        marker = fixture.with_suffix('.extra')/'probe.sff'
        for relative in ['data/fight.def','data/mugen1/fight.def']:
            fight = run/relative
            shutil.copyfile(marker,fight.parent/'probe-hud.sff')
            source = fight.read_text(encoding='utf-8-sig')
            source = re.sub(r'(?im)^sff\s*=.*$','sff = probe-hud.sff',source)
            sections = re.split(r'(?im)(^\s*\[[^\r\n]+\]\s*$)',source)
            for index in range(1,len(sections),2):
                if sections[index].strip().lower()!='[face]':continue
                sections[index+1]='\np1.pos=128,64\np1.bg0.spr=0,4\np1.bg0.scale=4,4\np1.bg1.anim=-1\np1.face.spr=-1,0\np2.pos=1152,64\np2.bg0.spr=0,4\np2.bg0.scale=4,4\np2.bg1.anim=-1\np2.face.spr=-1,0\n'
            fight.write_text(''.join(sections))
            hud_hashes[relative]=digest(fight)
    stage_hash=None
    if args.stage_version:
        stage_path=Path(args.stage)
        if stage_path.suffix.lower()!='.def':stage_path=stage_path.with_suffix('.def')
        if stage_path.parts[0]!='stages':stage_path=Path('stages')/stage_path
        stage_path=run/stage_path
        stage_source=stage_path.read_text(encoding='utf-8-sig')
        stage_source,count=re.subn(r'(?im)^\s*mugenversion\s*=.*$','mugenversion = '+args.stage_version,stage_source)
        if not count:raise RuntimeError('Version probe stage lacks a declared Info mugenversion')
        stage_path.write_text(stage_source)
        stage_hash=digest(stage_path)
    evidence = {'engine_sha256': digest(reference / 'mugen.exe'),
                'cns_sha256': digest(fixture.with_suffix('.cns')),
                'air_sha256': digest(fixture.with_suffix('.air')),
                'invocation': 'wine mugen.exe kfm ' + ('probeopponent' if args.p2_localcoord or args.p2_fixture else 'kfm') + ' -s '+args.stage+' -nojoy', 'captures': [],
                'bank': args.bank, 'bare_constants': args.bare_constants}
    if files:
        evidence['files'] = files
    if args.fixture_constants:
        evidence['fixture_constants'] = True
    if args.clipboard_in_cmd:
        evidence['clipboard_in_cmd'] = True
    if args.clipboard_delay!=30:
        evidence['clipboard_delay']=args.clipboard_delay
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
    if p2_fixture is not None:
        evidence['p2_fixture'] = args.p2_fixture
        evidence['p2_sha256'] = {suffix[1:]: digest(p2_fixture.with_suffix(suffix)) for suffix in ['.cns','.air','.cmd'] if p2_fixture.with_suffix(suffix).exists()}
        p2_extras = p2_fixture.with_suffix('.extra')
        if p2_extras.exists():
            evidence['p2_extra_sha256'] = {str(path.relative_to(p2_extras)): digest(path) for path in sorted(p2_extras.rglob('*')) if path.is_file()}
    if args.game_width is not None:
        evidence['game_width'] = args.game_width
    if args.game_height is not None:
        evidence['game_height'] = args.game_height
    if video:
        evidence['video'], evidence['config_sha256'] = video, digest(run/'data/mugen.cfg')
    if args.helper_max is not None:
        evidence['helper_max'],evidence['config_sha256']=args.helper_max,digest(run/'data/mugen.cfg')
    if args.hud_marker:
        evidence['hud_marker'],evidence['hud_fight_sha256']=True,hud_hashes
    if args.stage!='kfm':
        evidence['stage']=args.stage
    if args.stage_version:
        evidence['stage_version'],evidence['stage_sha256']=args.stage_version,stage_hash
    if fixture.with_suffix('.cmd').exists():
        evidence['cmd_sha256'] = digest(fixture.with_suffix('.cmd'))
    expected_variables = {}
    expected = {}
    if not args.record:
        expected = case_expected or json.loads(fixture.with_suffix('.json').read_text())
        if expected.get('bank', 'var') != args.bank or expected.get('bare_constants', False) != args.bare_constants:
            raise RuntimeError('Variable bank/constants mode differs from pinned fixture')
        if expected.get('fixture_constants', False) != args.fixture_constants:
            raise RuntimeError('Fixture constant ownership differs from pinned fixture')
        if expected.get('mugenversion') != args.mugenversion:
            raise RuntimeError('Character version differs from pinned fixture')
        if expected.get('localcoord') != args.localcoord:
            raise RuntimeError('Character localcoord differs from pinned fixture')
        if expected.get('p2_localcoord') != args.p2_localcoord:
            raise RuntimeError('Opponent localcoord differs from pinned fixture')
        if expected.get('p2_fixture') != args.p2_fixture:
            raise RuntimeError('Opponent fixture differs from pinned fixture')
        if expected.get('game_width') != args.game_width:
            raise RuntimeError('Common game-space width differs from pinned fixture')
        if expected.get('game_height') != args.game_height or expected.get('stage','kfm')!=args.stage:
            raise RuntimeError('Configured height/stage differs from pinned fixture')
        if expected.get('stage_version')!=args.stage_version:
            raise RuntimeError('Stage version differs from pinned fixture')
        if expected.get('video',{})!=video:
            raise RuntimeError('Video overrides differ from pinned fixture')
        if expected.get('hud_marker',False)!=args.hud_marker:
            raise RuntimeError('HUD marker mode differs from pinned fixture')
        if expected.get('helper_max')!=args.helper_max:
            raise RuntimeError('Helper allocation limit differs from pinned fixture')
        if expected.get('files', {}) != files or expected.get('clipboard_in_cmd', False) != args.clipboard_in_cmd:
            raise RuntimeError('Files/clipboard mode differs from pinned fixture')
        if expected.get('clipboard_delay',30)!=args.clipboard_delay:
            raise RuntimeError('Clipboard delay differs from pinned fixture')
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
            path = capture(run, project / 'temp/wine-reference', base, page, args.seconds, args.bank, args.clipboard_in_cmd,args.stage,args.clipboard_delay,args.p2_fixture is not None)
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
