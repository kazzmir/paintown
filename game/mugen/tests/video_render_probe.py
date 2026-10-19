#!/usr/bin/env python3
"""Replay calibrated fight viewport/HUD marker measurements in owned windows.

Only these static KFM fixtures and the original RGB ramp are calibrated. Solid
cores establish projection, not full-frame filtering or background fidelity.
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


def cores(image, region):
    mask = {(x, y) for y in range(region[1], region[3]) for x in range(region[0], region[2])
            if (lambda c: c[2] == 75 and c[0] <= 217 and c[1] <= 201)(image.getpixel((x, y)))}
    found = []
    while mask:
        first = min(mask)
        mask.remove(first)
        queue, component = deque([first]), []
        while queue:
            x, y = queue.popleft()
            component.append((x, y))
            for point in [(x-1, y), (x+1, y), (x, y-1), (x, y+1)]:
                if point in mask:
                    mask.remove(point)
                    queue.append(point)
        left, top = min(x for x, y in component), min(y for x, y in component)
        right, bottom = max(x for x, y in component)+1, max(y for x, y in component)+1
        if len(component) >= 80 and len(component) == (right-left)*(bottom-top) and 1.5 <= (right-left)/(bottom-top) <= 3.5:
            found.append([left, top, right, bottom])
    return found


def measure(path):
    image = Image.open(path).convert('RGB')
    w, h = image.size
    hud = cores(image, (w*2//3, 0, w, h//4))
    world = cores(image, (w//5, h//4, w//2, h*9//10))
    # The fighter uses the same original ramp at floor level. The super marker
    # is the upper component, fifty local units above it in this fixed scene.
    if len(hud) != 1 or not 1 <= len(world) <= 2:
        raise RuntimeError('Expected one clean HUD/world marker: '+str((hud, world)))
    world = min(world, key=lambda rectangle: rectangle[1])
    return {'window': [w, h], 'hud_core': hud[0], 'world_core': world,
            'hud_core_sha256': hashlib.sha256(image.crop(tuple(hud[0])).tobytes()).hexdigest(),
            'world_core_sha256': hashlib.sha256(image.crop(tuple(world)).tobytes()).hexdigest()}


def main():
    project = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--case', required=True)
    parser.add_argument('--name', required=True)
    parser.add_argument('--adopt-run')
    parser.add_argument('--record', action='store_true')
    args = parser.parse_args()
    if not all(re.fullmatch('[a-z0-9_-]+', value) for value in [args.case, args.name]+([args.adopt_run] if args.adopt_run else [])):
        parser.error('Use simple lowercase names')
    stem = project/'tests/testdata/reference/video-fit'
    catalogue = json.loads(stem.with_suffix('.json').read_text())
    expected = catalogue['cases'][args.case]
    if not expected['hud_marker']:
        raise RuntimeError('This calibration requires the authored HUD marker')
    if args.adopt_run:
        run = project/'temp/reference-runs'/args.adopt_run
    else:
        subprocess.run([sys.executable, str(project/'tests/reference_probe.py'), '--fixture', 'video-fit',
                        '--case', args.case, '--name', args.name], check=True)
        run = project/'temp/reference-runs'/args.name
    evidence = json.loads((run/'evidence.json').read_text())
    for key, value in expected.items():
        if key.endswith('_sha256') or key in ['video', 'hud_marker', 'game_width', 'game_height', 'mugenversion', 'files']:
            if evidence.get(key) != value:
                raise RuntimeError('Native recipe differs: '+key)
    for suffix, key in [('.cns', 'cns_sha256'), ('.air', 'air_sha256')]:
        if digest(stem.with_suffix(suffix)) != evidence[key]:
            raise RuntimeError('Source changed since capture')
    if digest(stem.with_suffix('.extra')/'probe.sff') != evidence['extra_sha256']['probe.sff']:
        raise RuntimeError('Original marker changed')
    result = measure(run/'animation-page0.png')
    work = project/'temp/video-render-runs'/args.name
    work.mkdir(parents=True, exist_ok=False)
    (work/'measurements.json').write_text(json.dumps(result, indent=2)+'\n')
    if args.record:
        expected['video_render'] = result
        stem.with_suffix('.json').write_text(json.dumps(catalogue, indent=2)+'\n')
    elif result != expected['video_render']:
        raise RuntimeError('Native marker projection/core differs')
    print(('Recorded' if args.record else 'Verified')+' video projection '+args.case, flush=True)


if __name__ == '__main__':
    main()
