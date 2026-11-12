#!/usr/bin/env python3
"""Verify native and Go actor ordering with original solid RGB sprites.

This calibration covers fixed 320-unit opaque marker scenes. It does not
establish general texture filtering, effects, projectiles or framebuffer parity.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys

from PIL import Image


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def measure(image, variables):
    if image.size != (1280, 720):
        raise RuntimeError('Expected the calibrated 1280x720 native window')
    point = (640, 492)
    rgb = image.getpixel(point)
    identities = {(220, 30, 40): int(variables['0']),
                  (30, 210, 50): int(variables['1']),
                  (40, 60, 230): int(variables['4']),
                  (230, 200, 30): int(variables['5'])}
    if rgb not in identities or identities[rgb] <= 0:
        raise RuntimeError('Foreground is not one of the authored entity markers: ' + str(rgb))
    points = [(x, y) for y in range(430, 550) for x in range(580, 700)
              if image.getpixel((x, y)) == rgb]
    bounds = [min(x for x, y in points), min(y for x, y in points),
              max(x for x, y in points) + 1, max(y for x, y in points) + 1]
    return {'window': list(image.size), 'point': list(point), 'rgb': list(rgb),
            'front_entity_id': identities[rgb], 'pure_color_pixels': len(points),
            'pure_color_bounds': bounds}


def check_go(project, stem, expected, args):
    if not expected.get('actor_render'):
        raise RuntimeError('Record native foreground evidence before checking Go')
    run = project / 'temp/app-runs' / args.go_run
    evidence = json.loads((run / 'evidence.json').read_text())
    if evidence.get('error') or not evidence.get('captures'):
        raise RuntimeError('Go app run failed or has no captures')
    for path, key in [(run / 'mugen-go', 'binary_sha256'),
                      (run / 'stock/data/mugen.cfg', 'config_sha256')]:
        if digest(path) != evidence[key]:
            raise RuntimeError('Go app artifact changed: ' + key)
    char = run / 'stock/chars/kfm'
    if digest(char / 'probe.sff') != expected['extra_sha256']['probe.sff']:
        raise RuntimeError('Go original marker bank differs')
    if digest(char / 'probe.air') != expected['air_sha256']:
        raise RuntimeError('Go marker AIR differs')
    if not (char / 'probe.cns').read_text().rstrip().endswith(stem.with_suffix('.cns').read_text().rstrip()):
        raise RuntimeError('Go prepared CNS differs from the authored program')
    commands = (char / 'probe.cmd').read_text()
    states = re.search(r'(?im)^\s*\[statedef', commands)
    if states is None or not commands[states.start():].lstrip().startswith(stem.with_suffix('.cmd').read_text().strip()):
        raise RuntimeError('Go prepared CMD differs from the authored program')
    if digest(run / 'stock/stages/probe-stage.def') != expected['stage_fixture_sha256']:
        raise RuntimeError('Go marker stage differs')
    config = (run / 'stock/data/mugen.cfg').read_text()
    for key, value in [('GameWidth', 1280), ('StageFit', 1), ('KeepAspect', 1)]:
        actual = re.search(r'(?im)^\s*' + key + r'\s*=\s*(\d+)\s*$', config)
        if actual is None or int(actual[1]) != value:
            raise RuntimeError('Go marker render profile differs: ' + key)
    log = (run / 'run.log').read_text().lower()
    if any(text in log for text in ['runtime error', 'panic:', 'missing resource', 'failed to load', 'cannot load']):
        raise RuntimeError('Go runtime/resource error; inspect run.log')
    results = []
    for capture in evidence['captures']:
        path = run / capture['path']
        if digest(path) != capture['sha256']:
            raise RuntimeError('Go captured window hash differs')
        result = measure(Image.open(path).convert('RGB'), expected['captures'][0]['variables'])
        if result != expected['actor_render']:
            raise RuntimeError('Go foreground color/identity/rectangle differs: ' + str(result))
        results.append({'seconds': capture['seconds'], 'render': result})
    output = project / 'temp/actor-render-runs' / args.name
    output.mkdir(parents=True, exist_ok=False)
    (output / 'measurements.json').write_text(json.dumps({'go_run': args.go_run, 'captures': results}, indent=2) + '\n')
    print('Verified Go ' + args.fixture + ': ' + str(len(results)) + ' foreground crops', flush=True)


def main():
    project = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True)
    parser.add_argument('--name', required=True)
    parser.add_argument('--adopt-run')
    parser.add_argument('--go-run')
    parser.add_argument('--record', action='store_true')
    args = parser.parse_args()
    if not re.fullmatch('(actororder|hitdraw)-[a-z0-9-]+', args.fixture):
        parser.error('Use an authored actororder or hitdraw fixture')
    if not all(re.fullmatch('[a-z0-9_-]+', value) for value in
               [args.name] + ([args.adopt_run] if args.adopt_run else []) +
               ([args.go_run] if args.go_run else [])):
        parser.error('Use simple lowercase run names')
    if args.go_run and (args.record or args.adopt_run):
        parser.error('Go verification cannot record or adopt native goldens')
    stem = project / 'tests/testdata/reference' / args.fixture
    expected = json.loads(stem.with_suffix('.json').read_text())
    if expected.get('load_error'):
        parser.error('Use reference_probe.py for the rejected priority fixture')
    for suffix, key in [('.cns', 'cns_sha256'), ('.air', 'air_sha256'),
                        ('.cmd', 'cmd_sha256'), ('.stage.def', 'stage_fixture_sha256')]:
        if digest(stem.with_suffix(suffix)) != expected[key]:
            raise RuntimeError('Authored input changed: ' + suffix)
    if digest(stem.with_suffix('.extra') / 'probe.sff') != expected['extra_sha256']['probe.sff']:
        raise RuntimeError('Original marker bank changed')
    if args.go_run:
        check_go(project, stem, expected, args)
        return
    if not args.adopt_run:
        subprocess.run([sys.executable, str(project / 'tests/reference_probe.py'),
                        '--fixture', args.fixture, '--name', args.name,
                        '--fixture-stage', '--bank', 'fvar', '--mugenversion', '1.1',
                        '--clipboard-in-cmd', '--file', 'sprite=probe.sff',
                        '--pages', '0', '--seconds', '7'], check=True)
    run = project / 'temp/reference-runs' / (args.adopt_run or args.name)
    evidence = json.loads((run / 'evidence.json').read_text())
    for key in ['engine_sha256', 'cns_sha256', 'air_sha256', 'cmd_sha256',
                'stage_fixture_sha256', 'extra_sha256', 'files', 'mugenversion',
                'clipboard_in_cmd', 'fixture_stage', 'bank']:
        if evidence.get(key) != expected.get(key):
            raise RuntimeError('Native recipe differs: ' + key)
    for suffix, key in [('.cns', 'cns_sha256'), ('.air', 'air_sha256'),
                        ('.cmd', 'cmd_sha256'), ('.stage.def', 'stage_fixture_sha256')]:
        if digest(stem.with_suffix(suffix)) != evidence[key]:
            raise RuntimeError('Authored input changed: ' + suffix)
    if digest(stem.with_suffix('.extra') / 'probe.sff') != evidence['extra_sha256']['probe.sff']:
        raise RuntimeError('Original marker bank changed')
    capture = evidence['captures'][0]
    if capture['variables'] != expected['captures'][0]['variables']:
        raise RuntimeError('Native identity/clock observations differ')
    image_path = run / capture['path']
    if digest(image_path) != capture['sha256']:
        raise RuntimeError('Native captured window hash differs')
    result = measure(Image.open(image_path).convert('RGB'), capture['variables'])
    if args.record:
        expected['actor_render'] = result
        stem.with_suffix('.json').write_text(json.dumps(expected, indent=2) + '\n')
    elif result != expected.get('actor_render'):
        raise RuntimeError('Native actor color/order/rectangle differs: ' + str(result))
    output = project / 'temp/actor-render-runs' / args.name
    output.mkdir(parents=True, exist_ok=False)
    (output / 'measurements.json').write_text(json.dumps(result, indent=2) + '\n')
    print(('Recorded ' if args.record else 'Verified ') + args.fixture + ': ' + str(result), flush=True)


if __name__ == '__main__':
    main()
