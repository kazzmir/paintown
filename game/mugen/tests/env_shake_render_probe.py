#!/usr/bin/env python3
"""Strict read-only comparison of environment shake/viewport behavior with frozen beta captures."""
import argparse
import hashlib
import json
from pathlib import Path
import re

import numpy as np
from PIL import Image


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def opened(run, capture):
    path = run / capture['path']
    if digest(path) != capture['sha256']:
        raise RuntimeError('Capture changed: ' + str(path))
    image = Image.open(path).convert('RGB')
    if image.size != (1280, 720):
        raise RuntimeError('Expected calibrated 1280x720 capture')
    return image


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True)
    parser.add_argument('--go-run', required=True)
    parser.add_argument('--name', required=True)
    args = parser.parse_args()
    if not re.fullmatch('envshake-[a-z0-9-]+', args.fixture):
        parser.error('Use an authored envshake fixture')
    if any(not re.fullmatch('[a-z0-9_-]+', value) for value in [args.go_run, args.name]):
        parser.error('Use simple lowercase run names')
    root = Path(__file__).resolve().parents[1]
    bank = root / 'tests/testdata/reference'
    expected = json.loads((bank / (args.fixture + '.json')).read_text())
    spec = expected.get('env_shake_render')
    if spec is None:
        parser.error('Rejected or dynamic-phase controls have no fixed-image expectation')
    for ext, key in [('.cns', 'cns_sha256'), ('.air', 'air_sha256'),
                     ('.cmd', 'cmd_sha256'), ('.stage.def', 'stage_fixture_sha256')]:
        if digest(bank / (args.fixture + ext)) != expected[key]:
            raise RuntimeError('Authored source changed: ' + ext)
    if digest(bank / spec['calibration_json']) != spec['calibration_sha256']:
        raise RuntimeError('Native point calibration changed')
    native = root / 'temp/reference-runs' / ('m4cv-' + args.fixture)
    nd = json.loads((native / 'evidence.json').read_text())
    if nd['captures'] != expected['captures'] or nd.get('load_error'):
        raise RuntimeError('Native numeric/capture evidence differs')
    if digest(native / 'stock/mugen.exe') != expected['engine_sha256']:
        raise RuntimeError('Reference executable changed')
    ni = opened(native, nd['captures'][0])
    for sample in spec['native_rgb_samples']:
        if list(ni.getpixel(tuple(sample['point']))) != sample['rgb']:
            raise RuntimeError('Native sample differs from frozen capture')
    reference = ni.crop(spec['region'])
    if hashlib.sha256(reference.tobytes()).hexdigest() != spec['native_crop_sha256']:
        raise RuntimeError('Native crop changed')
    run = root / 'temp/app-runs' / args.go_run
    evidence = json.loads((run / 'evidence.json').read_text())
    if evidence.get('error') or not evidence.get('captures'):
        raise RuntimeError('Application capture failed')
    if evidence['players'] != ['chars/kfm/kfm.def'] * 2 or evidence['stage'] != 'stages/probe-stage.def':
        raise RuntimeError('Application selected another fixture')
    if digest(run / 'mugen-go') != evidence['binary_sha256'] or digest(run / 'stock/data/mugen.cfg') != evidence['config_sha256']:
        raise RuntimeError('Application executable/configuration changed')
    asset_suffixes = {'.sff', '.snd', '.air', '.def', '.fnt', '.ttf', '.otf', '.cns', '.cmd'}
    for source in (native / 'stock').rglob('*'):
        if source.is_file() and source.suffix.lower() in asset_suffixes:
            relative = source.relative_to(native / 'stock')
            if digest(source) != digest(run / 'stock' / relative):
                raise RuntimeError('Prepared asset differs: ' + str(relative))
    log = (run / 'run.log').read_text().lower()
    if any(text in log for text in ['panic:', 'runtime error', 'failed to load', 'cannot load']):
        raise RuntimeError('Application runtime/resource error')
    results = []
    for capture in evidence['captures']:
        image = opened(run, capture)
        samples = [{'point': sample['point'], 'native': sample['rgb'],
                    'go': list(image.getpixel(tuple(sample['point'])))}
                   for sample in spec['native_rgb_samples']
                   if list(image.getpixel(tuple(sample['point']))) != sample['rgb']]
        actual = image.crop(spec['region'])
        a, b = np.array(actual), np.array(reference)
        results.append({'rgb_differences': int(np.count_nonzero(np.any(a != b, axis=2))),
                        'crop_pixels': a.shape[0] * a.shape[1],
                        'sample_count': len(spec['native_rgb_samples']), 'sample_errors': samples})
    output = root / 'temp/env-shake-runs' / args.name
    output.mkdir(parents=True, exist_ok=False)
    (output / 'measurements.json').write_text(json.dumps({
        'fixture': args.fixture, 'go_run': args.go_run,
        'binary_sha256': evidence['binary_sha256'], 'captures': results,
    }, indent=2) + '\n')
    print(json.dumps(results), flush=True)
    if any(row['rgb_differences'] or row['sample_errors'] for row in results):
        raise SystemExit('Strict native EnvShake comparison failed; goldens unchanged')


if __name__ == '__main__':
    main()
