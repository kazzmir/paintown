#!/usr/bin/env python3
"""Strict read-only comparison of GameMakeAnim controller outcomes with frozen beta captures."""
import argparse
import hashlib
import json
from pathlib import Path
import re

import numpy as np
from PIL import Image


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def opened(run, capture, size):
    path = run / capture['path']
    if digest(path) != capture['sha256']:
        raise RuntimeError('Capture changed: ' + str(path))
    image = Image.open(path).convert('RGB')
    if image.size != tuple(size):
        raise RuntimeError('Capture dimensions differ from the native recipe')
    return image


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True)
    parser.add_argument('--go-run', required=True)
    parser.add_argument('--name', required=True)
    args = parser.parse_args()
    if not re.fullmatch('(?:systemanim-game|systemearly-game|systemresource|gameanim|gameoverlap|gamephase|gamecontext|gameedge|gameorigin|gamevertical)-[a-z0-9-]+', args.fixture):
        parser.error('Use an authored GameMakeAnim observer fixture')
    if any(not re.fullmatch('[a-z0-9_-]+', value) for value in [args.go_run, args.name]):
        parser.error('Use simple lowercase run names')
    root = Path(__file__).resolve().parents[1]
    bank = root / 'tests/testdata/reference'
    expected = json.loads((bank / (args.fixture + '.json')).read_text())
    spec = expected.get('game_animation_render')
    if spec is None:
        parser.error('Rejected controls have no fixed-image expectation')
    for ext, key in [('.cns', 'cns_sha256'), ('.air', 'air_sha256'),
                     ('.cmd', 'cmd_sha256'), ('.stage.def', 'stage_fixture_sha256')]:
        if digest(bank / (args.fixture + ext)) != expected[key]:
            raise RuntimeError('Authored source changed: ' + ext)
    native = root / expected['native_run']
    if not native.resolve().is_relative_to(root / 'temp'):
        raise RuntimeError('Native artifact path escaped the task workspace')
    nd = json.loads((native / 'evidence.json').read_text())
    if nd['captures'] != expected['captures'] or nd.get('load_error'):
        raise RuntimeError('Native numeric/capture evidence differs')
    if digest(native / 'stock/mugen.exe') != expected['engine_sha256']:
        raise RuntimeError('Reference executable changed')
    configurations = {}
    for ledger in ['gamefirst-native-config-finalization.json','gameresource-native-config-finalization.json','gameschema-native-config-finalization.json','gameoverlap-native-config-finalization.json','gamephase-native-config-finalization.json','gamecontext-native-config-finalization.json','gameedge-native-config-finalization.json','gameorigin-native-config-finalization.json','gamevertical-native-config-finalization.json']:
        for name, recipe in json.loads((bank / ledger).read_text())['programs'].items():
            if name in configurations: raise RuntimeError('Duplicate native configuration recipe')
            configurations[name] = recipe
    finalization = configurations[args.fixture]
    if (nd.get('effect_limits', {}) != finalization['effect_limits']
            or finalization['prepared_sha256'] != expected['config_sha256']
            or digest(native / 'stock/data/mugen.cfg') != finalization['final_sha256']):
        raise RuntimeError('Native preparation configuration changed')
    if digest(native / 'stock/mugen.log') != expected['seed_log_sha256']:
        raise RuntimeError('Native seed log changed')
    seed = re.search(r'Match RNG seed: (\d+)', (native / 'stock/mugen.log').read_text())
    if seed is None or int(seed[1]) != expected['random_seed']:
        raise RuntimeError('Native initial Match RNG seed differs')
    if 'common_air_patch' in expected:
        patch = expected['common_air_patch']
        if digest(bank / (args.fixture + '.common.air')) != patch['appendix_sha256']:
            raise RuntimeError('Authored common AIR appendix changed')
        if digest(root / 'temp/mugen-1.1b1/data/fightfx.air') != patch['canonical_sha256']:
            raise RuntimeError('Canonical common AIR changed')
        if digest(native / 'stock/data/fightfx.air') != patch['prepared_sha256']:
            raise RuntimeError('Prepared native common AIR changed')
    if 'common_air_override' in expected:
        if 'common_air_patch' in expected:
            raise RuntimeError('Common AIR cannot be both appended and replaced')
        patch = expected['common_air_override']
        if (digest(bank / (args.fixture + '.common.air')) != patch['replacement_sha256']
                or digest(root / 'temp/mugen-1.1b1/data/fightfx.air') != patch['canonical_sha256']
                or digest(native / 'stock/data/fightfx.air') != patch['prepared_sha256']
                or patch['replacement_sha256'] != patch['prepared_sha256']):
            raise RuntimeError('Exact common AIR replacement recipe changed')
    ni = opened(native, nd['captures'][0], spec['image_size'])
    for capture in nd['captures']:
        image = opened(native, capture, spec['image_size'])
        if hashlib.sha256(image.crop(spec['region']).tobytes()).hexdigest() != spec['native_crop_sha256']:
            raise RuntimeError('Native clipboard pages have different world captures')
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
    if expected.get('recipe_sha256'):
        recipe_path = bank / (args.fixture + '.input.json')
        recipe = json.loads(recipe_path.read_text())
        recorded = evidence.get('input_recipe', {})
        if (digest(recipe_path) != expected['recipe_sha256']
                or recorded.get('sha256') != expected['recipe_sha256'] or recorded.get('recipe') != recipe):
            raise RuntimeError('Application input recipe differs from native')
        when = recipe['settle_seconds']
        wanted = []
        for event in recipe['events']:
            wanted.extend((when, 'keydown', key) for key in event['keys'])
            when += event['hold_seconds']
            wanted.extend((when, 'keyup', key) for key in event['keys'])
            when += event['gap_seconds']
        events = recorded.get('events', [])
        if [(e['seconds'], e['operation'], e['key']) for e in events] != wanted:
            raise RuntimeError('Application did not complete exact owned-input events')
        if len({(e['pid'], e['window']) for e in events}) != 1:
            raise RuntimeError('Application input ownership changed')
    elif evidence.get('input_recipe'):
        raise RuntimeError('Unexpected application input recipe')
    actual_seeds = [int(n) for n in re.findall(r'Match RNG seed: (\d+)', (run / 'run.log').read_text())]
    if evidence.get('match_random_seed') != expected['random_seed'] or actual_seeds != [expected['random_seed']]:
        raise RuntimeError('Application did not reproduce the native initial Match RNG seed')
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
        image = opened(run, capture, spec['image_size'])
        actual = image.crop(spec['region'])
        a, b = np.array(actual), np.array(reference)
        results.append({'rgb_differences': int(np.count_nonzero(np.any(a != b, axis=2))),
                        'crop_pixels': a.shape[0] * a.shape[1],
                        'max_rgb_error': int(np.max(np.abs(a.astype(int)-b.astype(int))))})
    output = root / 'temp/game-animation-runs' / args.name
    output.mkdir(parents=True, exist_ok=False)
    (output / 'measurements.json').write_text(json.dumps({
        'fixture': args.fixture, 'go_run': args.go_run,
        'binary_sha256': evidence['binary_sha256'], 'captures': results,
    }, indent=2) + '\n')
    print(json.dumps(results), flush=True)
    if any(row['rgb_differences'] for row in results):
        raise SystemExit('Strict native GameMakeAnim outcome comparison failed; goldens unchanged')


if __name__ == '__main__':
    main()
