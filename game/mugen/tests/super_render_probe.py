#!/usr/bin/env python3
"""Measure static SuperPause colors and the original RGB effect in owned captures.

The calibration covers these fixed-camera 320-wide KFM marker fixtures only.
It does not claim full framebuffer or sprite-filter parity.
"""
import argparse
from collections import deque
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys

from PIL import Image


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def measurements(path, owner_moved=False, expect_none=False):
    image = Image.open(path).convert('RGB')
    if image.size != (1280, 720):
        raise RuntimeError('This calibration requires the pinned 1280x720 window')
    points = {'stage': (640, 300), 'stage_floor': (500, 500), 'hud': (300, 23),
              'owner': (500 if owner_moved else 440, 605), 'opponent': (840, 605)}
    region = (480, 440, 720, 480)
    mask = {(x, y) for y in range(region[1], region[3]) for x in range(region[0], region[2])
            if image.getpixel((x, y))[2] == 75 and image.getpixel((x, y))[0] <= 217 and image.getpixel((x, y))[1] <= 201}
    cores = []
    while mask:
        first = min(mask)
        mask.remove(first)
        queue, component = deque([first]), []
        while queue:
            x, y = queue.popleft()
            component.append((x, y))
            for adjacent in [(x-1, y), (x+1, y), (x, y-1), (x, y+1)]:
                if adjacent in mask:
                    mask.remove(adjacent)
                    queue.append(adjacent)
        left, top = min(x for x,y in component), min(y for x,y in component)
        right, bottom = max(x for x,y in component)+1, max(y for x,y in component)+1
        if len(component) >= 80 and len(component) == (right-left)*(bottom-top):
            cores.append([left, top, right, bottom])
    if expect_none:
        if cores or image.getpixel(points['owner'])[2] != 75 or sum(image.getpixel(points['stage'])) < 100:
            raise RuntimeError('Expected a retired effect in a complete visible scene')
        return {'window': list(image.size), 'visible': False}
    if len(cores) != 1:
        raise RuntimeError('Expected one solid original effect core: ' + str(cores))
    points['super_effect'] = (cores[0][0]+5, cores[0][1]+5)
    return {'window': list(image.size), 'visible': True, 'samples': {key: {'point': list(point), 'rgb': list(image.getpixel(point))} for key,point in points.items()},
            'effect_core': cores[0], 'effect_core_sha256': hashlib.sha256(image.crop(tuple(cores[0])).tobytes()).hexdigest()}


def main():
    project = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True, choices=['superpause-dark-0', 'superpause-dark-1', 'superpause-effect-clock', 'superpause-effect-position', 'superpause-effect-expire', 'superpause-effect-replace', 'superpause-effect-finite', 'superpause-effect-ordinary-pause', 'superpause-effect-other-pause', 'superpause-effect-super-replaced'])
    parser.add_argument('--name', required=True)
    parser.add_argument('--adopt-run')
    parser.add_argument('--record', action='store_true')
    args = parser.parse_args()
    if not all(re.fullmatch('[a-z0-9_-]+', value) for value in [args.name]+([args.adopt_run] if args.adopt_run else [])):
        parser.error('Use simple run names')
    stem = project/'tests/testdata/reference'/args.fixture
    expected = json.loads(stem.with_suffix('.json').read_text())
    if args.adopt_run:
        run = project/'temp/reference-runs'/args.adopt_run
    else:
        subprocess.run([sys.executable, str(project/'tests/reference_probe.py'), '--fixture', args.fixture, '--name', args.name,
                        '--mugenversion', '1.1', '--clipboard-in-cmd', '--clipboard-delay', str(expected.get('clipboard_delay',30)),
                        '--bank', 'fvar', '--file', 'sprite=probe.sff', '--pages', '0'], check=True)
        run = project/'temp/reference-runs'/args.name
    evidence = json.loads((run/'evidence.json').read_text())
    for key in ['engine_sha256', 'cns_sha256', 'air_sha256', 'extra_sha256']:
        if evidence[key] != expected[key]:
            raise RuntimeError('Native capture recipe mismatch: '+key)
    if evidence.get('clipboard_delay',30)!=expected.get('clipboard_delay',30):
        raise RuntimeError('Native clipboard timing differs')
    for suffix,key in [('.cns','cns_sha256'),('.air','air_sha256')]:
        if digest(stem.with_suffix(suffix)) != evidence[key]:
            raise RuntimeError('Source changed since capture')
    if digest(stem.with_suffix('.extra')/'probe.sff') != evidence['extra_sha256']['probe.sff']:
        raise RuntimeError('Original marker bank changed')
    result = measurements(run/'animation-page0.png', args.fixture == 'superpause-effect-position', args.fixture == 'superpause-effect-finite')
    work = project/'temp/super-render-runs'/args.name
    work.mkdir(parents=True, exist_ok=False)
    (work/'measurements.json').write_text(json.dumps(result, indent=2)+'\n')
    if args.record:
        expected['super_render'] = result
        stem.with_suffix('.json').write_text(json.dumps(expected, indent=2)+'\n')
    elif result != expected['super_render']:
        raise RuntimeError('Native static samples/effect core differ')
    print(('Recorded' if args.record else 'Verified')+' SuperPause rendering '+args.fixture, flush=True)


if __name__ == '__main__':
    main()
