#!/usr/bin/env python3
"""Verify frozen actor transparency/geometry RGB crops without changing goldens.

Run reference_probe.py and app_smoke.py serially first. Native images/resources
remain in ignored temp; the committed fixture pins the exact decoded crop hash.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re

from PIL import Image, ImageChops


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True)
    parser.add_argument('--go-run', required=True)
    parser.add_argument('--native-run')
    parser.add_argument('--name', required=True)
    parser.add_argument('--original-region', action='store_true', help='Check the preserved wider diagnostic crop')
    args = parser.parse_args()
    if not re.fullmatch(r'(?:actortrans|actordraw|palettefx)-[a-z0-9-]+|render-world-overlay-empty', args.fixture):
        parser.error('Use an authored actortrans, actordraw or palettefx fixture')
    native_name = args.native_run or ('m4cp-' if args.fixture.startswith('actortrans-') else 'm4cr-' if args.fixture.startswith('palettefx-') else 'm4cq-') + args.fixture
    if not all(re.fullmatch(r'[a-z0-9_-]+', x) for x in [args.go_run, native_name, args.name]):
        parser.error('Use simple lowercase run names')
    stem = root / 'tests/testdata/reference' / args.fixture
    expected = json.loads(stem.with_suffix('.json').read_text())
    spec = expected.get('trans_render') or expected.get('drawing_render') or expected.get('palette_render')
    if expected.get('load_error') or not spec:
        parser.error('Use an accepted fixture with frozen drawing evidence')
    crop_source = stem.with_suffix('.json').name
    if args.fixture.startswith('actordraw-') and not args.original_region:
        manifest_path = root / 'tests/testdata/reference/actor-drawing-crops.json'
        manifest = json.loads(manifest_path.read_text())
        row = manifest['fixtures'][args.fixture]
        if digest(stem.with_suffix('.json')) != row['source_json_sha256']:
            raise RuntimeError('Original diagnostic native golden changed')
        calibration = root / 'tests/testdata/reference' / (manifest['calibration_fixture'] + '.json')
        if digest(calibration) != manifest['calibration_json_sha256']:
            raise RuntimeError('Empty-world crop calibration changed')
        spec = dict(region=manifest['region'], native_crop_sha256=row['native_crop_sha256'])
        crop_source = manifest_path.name
    native = root / 'temp/reference-runs' / native_name
    run = root / 'temp/app-runs' / args.go_run
    nd = json.loads((native / 'evidence.json').read_text())
    gd = json.loads((run / 'evidence.json').read_text())
    for ext, key in [('.cns', 'cns_sha256'), ('.air', 'air_sha256'),
                     ('.cmd', 'cmd_sha256'), ('.stage.def', 'stage_fixture_sha256')]:
        if digest(stem.with_suffix(ext)) != expected[key] or nd.get(key) != expected[key]:
            raise RuntimeError('Frozen native input differs: ' + key)
    for key in ['engine_sha256', 'localcoord', 'p2_localcoord', 'fixture_constants', 'mugenversion']:
        if nd.get(key) != expected.get(key):
            raise RuntimeError('Native recipe differs: ' + key)
    players = ['chars/kfm/kfm.def', 'chars/probeopponent/probeopponent.def' if expected.get('p2_localcoord') else 'chars/kfm/kfm.def']
    if gd.get('error') or gd.get('players') != players:
        raise RuntimeError('Go application failed or selected the wrong definitions')
    for path in players + ['chars/kfm/probe.cns', 'chars/kfm/probe.cmd', 'chars/kfm/probe.air', 'chars/kfm/kfm.sff', 'stages/probe-stage.def']:
        if digest(run / 'stock' / path) != digest(native / 'stock' / path):
            raise RuntimeError('Prepared Go/native resource differs: ' + path)
    if digest(run / 'mugen-go') != gd['binary_sha256'] or digest(run / 'stock/data/mugen.cfg') != gd['config_sha256']:
        raise RuntimeError('Go binary/configuration changed after capture')
    config = (run / 'stock/data/mugen.cfg').read_text()
    for key, value in [('GameWidth', 1280), ('StageFit', 1), ('KeepAspect', 1)]:
        match = re.search(r'(?im)^\s*' + key + r'\s*=\s*(\d+)\s*$', config)
        if match is None or int(match[1]) != value:
            raise RuntimeError('Go render profile differs: ' + key)
    log = (run / 'run.log').read_text().lower()
    if any(x in log for x in ['runtime error', 'panic:', 'missing resource', 'failed to load', 'cannot load']):
        raise RuntimeError('Go runtime/resource error; inspect run.log')
    if len(nd.get('captures', [])) != 1 or not gd.get('captures'):
        raise RuntimeError('Incomplete native/Go capture evidence')
    nc = nd['captures'][0]
    if digest(native / nc['path']) != nc['sha256']:
        raise RuntimeError('Native captured image changed')
    ni = Image.open(native / nc['path']).convert('RGB')
    region = spec['region']
    native_crop = ni.crop(region)
    if ni.size != (1280, 720) or hashlib.sha256(native_crop.tobytes()).hexdigest() != spec['native_crop_sha256']:
        raise RuntimeError('Native crop differs from the frozen text golden')
    results = []
    for capture in gd['captures']:
        path = run / capture['path']
        if digest(path) != capture['sha256']:
            raise RuntimeError('Go captured image changed')
        im = Image.open(path).convert('RGB')
        if im.size != ni.size:
            raise RuntimeError('Go window dimensions differ')
        crop = im.crop(region)
        exact = hashlib.sha256(crop.tobytes()).hexdigest() == spec['native_crop_sha256']
        difference = ImageChops.difference(crop, native_crop)
        pixels = 0 if exact else sum(rgb != (0, 0, 0) for rgb in difference.getdata())
        errors = [s for s in spec.get('native_rgb_samples', []) if list(im.getpixel(tuple(s['point']))) != s['rgb']]
        results.append({'seconds': capture['seconds'], 'exact_rgb_crop': exact,
                        'rgb_differences': pixels, 'sample_errors': errors,
                        'crop_pixels': crop.width * crop.height})
    out = root / 'temp/drawing-render-runs' / args.name
    out.mkdir(parents=True, exist_ok=False)
    (out / 'measurements.json').write_text(json.dumps({'fixture': args.fixture,
        'native_run': native_name, 'go_run': args.go_run, 'crop_source': crop_source, 'binary_sha256': gd['binary_sha256'],
        'captures': results}, indent=2) + '\n')
    print(json.dumps(results), flush=True)
    if any(not r['exact_rgb_crop'] or r['sample_errors'] for r in results):
        raise SystemExit('Strict native drawing crop comparison failed; goldens unchanged')


if __name__ == '__main__':
    main()
