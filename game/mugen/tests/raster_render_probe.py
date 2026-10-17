#!/usr/bin/env python3
"""Verify original patterned sprites against native framebuffer crops.

The fixed crop contains only the authored sprite and the black lower stage.
Stock artwork must never be recorded as a golden by this tool.
"""
import argparse
import base64
import collections
import hashlib
import json
from pathlib import Path
import re
import sys
import zlib

from PIL import Image

sys.dont_write_bytecode = True
from generate_actor_markers import archive as solid_archive
from generate_raster_markers import archive as pattern_archive

REGION = (400, 450, 800, 620)


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def raster(path):
    image = Image.open(path).convert('RGB')
    if image.size != (1280, 720):
        raise RuntimeError('Expected the calibrated 1280x720 window')
    return image.crop(REGION).tobytes()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True)
    parser.add_argument('--name', required=True)
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument('--native-run')
    source.add_argument('--go-run')
    parser.add_argument('--record', action='store_true')
    args = parser.parse_args()
    if not re.fullmatch('raster-(marker|pattern)-[a-z0-9-]+', args.fixture):
        parser.error('Only the original raster markers/patterns can be recorded')
    if not all(re.fullmatch('[a-z0-9_-]+', value) for value in
               [args.name, args.native_run or args.go_run]):
        parser.error('Use simple lowercase run names')
    if args.go_run and args.record:
        parser.error('Go output cannot record native goldens')
    project = Path(__file__).resolve().parents[1]
    stem = project / 'tests/testdata/reference' / args.fixture
    expected = json.loads(stem.with_suffix('.json').read_text())
    for suffix, key in [('.cns', 'cns_sha256'), ('.cmd', 'cmd_sha256'),
                        ('.air', 'air_sha256'), ('.stage.def', 'stage_fixture_sha256')]:
        if digest(stem.with_suffix(suffix)) != expected[key]:
            raise RuntimeError('Authored input changed: ' + suffix)
    bank = stem.with_suffix('.extra') / 'probe.sff'
    if digest(bank) != expected['extra_sha256']['probe.sff']:
        raise RuntimeError('Original sprite archive changed')
    originals = [solid_archive()] + [pattern_archive(indexed, fine, lz5, power2)
                                    for indexed in [False, True] for fine in [False, True]
                                    for lz5 in ([False, True] if indexed else [False])
                                    for power2 in [False, True]]
    if bank.read_bytes() not in originals:
        raise RuntimeError('Raster bank is not one of the original generated patterns')
    run = project / ('temp/app-runs' if args.go_run else 'temp/reference-runs') / (args.go_run or args.native_run)
    evidence = json.loads((run / 'evidence.json').read_text())
    if evidence.get('error') or evidence.get('load_error') or not evidence.get('captures'):
        raise RuntimeError('Capture run failed')
    config_hash = digest(run / 'stock/data/mugen.cfg')
    if args.go_run or 'config_sha256' in evidence:
        if config_hash != evidence['config_sha256']:
            # Native clears FirstRun and writes CRLF on shutdown. Verify the
            # original hash after reversing only these measured rewrites.
            normalized = (run / 'stock/data/mugen.cfg').read_bytes().replace(b'\r\n', b'\n')
            first_run = normalized.replace(b'FirstRun = 0\n', b'FirstRun = 1\n', 1)
            if args.go_run or evidence['config_sha256'] not in [hashlib.sha256(value).hexdigest() for value in [normalized, first_run]]:
                raise RuntimeError('Prepared config changed')
    if args.native_run:
        if digest(run / 'stock/mugen.exe') != expected['engine_sha256']:
            raise RuntimeError('Native executable changed')
        if args.record:
            expected['pattern_native_config_sha256'] = config_hash
        elif config_hash != expected['pattern_native_config_sha256']:
            raise RuntimeError('Native render config differs')
    for key, value in [('GameWidth', expected.get('game_width', 1280)),
                       ('StageFit', 1), ('KeepAspect', 1)]:
        config = (run / 'stock/data/mugen.cfg').read_text()
        actual = re.search(r'(?im)^\s*' + key + r'\s*=\s*(\d+)\s*$', config)
        if actual is None or int(actual[1]) != value:
            raise RuntimeError('Render profile differs: ' + key)
    char = run / 'stock/chars/kfm'
    if not (char / 'probe.cns').read_text().rstrip().endswith(stem.with_suffix('.cns').read_text().rstrip()):
        raise RuntimeError('Prepared CNS differs')
    if stem.with_suffix('.cmd').read_text().strip() not in (char / 'probe.cmd').read_text():
        raise RuntimeError('Prepared CMD differs')
    for path, want in [(char / 'probe.sff', expected['extra_sha256']['probe.sff']),
                       (char / 'probe.air', expected['air_sha256']),
                       (run / 'stock/stages/probe-stage.def', expected['stage_fixture_sha256'])]:
        if digest(path) != want:
            raise RuntimeError('Prepared input changed: ' + str(path))
    if args.go_run:
        if digest(run / 'mugen-go') != evidence['binary_sha256']:
            raise RuntimeError('Go binary changed')
        log = (run / 'run.log').read_text().lower()
        if any(text in log for text in ['runtime error', 'panic:', 'missing resource', 'failed to load', 'cannot load']):
            raise RuntimeError('Go runtime/resource error')
    else:
        for key in ['engine_sha256', 'cns_sha256', 'air_sha256', 'cmd_sha256',
                    'stage_fixture_sha256', 'extra_sha256', 'files', 'mugenversion',
                    'clipboard_in_cmd', 'fixture_stage', 'bank', 'game_width', 'video', 'config_sha256']:
            if evidence.get(key) != expected.get(key):
                raise RuntimeError('Native recipe differs: ' + key)
        if [c['variables'] for c in evidence['captures']] != [c['variables'] for c in expected['captures']]:
            raise RuntimeError('Native numeric observations differ')
    results = []
    for capture in evidence['captures']:
        path = run / capture['path']
        if digest(path) != capture['sha256']:
            raise RuntimeError('Captured window hash changed')
        actual = raster(path)
        measured = {'region': list(REGION), 'sha256': hashlib.sha256(actual).hexdigest()}
        if args.record:
            expected['pattern_render'] = measured
            expected['pattern_crop_rgb_zlib'] = base64.b64encode(zlib.compress(actual, 9)).decode('ascii')
            stem.with_suffix('.json').write_text(json.dumps(expected, indent=2) + '\n')
        want = expected['pattern_render']
        native = zlib.decompress(base64.b64decode(expected['pattern_crop_rgb_zlib'], validate=True))
        if want['region'] != list(REGION) or len(native) != len(actual) or hashlib.sha256(native).hexdigest() != want['sha256']:
            raise RuntimeError('Invalid native raster golden')
        errors = collections.Counter(max(abs(x-y) for x, y in zip(native[i:i+3], actual[i:i+3]))
                                     for i in range(0, len(native), 3))
        results.append({'capture': capture['path'], 'raster': measured,
                        'pixel_errors': dict(sorted(errors.items())), 'max_channel_error': max(errors)})
        if max(errors) != 0:
            raise RuntimeError('Sprite raster differs: ' + json.dumps(results[-1]))
    output = project / 'temp/raster-render-runs' / args.name
    output.mkdir(parents=True, exist_ok=False)
    (output / 'measurements.json').write_text(json.dumps(results, indent=2) + '\n')
    print(('Recorded' if args.record else 'Verified') + ' ' + args.fixture + ': ' + json.dumps(results), flush=True)


if __name__ == '__main__':
    main()
