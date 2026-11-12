#!/usr/bin/env python3
"""Replay the original red sprite's native flip/offset raster calibration.

These fixed camera scenes use a black region behind one opaque RGB marker.
They cover the recorded render profiles, not every asset or renderer feature.
"""
import argparse
import base64
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys
import zlib

from PIL import Image


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def measure(image):
    if image.size != (1280, 720):
        raise RuntimeError('Expected the calibrated 1280x720 window')
    rgb = (220, 30, 40)
    points = [(x, y) for y in range(450, 674) for x in range(160, 1120)
              if image.getpixel((x, y)) == rgb]
    if not points:
        raise RuntimeError('Original solid red marker is absent')
    bounds = [min(x for x, y in points), min(y for x, y in points),
              max(x for x, y in points) + 1, max(y for x, y in points) + 1]
    region = [bounds[0] - 4, bounds[1] - 4, bounds[2] + 4, bounds[3] + 4]
    return {'window': list(image.size), 'rgb': list(rgb),
            'pure_color_pixels': len(points), 'pure_color_bounds': bounds,
            'crop_region': region,
            'crop_sha256': hashlib.sha256(image.crop(tuple(region)).tobytes()).hexdigest()}


def check_inputs(stem, expected):
    for suffix, key in [('.cns', 'cns_sha256'), ('.air', 'air_sha256'),
                        ('.cmd', 'cmd_sha256'), ('.stage.def', 'stage_fixture_sha256')]:
        if digest(stem.with_suffix(suffix)) != expected[key]:
            raise RuntimeError('Authored input changed: ' + suffix)
    if digest(stem.with_suffix('.extra') / 'probe.sff') != expected['extra_sha256']['probe.sff']:
        raise RuntimeError('Original marker bank changed')


def check_go(project, stem, expected, args):
    if not expected.get('sprite_render'):
        raise RuntimeError('Record native raster evidence before checking Go')
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
        raise RuntimeError('Go marker bank differs')
    if digest(char / 'probe.air') != expected['air_sha256']:
        raise RuntimeError('Go AIR differs')
    if not (char / 'probe.cns').read_text().rstrip().endswith(stem.with_suffix('.cns').read_text().rstrip()):
        raise RuntimeError('Go prepared CNS differs from the authored program')
    if digest(run / 'stock/stages/probe-stage.def') != expected['stage_fixture_sha256']:
        raise RuntimeError('Go stage differs')
    config = (run / 'stock/data/mugen.cfg').read_text()
    for key, value in [('GameWidth', expected.get('game_width', 1280)),
                       ('StageFit', 1), ('KeepAspect', 1)]:
        actual = re.search(r'(?im)^\s*' + key + r'\s*=\s*(\d+)\s*$', config)
        if actual is None or int(actual[1]) != value:
            raise RuntimeError('Go render profile differs: ' + key)
    log = (run / 'run.log').read_text().lower()
    if any(text in log for text in ['runtime error', 'panic:', 'missing resource', 'failed to load', 'cannot load']):
        raise RuntimeError('Go runtime/resource error; inspect run.log')
    results = []
    for capture in evidence['captures']:
        path = run / capture['path']
        if digest(path) != capture['sha256']:
            raise RuntimeError('Go captured window hash differs')
        image = Image.open(path).convert('RGB')
        result = measure(image)
        want = expected['sprite_render']
        if any(result[key] != want[key] for key in want if key != 'crop_sha256'):
            raise RuntimeError('Go sprite geometry differs: ' + str(result))
        differences, max_error = 0, 0
        if result['crop_sha256'] != want['crop_sha256']:
            if not args.max_edge_channel_error:
                raise RuntimeError('Go sprite raster differs: ' + str(result))
            native = zlib.decompress(base64.b64decode(expected['sprite_crop_rgb_zlib'], validate=True))
            actual = image.crop(tuple(want['crop_region'])).tobytes()
            if len(native) != len(actual) or hashlib.sha256(native).hexdigest() != want['crop_sha256']:
                raise RuntimeError('Native original-marker raster data is invalid')
            for i in range(0, len(native), 3):
                a, b = native[i:i+3], actual[i:i+3]
                if a == b:
                    continue
                differences += 1
                error = max(abs(x-y) for x, y in zip(a, b))
                max_error = max(max_error, error)
                if a in [bytes(3), bytes(want['rgb'])] or error > args.max_edge_channel_error:
                    raise RuntimeError('Go differs outside the allowed blended-edge precision')
            if differences > 4:
                raise RuntimeError('Go differs at more than four blended edge pixels')
        results.append({'seconds': capture['seconds'], 'render': result,
                        'different_edge_pixels': differences, 'max_channel_error': max_error})
    output = project / 'temp/sprite-render-runs' / args.name
    output.mkdir(parents=True, exist_ok=False)
    (output / 'measurements.json').write_text(json.dumps({'go_run': args.go_run, 'captures': results}, indent=2) + '\n')
    print('Verified Go ' + args.fixture + ': ' + str(len(results)) + ' marker crops; ' +
          str(sum(r['different_edge_pixels'] for r in results)) + ' blended edge pixels differ', flush=True)


def main():
    project = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True)
    parser.add_argument('--name', required=True)
    parser.add_argument('--adopt-run')
    parser.add_argument('--go-run', help='Verify an existing app_smoke run against native raster goldens')
    parser.add_argument('--record', action='store_true')
    parser.add_argument('--max-edge-channel-error', type=int, choices=[0, 1], default=0,
                        help='Go only: allow one channel level at up to four blended edge pixels; geometry stays exact')
    args = parser.parse_args()
    if not re.fullmatch('flipmark-[a-z0-9-]+', args.fixture):
        parser.error('Use an authored flipmark fixture')
    if not all(re.fullmatch('[a-z0-9_-]+', value) for value in
               [args.name] + ([args.adopt_run] if args.adopt_run else []) +
               ([args.go_run] if args.go_run else [])):
        parser.error('Use simple lowercase run names')
    if args.go_run and (args.record or args.adopt_run):
        parser.error('Go verification cannot record or adopt native goldens')
    if args.max_edge_channel_error and not args.go_run:
        parser.error('Edge precision applies only to Go verification')
    stem = project / 'tests/testdata/reference' / args.fixture
    expected = json.loads(stem.with_suffix('.json').read_text())
    if expected.get('load_error'):
        raise RuntimeError('Unexpected rejected sprite calibration input')
    check_inputs(stem, expected)
    if args.go_run:
        check_go(project, stem, expected, args)
        return
    if not args.adopt_run:
        command = [sys.executable, str(project / 'tests/reference_probe.py'),
                   '--fixture', args.fixture, '--name', args.name,
                   '--fixture-stage', '--bank', 'fvar', '--mugenversion', '1.1',
                   '--clipboard-in-cmd', '--file', 'sprite=probe.sff',
                   '--pages', '0', '--seconds', '7']
        if expected.get('game_width'):
            command += ['--game-width', str(expected['game_width'])]
        for key, value in expected.get('video', {}).items():
            command += ['--video', key + '=' + str(value)]
        subprocess.run(command, check=True)
    run = project / 'temp/reference-runs' / (args.adopt_run or args.name)
    evidence = json.loads((run / 'evidence.json').read_text())
    for key in ['engine_sha256', 'cns_sha256', 'air_sha256', 'cmd_sha256',
                'stage_fixture_sha256', 'extra_sha256', 'files', 'mugenversion',
                'clipboard_in_cmd', 'fixture_stage', 'bank', 'game_width',
                'video', 'config_sha256']:
        if evidence.get(key) != expected.get(key):
            raise RuntimeError('Native recipe differs: ' + key)
    capture = evidence['captures'][0]
    if capture['variables'] != expected['captures'][0]['variables']:
        raise RuntimeError('Native identity/clock observations differ')
    image_path = run / capture['path']
    if digest(image_path) != capture['sha256']:
        raise RuntimeError('Native captured window hash differs')
    image = Image.open(image_path).convert('RGB')
    result = measure(image)
    if args.record:
        expected['sprite_render'] = result
        pixels = image.crop(tuple(result['crop_region'])).tobytes()
        expected['sprite_crop_rgb_zlib'] = base64.b64encode(zlib.compress(pixels, 9)).decode('ascii')
        stem.with_suffix('.json').write_text(json.dumps(expected, indent=2) + '\n')
    elif result != expected.get('sprite_render'):
        raise RuntimeError('Native sprite rectangle/raster differs: ' + str(result))
    output = project / 'temp/sprite-render-runs' / args.name
    output.mkdir(parents=True, exist_ok=False)
    (output / 'measurements.json').write_text(json.dumps(result, indent=2) + '\n')
    print(('Recorded ' if args.record else 'Verified ') + args.fixture + ': ' + str(result), flush=True)


if __name__ == '__main__':
    main()
