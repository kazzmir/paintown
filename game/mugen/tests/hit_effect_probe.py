#!/usr/bin/env python3
"""Compare owned HitDef palette colors or rigid shake displacement.

Requires prior native numeric evidence and private-display app captures. Stores
only a few numeric colors/coordinates or a displacement, never stock artwork.
Go output cannot create or update a native expectation.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
from PIL import Image

REGION = (720, 260, 1030, 650)
PORTRAIT = (1028, 14, 1108, 89)
CLOTH = (247, 239, 231)
PREPARED = ('chars/kfm/kfm.def', 'chars/kfm/kfm.cns', 'chars/kfm/kfm.sff',
            'chars/kfm/probe.cns', 'chars/kfm/probe.air', 'chars/kfm/probe.cmd',
            'stages/probe-stage.def', 'stages/kfm.sff', 'data/mugen1/fight.def')


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def prepared_inputs(stock):
    names = list(PREPARED)
    opponent = stock/'chars/probeopponent'
    if opponent.exists():
        names.extend(str(path.relative_to(stock)) for path in sorted(opponent.iterdir()) if path.is_file())
    return {name: digest(stock/name) for name in names}


def opened(path, expected):
    if digest(path) != expected['sha256']:
        raise RuntimeError('Captured window changed: ' + str(path))
    image = Image.open(path).convert('RGB')
    if image.size != (1280, 720):
        raise RuntimeError('Expected calibrated 1280x720 window')
    return image


def mask(image):
    x0, y0, x1, y1 = REGION
    return {(x, y) for y in range(y0, y1) for x in range(x0, x1)
            if image.getpixel((x, y)) == CLOTH}


def measure_palette(image, baseline, points=None, portrait_baseline=None):
    if points is None:
        # Fixed colors and interior points avoid edge filtering and stock art
        # storage. Pick a small, reproducible subset from the native baseline.
        candidates = sorted(mask(baseline))
        inside = [(x, y) for x, y in candidates
                  if all(baseline.getpixel((x+dx, y+dy)) == CLOTH
                         for dx in [-1, 0, 1] for dy in [-1, 0, 1])]
        if len(inside) < 100:
            raise RuntimeError('Native receiver garment is absent/occluded')
        points = [list(inside[i * (len(inside)-1)//11]) for i in range(12)]
    portrait_baseline = portrait_baseline or baseline
    difference = sum(a != b for a, b in zip(image.crop(PORTRAIT).getdata(), portrait_baseline.crop(PORTRAIT).getdata()))
    return {'points': points, 'rgb': [list(image.getpixel(tuple(p))) for p in points], 'portrait_region': list(PORTRAIT), 'portrait_delta_pixels': difference}


def measure_shake(image, baseline):
    original, actual = mask(baseline), mask(image)
    if len(original) < 100 or len(original) != len(actual):
        raise RuntimeError('Receiver garment count differs or is occluded')
    shifts = [dy for dy in range(-64, 65)
              if {(x, y+dy) for x, y in original} == actual]
    if len(shifts) != 1:
        raise RuntimeError('No unique rigid garment displacement: ' + str(shifts))
    return {'offset_y': shifts[0], 'garment_pixels': len(original), 'region': list(REGION)}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True)
    parser.add_argument('--name', required=True)
    runs = parser.add_mutually_exclusive_group(required=True)
    runs.add_argument('--native-run')
    runs.add_argument('--go-run')
    parser.add_argument('--baseline-run')
    parser.add_argument('--go-baseline-run')
    parser.add_argument('--record', action='store_true')
    args = parser.parse_args()
    if not re.fullmatch('(hitpalette|hitshake|fallshake)-[a-z0-9-]+', args.fixture):
        parser.error('Use an authored HitDef effect fixture')
    for value in [args.name, args.native_run or args.go_run, args.baseline_run or 'default']:
        if not re.fullmatch('[a-z0-9_-]+', value):
            parser.error('Use simple lowercase run names')
    if args.go_run and args.record:
        parser.error('Go output cannot record native expectations')
    project = Path(__file__).resolve().parents[1]
    stem = project/'tests/testdata/reference'/args.fixture
    path = stem.with_suffix('.json')
    expected = json.loads(path.read_text())
    for ext, key in [('.cns','cns_sha256'),('.air','air_sha256'),('.cmd','cmd_sha256'),('.stage.def','stage_fixture_sha256')]:
        if digest(stem.with_suffix(ext)) != expected[key]:
            raise RuntimeError('Authored source changed: ' + ext)
    if expected.get('load_error'):
        parser.error('Rejected fixtures have no render expectation')
    prefix = args.fixture.split('-')[0]
    baseline_name = args.baseline_run or prefix+'-defaults-native'
    baseline_run = project/'temp/reference-runs'/baseline_name
    baseline_data = json.loads((baseline_run/'evidence.json').read_text())
    if baseline_data.get('load_error'):
        raise RuntimeError('Baseline was rejected')
    baseline = opened(baseline_run/baseline_data['captures'][0]['path'], baseline_data['captures'][0])
    folder = 'app-runs' if args.go_run else 'reference-runs'
    run = project/'temp'/folder/(args.go_run or args.native_run)
    evidence = json.loads((run/'evidence.json').read_text())
    if evidence.get('error') or evidence.get('load_error') or not evidence.get('captures'):
        raise RuntimeError('Capture run failed')
    stock = run/'stock'
    prepared = prepared_inputs(stock)
    if stem.with_suffix('.cns').read_text() not in (stock/'chars/kfm/probe.cns').read_text():
        raise RuntimeError('Prepared CNS does not contain the frozen authored program')
    config = (stock/'data/mugen.cfg').read_text()
    for field, wanted in [('GameWidth',1280),('StageFit',1),('KeepAspect',1)]:
        actual = re.search(r'(?im)^\s*'+field+r'\s*=\s*(\d+)\s*$',config)
        if actual is None or int(actual[1]) != wanted:
            raise RuntimeError('Effect viewport profile differs: '+field)
    key = 'hit_effect_render'
    if args.go_run:
        if key not in expected:
            raise RuntimeError('Record native effect measurement first')
        if prepared != expected[key]['prepared_sha256']:
            raise RuntimeError('Go prepared character/stage/HUD inputs differ')
        if digest(run/'mugen-go') != evidence['binary_sha256'] or digest(stock/'data/mugen.cfg') != evidence['config_sha256']:
            raise RuntimeError('Go binary or configuration changed')
        log = (run/'run.log').read_text().lower()
        if any(p in log for p in ['panic:', 'runtime error', 'missing resource', 'failed to load', 'cannot load']):
            raise RuntimeError('Go runtime/resource error')
    else:
        if digest(stock/'mugen.exe') != expected['engine_sha256']:
            raise RuntimeError('Native executable changed')
        for field in ['engine_sha256','cns_sha256','air_sha256','cmd_sha256','stage_fixture_sha256','mugenversion','fixture_stage','bank']:
            if evidence.get(field) != expected.get(field):
                raise RuntimeError('Native recipe differs: '+field)
        if [c['variables'] for c in evidence['captures']] != [c['variables'] for c in expected['captures']]:
            raise RuntimeError('Native numeric observations differ')
    if not args.record and (baseline_name != expected[key]['baseline_run'] or baseline_data['captures'][0]['sha256'] != expected[key]['baseline_png_sha256']):
        raise RuntimeError('Baseline differs from the frozen render recipe')
    portrait_baseline = baseline
    if args.go_run and prefix == 'hitpalette':
        if not args.go_baseline_run or not re.fullmatch('[a-z0-9_-]+', args.go_baseline_run):
            parser.error('Palette checks require a Go neutral-baseline app run')
        neutral = project/'temp/app-runs'/args.go_baseline_run
        neutral_data = json.loads((neutral/'evidence.json').read_text())
        if neutral_data['binary_sha256'] != evidence['binary_sha256'] or digest(neutral/'mugen-go') != evidence['binary_sha256']:
            raise RuntimeError('Neutral baseline uses a different application binary')
        if prepared_inputs(neutral/'stock') != expected[key]['baseline_prepared_sha256']:
            raise RuntimeError('Neutral baseline prepared assets differ')
        portrait_baseline = opened(neutral/neutral_data['captures'][0]['path'], neutral_data['captures'][0])
    measurements = []
    for capture in evidence['captures']:
        image = opened(run/capture['path'], capture)
        if prefix == 'hitpalette':
            points = None if args.record else expected[key]['measurement']['points']
            measurement = measure_palette(image, baseline, points, portrait_baseline)
        else:
            measurement = measure_shake(image, baseline)
        if args.record:
            expected[key] = dict(baseline_run=baseline_name,
                                 baseline_png_sha256=baseline_data['captures'][0]['sha256'],
                                 baseline_prepared_sha256=prepared_inputs(baseline_run/'stock'),
                                 prepared_sha256=prepared, measurement=measurement)
        elif measurement != expected[key]['measurement']:
            raise RuntimeError('Effect colors/displacement differ: '+str(measurement))
        measurements.append(measurement)
    if args.record:
        path.write_text(json.dumps(expected, indent=2)+'\n')
    output = project/'temp/hit-effect-runs'/args.name
    output.mkdir(parents=True, exist_ok=False)
    (output/'measurements.json').write_text(json.dumps(dict(fixture=args.fixture, captures=measurements), indent=2)+'\n')
    print('Verified '+args.fixture+': '+str(len(measurements))+' effect measurements')


if __name__ == '__main__':
    main()
