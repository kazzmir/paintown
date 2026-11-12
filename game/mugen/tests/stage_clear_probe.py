#!/usr/bin/env python3
"""Check native diagnostic stage fill and viewport bars using solid-color regions.

Only original stageclear fixtures can record native goldens. No reference art
or window pixels are saved in the source corpus, and Go cannot record a golden.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re

from PIL import Image


BOX = (900, 660, 1100, 700)
POINTS = ((100, 640), (1180, 640), (1000, 640))
COLORS = {(0, 0, 0), (255, 0, 255)}
PREPARED = ('chars/kfm/probe.cns', 'chars/kfm/probe.cmd',
            'chars/kfm/probe.air', 'chars/kfm/kfm.def',
            'chars/kfm/kfm.sff', 'stages/probe-stage.def',
            'stages/kfm.sff', 'data/mugen1/fight.def')


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def measure(path):
    image = Image.open(path).convert('RGB')
    if image.size != (1280, 720):
        raise RuntimeError('Expected the calibrated 1280x720 window')
    region = image.crop(BOX)
    colors = set(region.getdata())
    if len(colors) != 1 or not colors <= COLORS:
        raise RuntimeError('Clear region is not uniformly black or magenta')
    samples = [list(image.getpixel(point)) for point in POINTS]
    if any(tuple(rgb) not in COLORS for rgb in samples):
        raise RuntimeError('Viewport sample is obscured or has an unexpected color')
    return dict(window=list(image.size), box=list(BOX), rgb=list(next(iter(colors))),
                pixels=region.width * region.height,
                rgb_sha256=hashlib.sha256(region.tobytes()).hexdigest(),
                sample_points=[list(p) for p in POINTS], samples=samples)


def profile(stock, expected):
    source = (stock / 'data/mugen.cfg').read_text()
    values = dict(GameWidth=1280, GameHeight=720, Width=1280, Height=720,
                  StageFit=1, KeepAspect=1, SystemFit=1)
    values.update(expected.get('video', {}))
    for key, value in values.items():
        actual = re.search(r'(?im)^\s*' + key + r'\s*=\s*(\d+)\s*$', source)
        if actual is None and key in ['Width', 'Height'] and key not in expected.get('video', {}):
            # Native window dimensions default to the configured game size.
            actual = re.search(r'(?im)^\s*Game' + key + r'\s*=\s*(\d+)\s*$', source)
        if actual is None or int(actual[1]) != value:
            raise RuntimeError('Render profile differs: ' + key)


def main():
    project = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True,
                        choices=['stageclear-' + n for n in
                                 ['omitted', 'zero', 'one', 'negative', 'two', 'stage0', 'stretch',
                                  'half', 'onehalf', 'word', 'negativehalf']])
    parser.add_argument('--native-run')
    parser.add_argument('--go-run')
    parser.add_argument('--record', action='store_true')
    args = parser.parse_args()
    if bool(args.native_run) == bool(args.go_run) or args.go_run and args.record:
        parser.error('Choose one native or Go run; only native may record')
    name = args.native_run or args.go_run
    if not re.fullmatch('[a-z0-9_-]+', name):
        parser.error('Use a simple lowercase run name')
    stem = project / 'tests/testdata/reference' / args.fixture
    expected_path = stem.with_suffix('.json')
    expected = json.loads(expected_path.read_text())
    for ext, key in [('.cns', 'cns_sha256'), ('.cmd', 'cmd_sha256'),
                     ('.air', 'air_sha256'), ('.stage.def', 'stage_fixture_sha256')]:
        if digest(stem.with_suffix(ext)) != expected[key]:
            raise RuntimeError('Authored input changed: ' + ext)
    runs = 'reference-runs' if args.native_run else 'app-runs'
    run = project / 'temp' / runs / name
    evidence = json.loads((run / 'evidence.json').read_text())
    if evidence.get('error') or evidence.get('load_error') or not evidence.get('captures'):
        raise RuntimeError('Run failed or has no captures')
    stock = run / 'stock'
    profile(stock, expected)
    prepared = {rel: digest(stock / rel) for rel in PREPARED}
    if args.native_run:
        for key in ['engine_sha256', 'cns_sha256', 'air_sha256', 'cmd_sha256',
                    'stage_fixture_sha256', 'bank', 'mugenversion', 'fixture_stage',
                    'clipboard_delay', 'video']:
            if evidence.get(key) != expected.get(key):
                raise RuntimeError('Native recipe differs: ' + key)
        if digest(stock / 'mugen.exe') != expected['engine_sha256']:
            raise RuntimeError('Native executable differs')
        if [c['variables'] for c in evidence['captures']] != [c['variables'] for c in expected['captures']]:
            raise RuntimeError('Native numeric observations differ')
        if not args.record:
            pinned = expected['stage_clear_provenance']
            if prepared != pinned['prepared_sha256'] or digest(stock / 'data/mugen.cfg') != pinned['config_sha256']:
                raise RuntimeError('Native prepared inputs/configuration differ')
    else:
        if not expected.get('stage_clear_render'):
            raise RuntimeError('Record native clear-color evidence first')
        if prepared != expected['stage_clear_provenance']['prepared_sha256']:
            raise RuntimeError('Go prepared player/stage/HUD inputs differ')
        if digest(run / 'mugen-go') != evidence['binary_sha256'] or digest(stock / 'data/mugen.cfg') != evidence['config_sha256']:
            raise RuntimeError('Go executable or configuration changed')
        log = (run / 'run.log').read_text().lower()
        if any(text in log for text in ['runtime error', 'panic:', 'missing resource', 'failed to load', 'cannot load']):
            raise RuntimeError('Go runtime/resource failure; inspect run.log')
    results = []
    for capture in evidence['captures']:
        path = run / capture['path']
        if digest(path) != capture['sha256']:
            raise RuntimeError('Captured window hash differs')
        result = measure(path)
        if not args.record and result != expected['stage_clear_render']:
            raise RuntimeError('Clear color or viewport samples differ: ' + str(result))
        results.append(result)
    if args.record:
        if any(result != results[0] for result in results):
            raise RuntimeError('Native measurements are not stable')
        expected['stage_clear_render'] = results[0]
        expected['stage_clear_provenance'] = dict(native_run=name,
                                                 config_sha256=digest(stock / 'data/mugen.cfg'),
                                                 prepared_sha256=prepared)
        expected_path.write_text(json.dumps(expected, indent=2) + '\n')
    output = project / 'temp/stage-clear-runs' / name
    output.mkdir(parents=True, exist_ok=False)
    (output / 'measurements.json').write_text(json.dumps(dict(fixture=args.fixture,
                                                             native=bool(args.native_run),
                                                             measurements=results), indent=2) + '\n')
    print(('Recorded native ' if args.record else 'Verified ') + args.fixture, flush=True)


if __name__ == '__main__':
    main()
