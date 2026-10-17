#!/usr/bin/env python3
"""Replay native actor ordering with original solid RGB sprites.

This calibration covers these fixed 320-wide opaque marker scenes. It does not
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


def main():
    project = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True)
    parser.add_argument('--name', required=True)
    parser.add_argument('--adopt-run')
    parser.add_argument('--record', action='store_true')
    args = parser.parse_args()
    if not re.fullmatch('actororder-[a-z0-9-]+', args.fixture):
        parser.error('Use an authored actororder fixture')
    if not all(re.fullmatch('[a-z0-9_-]+', value) for value in
               [args.name] + ([args.adopt_run] if args.adopt_run else [])):
        parser.error('Use simple lowercase run names')
    stem = project / 'tests/testdata/reference' / args.fixture
    expected = json.loads(stem.with_suffix('.json').read_text())
    if expected.get('load_error'):
        parser.error('Use reference_probe.py for the rejected priority fixture')
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
