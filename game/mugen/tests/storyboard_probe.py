#!/usr/bin/env python3
"""Compare storyboard canvas placement using generated original RGB markers.

Only text sources and measured bounds are tracked. Generated SFF files, native
executables, distribution copies and captures remain below ignored temp/.
Run native captures and app_smoke sequentially on the private display.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time

from PIL import Image
from generate_actor_markers import archive


PROJECT = Path(__file__).resolve().parents[1]
CASES = {
    'default-wide': (None, (1280, 720), 1),
    'default-standard': (None, (640, 480), 1),
    'explicit-standard': ((640, 480), (1280, 720), 1),
    'explicit-wide': ((1280, 720), (1280, 720), 1),
    'wide-standard-window': ((1280, 720), (640, 480), 1),
    'default-stretch': (None, (640, 480), 0),
}
COLORS = [(220, 30, 40), (30, 210, 50)]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def source(local):
    width, height = local or (320, 240)
    info = '' if local is None else f'[Info]\nlocalcoord={width},{height}\n'
    return (info + '[SceneDef]\nspr=probe-story.sff\n[Scene 0]\n'
            'end.time=36000\nclearcolor=255,255,255\nlayerall.pos=0,0\n'
            'layer0.anim=0\nlayer0.offset=80,60\nlayer1.anim=1\n'
            f'layer1.offset={width-16},{height-16}\n'
            '[Begin Action 0]\n0,0,0,0,-1\n'
            '[Begin Action 1]\n0,1,0,0,-1\n')


def measure(path):
    image = Image.open(path).convert('RGB')
    pixels = image.load()
    result = {'dimensions': list(image.size), 'markers': []}
    for color in COLORS:
        points = [(x, y) for y in range(image.height) for x in range(image.width)
                  if pixels[x, y] == color]
        if not points:
            raise RuntimeError('Missing marker: ' + str(color))
        xs, ys = zip(*points)
        result['markers'].append({'rgb': list(color), 'bounds':
                                 [min(xs), min(ys), max(xs)+1, max(ys)+1],
                                 'pixels': len(points)})
    result['edge_samples'] = [list(pixels[x, y]) for x, y in
                              [(0, 0), (image.width//2, 0),
                               (0, image.height//2),
                               (image.width//2, image.height//2)]]
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', choices=CASES, required=True)
    parser.add_argument('--name', required=True)
    parser.add_argument('--record', action='store_true')
    parser.add_argument('--go-run', help='Compare an existing app_smoke run')
    args = parser.parse_args()
    if not re.fullmatch('[a-z0-9_-]+', args.name):
        parser.error('Use a simple lowercase run name')
    if args.go_run and not re.fullmatch('[a-z0-9_-]+', args.go_run):
        parser.error('Use a simple lowercase Go run name')
    if args.record and args.go_run:
        parser.error('Only the native engine may record expectations')
    local, window, keep = CASES[args.fixture]
    text = source(local)
    inputs = {'storyboard_sha256': hashlib.sha256(text.encode()).hexdigest(),
              'sprite_sha256': hashlib.sha256(archive()).hexdigest(),
              'localcoord': list(local or (320, 240)), 'window': list(window),
              'keepaspect': keep}
    golden = PROJECT / 'tests/testdata' / ('storyboard-' + args.fixture + '.json')
    work = PROJECT / 'temp/storyboard-runs' / args.name
    work.mkdir(parents=True, exist_ok=False)
    if args.go_run:
        run = PROJECT / 'temp/app-runs' / args.go_run
        evidence = json.loads((run / 'evidence.json').read_text())
        if evidence.get('error') or not evidence.get('captures') or evidence['window'] != list(window):
            raise RuntimeError('Invalid app capture')
        stock = run / 'stock'
        if digest(stock / 'data/mugen1/probe-story.def') != inputs['storyboard_sha256'] or digest(stock / 'data/mugen1/probe-story.sff') != inputs['sprite_sha256']:
            raise RuntimeError('App storyboard inputs differ')
        if digest(run / 'mugen-go') != evidence['binary_sha256'] or digest(stock / 'data/mugen.cfg') != evidence['config_sha256']:
            raise RuntimeError('App binary/config changed since capture')
        settings = (stock / 'data/mugen.cfg').read_text()
        for key, value in [('GameWidth', 1280), ('GameHeight', 720), ('KeepAspect', keep)]:
            if re.findall(r'(?im)^[ \t]*'+key+r'\s*=\s*(\d+)', settings) != [str(value)]:
                raise RuntimeError('App config differs: '+key)
        captures = []
        for capture in evidence['captures']:
            path = run / capture['path']
            if digest(path) != capture['sha256']:
                raise RuntimeError('App capture hash mismatch')
            captures.append(measure(path))
        expected = json.loads(golden.read_text())
        if digest(stock / 'data/mugen1/system.def') != expected['motif_sha256']:
            raise RuntimeError('App motif differs from native')
        if expected['inputs'] != inputs or any(c != expected['render'] for c in captures):
            raise RuntimeError('Storyboard placement differs: ' + json.dumps(captures))
        (work / 'evidence.json').write_text(json.dumps({'go_run': args.go_run, 'renders': captures}, indent=2)+'\n')
    else:
        reference = PROJECT / 'temp/mugen-1.1b1'
        stock = work / 'stock'
        shutil.copytree(reference, stock)
        directory = stock / 'data/mugen1'
        (directory / 'probe-story.sff').write_bytes(archive())
        (directory / 'probe-story.def').write_text(text)
        motif = directory / 'system.def'
        motif.write_text(re.sub(r'(?im)^intro.storyboard\s*=.*$',
                                'intro.storyboard = probe-story.def', motif.read_text(encoding='utf-8-sig')))
        config = stock / 'data/mugen.cfg'
        settings = config.read_text()
        for key, value in [('motif', 'data/mugen1/system.def'), ('Width', window[0]),
                           ('Height', window[1]), ('KeepAspect', keep)]:
            prefix = r'[ \t]*;?[ \t]*' if key in ('Width', 'Height') else r'[ \t]*'
            settings, count = re.subn(r'(?im)^'+prefix+key+r'\s*=.*$',
                                      lambda m: key+' = '+str(value), settings)
            if count != 1:
                raise RuntimeError('Expected one config key: '+key)
        config.write_text(settings)
        env = os.environ.copy()
        env.update(WINEARCH='win32', WINEPREFIX=str(PROJECT / 'temp/wine-reference'), WINEDEBUG='-all')
        capture = work / 'native.png'
        with (work / 'run.log').open('w') as log:
            process = subprocess.Popen(['wine', 'mugen.exe', '-nojoy'], cwd=stock, env=env, stdout=log, stderr=log)
            try:
                time.sleep(5)
                if process.poll() is not None:
                    raise RuntimeError('Native exited early')
                windows = subprocess.run(['xdotool', 'search', '--onlyvisible', '--pid', str(process.pid)], capture_output=True, text=True).stdout.split()
                if len(windows) != 1:
                    raise RuntimeError('Expected exactly one native window')
                subprocess.run(['import', '-window', windows[0], str(capture)], check=True, timeout=5)
            finally:
                subprocess.run(['wineserver', '-k'], env=env, stdout=log, stderr=log, check=False)
                process.wait(timeout=5)
        evidence = {'inputs': inputs, 'render': measure(capture),
                    'native_sha256': digest(stock / 'mugen.exe'),
                    'config_sha256': digest(config), 'motif_sha256': digest(motif),
                    'capture_sha256': digest(capture)}
        (work / 'evidence.json').write_text(json.dumps(evidence, indent=2)+'\n')
        if args.record:
            golden.write_text(json.dumps(evidence, indent=2)+'\n')
        else:
            expected = json.loads(golden.read_text())
            for key in ['inputs', 'render', 'native_sha256', 'config_sha256', 'motif_sha256']:
                if expected[key] != evidence[key]:
                    raise RuntimeError('Native reproduction differs: '+key)
    print('Verified storyboard ' + args.fixture)


if __name__ == '__main__':
    main()
