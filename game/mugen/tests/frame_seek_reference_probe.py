#!/usr/bin/env python3
"""Observe an authored native frame/key sequence without injecting clipboard text.

The authored fixture owns CNS/CMD/AIR/stage. Numeric decoding is opt-in and only
reads pages already authored in that fixture. Original recorders stay unchanged.
This observer seeks to an authored GameTime before collecting exact step captures.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time
from PIL import Image
import reference_probe as legacy


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', type=Path, required=True)
    parser.add_argument('--name', required=True)
    parser.add_argument('--frozen-inputs', type=Path, required=True)
    parser.add_argument('--sequence', type=Path, required=True)
    parser.add_argument('--numeric-page', type=int)
    parser.add_argument('--warmup', type=float, default=7)
    args = parser.parse_args()
    fixture = args.fixture.resolve()
    if not fixture.is_relative_to(root) or not re.fullmatch('[a-z0-9_-]+', fixture.name):
        parser.error('Fixture must be an authored stem under game/mugen')
    if not re.fullmatch('[a-z0-9_-]+', args.name) or not 3 <= args.warmup <= 10:
        parser.error('Use a fresh simple run name and a warmup from3 to10 seconds')
    sequence_path = args.sequence.resolve()
    if not sequence_path.is_relative_to(root):
        parser.error('Sequence must be authored under game/mugen')
    sequence = json.loads(sequence_path.read_text())
    if not isinstance(sequence, list) or not 1 <= len(sequence) <= 40:
        parser.error('Use a bounded sequence of at most40 actions')
    labels = set()
    total = 0
    for action in sequence:
        if set(action) == {'key'}:
            if action['key'] not in {'Pause', 'Scroll_Lock', 'F4'}:
                parser.error('Only owned native Pause/ScrollLock/F4 controls are allowed')
        elif set(action) == {'seek_game_time'}:
            if type(action['seek_game_time']) is not int or not 1 <= action['seek_game_time'] <= 1000:
                parser.error('Seek target must be an authored integer GameTime1 through1000')
            total += 250
        elif set(action) == {'steps'}:
            if type(action['steps']) is not int or not 1 <= action['steps'] <= 250:
                parser.error('Step groups require integers1 through250')
            total += action['steps']
        elif set(action) == {'capture', 'numeric'}:
            if not re.fullmatch('[a-z0-9_-]+', action['capture']) or action['capture'] in labels or type(action['numeric']) is not bool:
                parser.error('Capture names must be unique and numeric is a bool')
            labels.add(action['capture'])
        else:
            parser.error('Unknown frame sequence action')
    if total > 500 or not labels:
        parser.error('At most500 steps and at leastone capture are required')
    if args.numeric_page is not None and not 0 <= args.numeric_page <= 48:
        parser.error('Numeric page exceeds the integer variable observer range')
    original = json.loads(args.frozen_inputs.read_text())
    files = original.get('files', original)

    def verify_originals():
        for path, expected in files.items():
            if digest(path) != expected:
                raise RuntimeError('Frozen original changed: ' + path)

    verify_originals()
    work = root / 'temp/reference-runs' / args.name
    work.mkdir(parents=True, exist_ok=False)
    stock = work / 'stock'
    reference = root / 'temp/mugen-1.1b1'
    source = legacy.prepare(reference, stock, fixture, version='1.1')
    (stock / 'chars/kfm/probe.cns').write_text(source)
    shutil.copyfile(fixture.with_suffix('.stage.def'), stock / 'stages/probe-stage.def')
    # Native finalizes mugen.cfg itself; retain its prepared bytes/hash separately.
    prepared_config = (stock / 'data/mugen.cfg').read_bytes()
    (work / 'prepared-mugen.cfg').write_bytes(prepared_config)
    manifest = json.loads((root / 'tests/testdata/reference-manifest.json').read_text())
    prepared = {str(stock / x['path']): digest(stock / x['path']) for x in manifest
                if Path(x['path']).suffix.lower() != '.cfg'}
    for relative in ['chars/kfm/probe.cns', 'chars/kfm/probe.air',
                     'chars/kfm/probe.cmd', 'stages/probe-stage.def']:
        prepared[str(stock / relative)] = digest(stock / relative)
    ledger = {'source_git': original.get('source_git'), 'original_input_map_sha256': digest(args.frozen_inputs),
              'files': prepared}
    (work / 'prepared-inputs.json').write_text(json.dumps(ledger, indent=2) + '\n')
    evidence = {'purpose': 'authored native frame/key observation; fixture defines measured scope',
                'sequence': sequence, 'sequence_sha256': digest(sequence_path),
                'source_git': original.get('source_git'),
                'original_input_map_sha256': digest(args.frozen_inputs),
                'recorder_sha256': digest(__file__), 'engine_sha256': digest(stock / 'mugen.exe'),
                'fixture_sha256': {suffix: digest(fixture.with_suffix(suffix)) for suffix in
                                   ['.cns', '.air', '.cmd', '.stage.def']},
                'prepared_input_map_sha256': digest(work / 'prepared-inputs.json'),
                'prepared_config_sha256': digest(work / 'prepared-mugen.cfg'),
                'events': [], 'captures': []}
    env = os.environ.copy()
    env.update(WINEARCH='win32', WINEPREFIX=str(root / 'temp/wine-reference'), WINEDEBUG='-all')
    log = (work / 'run.log').open('x')
    process = None
    started = time.monotonic()
    checks = []
    try:
        process = subprocess.Popen(['wine', 'mugen.exe', 'kfm', 'kfm', '-s', 'probe-stage', '-nojoy'],
                                   cwd=stock, env=env, stdout=log, stderr=log)
        time.sleep(args.warmup)
        if process.poll() is not None:
            raise RuntimeError('Reference exited before the calibration')
        windows = subprocess.check_output(['xdotool', 'search', '--onlyvisible', '--pid', str(process.pid)],
                                          text=True).split()
        if len(windows) != 1:
            raise RuntimeError('Expected exactly one visible window owned by the launched reference')
        window = windows[0]
        window_pid = int(subprocess.check_output(['xdotool', 'getwindowpid', window], text=True))
        if window_pid != process.pid:
            raise RuntimeError('Reference input window ownership changed')
        evidence['window'], evidence['pid'] = window, process.pid

        def key(name):
            for operation in ['keydown', 'keyup']:
                if process.poll() is not None:
                    raise RuntimeError('Reference exited before an owned key event')
                subprocess.run(['xdotool', operation, '--window', window, name], check=True,
                               stdout=log, stderr=log, timeout=5)
                evidence['events'].append({'operation': operation, 'key': name, 'window': window,
                                           'pid': process.pid, 'seconds': time.monotonic()-started})
                if operation == 'keydown':
                    time.sleep(.05)

        def capture(label, numeric):
            verify_originals()
            path = work / (label + '.png')
            if path.exists():
                raise RuntimeError('Capture path already exists')
            subprocess.run(['import', '-window', window, str(path)], check=True,
                           stdout=log, stderr=log, timeout=5)
            im = Image.open(path).convert('RGB')
            if im.size != (1280, 720):
                raise RuntimeError('Calibration window dimensions changed')
            item = {'label': label, 'path': path.name, 'sha256': digest(path),
                    'dimensions': list(im.size), 'world_region': [160, 140, 1120, 640],
                    'world_rgb_sha256': hashlib.sha256(im.crop((160, 140, 1120, 640)).tobytes()).hexdigest()}
            if args.numeric_page is not None and numeric:
                item['variables'] = legacy.read_clipboard(path, reference, args.numeric_page)
            evidence['captures'].append(item)
            (work / 'evidence.json').write_text(json.dumps(evidence, indent=2) + '\n')
            return item

        for index, action in enumerate(sequence):
            if 'key' in action:
                key(action['key'])
                time.sleep(.15)
            elif 'seek_game_time' in action:
                if args.numeric_page != 0 or not evidence['captures'] or 'variables' not in evidence['captures'][-1]:
                    raise RuntimeError('Seek requires an authored live GameTime in numeric field0')
                target = action['seek_game_time']
                current = evidence['captures'][-1]['variables'].get(0)
                if type(current) is not int or not 0 <= target-current <= 250:
                    raise RuntimeError('Seek requires at most250 forward calibrated steps')
                for _ in range(target-current):
                    key('Scroll_Lock')
                    time.sleep(.05)
                time.sleep(.15)
                item = capture('seek-g'+str(target), True)
                if item['variables'].get(0) != target:
                    raise RuntimeError('Authored live GameTime did not reach the exact seek target')
                checks.append({'seek_game_time': target, 'steps': target-current, 'exact': True})
            elif 'steps' in action:
                for _ in range(action['steps']):
                    key('Scroll_Lock')
                    time.sleep(.05)
                time.sleep(.15)
            else:
                item = capture(action['capture'], action['numeric'])
                item['sequence_index'] = index
                print(json.dumps({'capture': action['capture'], 'variables': item.get('variables'),
                                  'world_rgb_sha256': item['world_rgb_sha256']}), flush=True)
        evidence['all_sequence_actions_completed'] = True
        evidence['checks'] = checks
    except BaseException as error:
        evidence['error'] = str(error)
        evidence['checks'] = checks
        raise
    finally:
        if process is not None:
            subprocess.run(['wineserver', '-k'], env=env, stdout=log, stderr=log, check=False)
            try:
                process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
            evidence['cleanup_process_exit'] = process.returncode
        log.close()
        for path, expected in prepared.items():
            if digest(path) != expected:
                raise RuntimeError('Prepared native input changed: ' + path)
        verify_originals()
        native_log = stock / 'mugen.log'
        if native_log.exists():
            evidence['seed_log_sha256'] = digest(native_log)
            evidence['match_seeds'] = [int(n) for n in re.findall(r'Match RNG seed: (\d+)', native_log.read_text())]
        evidence['final_config_sha256'] = digest(stock / 'data/mugen.cfg')
        evidence['native_originals_unchanged'] = True
        (work / 'evidence.json').write_text(json.dumps(evidence, indent=2) + '\n')
    print(json.dumps({'name': args.name, 'checks': checks, 'captures': len(evidence['captures'])}), flush=True)


if __name__ == '__main__':
    main()
