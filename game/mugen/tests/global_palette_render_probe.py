#!/usr/bin/env python3
"""Read-only comparison of a global palette application with frozen native data.

Checks stage and actor RGB crops and native stage/HUD/actor/effect/clear samples.
It cannot create or alter native expectations. Runtime captures remain private.
"""
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
    if not re.fullmatch('globalfx-[a-z0-9-]+', args.fixture):
        parser.error('Use an authored globalfx fixture')
    if any(not re.fullmatch('[a-z0-9_-]+', value) for value in [args.go_run, args.name]):
        parser.error('Use simple lowercase run names')
    root = Path(__file__).resolve().parents[1]
    bank = root / 'tests/testdata/reference'
    path = bank / (args.fixture + '.json')
    expected = json.loads(path.read_text())
    spec = expected.get('global_palette_render')
    if spec is None:
        parser.error('Rejected or dynamic-age controls have no static render expectation')
    for ext, key in [('.cns', 'cns_sha256'), ('.air', 'air_sha256'),
                     ('.cmd', 'cmd_sha256'), ('.stage.def', 'stage_fixture_sha256')]:
        if digest(bank / (args.fixture + ext)) != expected[key]:
            raise RuntimeError('Authored source changed: ' + ext)
    if digest(bank / spec['calibration_json']) != spec['calibration_sha256']:
        raise RuntimeError('Native point calibration changed')
    crops = json.loads((bank / 'global-palette-crops.json').read_text())
    crop = crops['fixtures'][args.fixture]
    if digest(path) != crop['source_json_sha256']:
        raise RuntimeError('Frozen native observation changed')
    native = root / 'temp/reference-runs' / ('m4cs-' + args.fixture)
    nd = json.loads((native / 'evidence.json').read_text())
    if nd['captures'] != expected['captures'] or nd.get('load_error'):
        raise RuntimeError('Native numeric/capture evidence differs')
    if digest(native / 'stock/mugen.exe') != expected['engine_sha256']:
        raise RuntimeError('Reference executable changed')
    ni = opened(native, nd['captures'][0])
    if nd['captures'][0]['sha256'] != crop['native_png_sha256']:
        raise RuntimeError('Native stage crop belongs to another capture')
    for sample in spec['native_rgb_samples']:
        if list(ni.getpixel(tuple(sample['point']))) != sample['rgb']:
            raise RuntimeError('Native sample differs from its frozen capture')
    run = root / 'temp/app-runs' / args.go_run
    evidence = json.loads((run / 'evidence.json').read_text())
    if evidence.get('error') or not evidence.get('captures'):
        raise RuntimeError('Application capture failed')
    if evidence['players'] != ['chars/kfm/kfm.def'] * 2 or evidence['stage'] != 'stages/probe-stage.def':
        raise RuntimeError('Application selected another fixture')
    if digest(run / 'mugen-go') != evidence['binary_sha256'] or digest(run / 'stock/data/mugen.cfg') != evidence['config_sha256']:
        raise RuntimeError('Application executable/configuration changed')
    # Asset paths are identical disposable copies, including HUD/fonts and the
    # prepared character resources; generated config and native logs are omitted.
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
        errors = []
        for sample in spec['native_rgb_samples']:
            got = list(image.getpixel(tuple(sample['point'])))
            if got != sample['rgb']:
                errors.append({**sample, 'go': got})
        row = {'sample_count': len(spec['native_rgb_samples']), 'sample_errors': errors}
        for kind, region, want in [
            ('actor', spec['actor_region'], spec['native_actor_crop_sha256']),
            ('stage', crops['stage_region'], crop['native_stage_crop_sha256']),
        ]:
            actual, reference = image.crop(region), ni.crop(region)
            if hashlib.sha256(reference.tobytes()).hexdigest() != want:
                raise RuntimeError('Native crop changed: ' + kind)
            a, b = np.array(actual), np.array(reference)
            row[kind + '_rgb_differences'] = int(np.count_nonzero(np.any(a != b, axis=2)))
            row[kind + '_crop_pixels'] = a.shape[0] * a.shape[1]
        results.append(row)
    output = root / 'temp/global-palette-runs' / args.name
    output.mkdir(parents=True, exist_ok=False)
    (output / 'measurements.json').write_text(json.dumps({
        'fixture': args.fixture, 'go_run': args.go_run,
        'binary_sha256': evidence['binary_sha256'], 'captures': results,
    }, indent=2) + '\n')
    print(json.dumps(results), flush=True)
    if any(row['sample_errors'] or row['actor_rgb_differences'] or row['stage_rgb_differences'] for row in results):
        raise SystemExit('Strict native palette crop/sample comparison failed; goldens unchanged')


if __name__ == '__main__':
    main()
