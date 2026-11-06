#!/usr/bin/env python3
"""Observe native debug Pause/Scroll Lock without injecting clipboard controllers.

The authored fixture owns CNS/CMD/AIR/stage. Numeric decoding is opt-in and only
reads pages already authored in that fixture. Original recorders stay unchanged.
This first use calibrates native runtime controls, not controller compatibility.
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
    parser.add_argument('--steps', type=int, nargs='+', default=[1, 2, 3])
    parser.add_argument('--numeric-page', type=int)
    parser.add_argument('--warmup', type=float, default=7)
    args = parser.parse_args()
    fixture = args.fixture.resolve()
    if not fixture.is_relative_to(root) or not re.fullmatch('[a-z0-9_-]+', fixture.name):
        parser.error('Fixture must be an authored stem under game/mugen')
    if not re.fullmatch('[a-z0-9_-]+', args.name) or not 3 <= args.warmup <= 10:
        parser.error('Use a fresh simple run name and a warmup from3 to10 seconds')
    if not args.steps or any(not 1 <= count <= 10 for count in args.steps) or sum(args.steps) > 30:
        parser.error('Use bounded positive frame-step counts with total at most30')
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
    evidence = {'purpose': 'native debug-frame control calibration; no new controller coverage',
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

        def capture(label):
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
            if args.numeric_page is not None:
                item['variables'] = legacy.read_clipboard(path, reference, args.numeric_page)
            evidence['captures'].append(item)
            (work / 'evidence.json').write_text(json.dumps(evidence, indent=2) + '\n')
            return item

        key('Pause')
        time.sleep(.15)
        previous = capture('paused-0')
        time.sleep(.2)
        still = capture('paused-1')
        checks.append({'check': 'paused_world_stable', 'pass': previous['world_rgb_sha256'] == still['world_rgb_sha256']})
        if args.numeric_page is not None:
            checks.append({'check': 'paused_numbers_stable', 'pass': previous['variables'] == still['variables']})
        previous = still
        for index, count in enumerate(args.steps):
            for _ in range(count):
                key('Scroll_Lock')
                time.sleep(.05)
            time.sleep(.15)
            current = capture('step-' + str(index))
            if args.numeric_page == 0:
                for field in [0, 1, 2]:
                    delta = current['variables'][field] - previous['variables'][field]
                    checks.append({'check': 'step_'+str(index)+'_field_'+str(field),
                                   'wanted_delta': count, 'actual_delta': delta, 'pass': delta == count})
            evidence.setdefault('world_step_observations', []).append(
                {'steps': count, 'same_world': current['world_rgb_sha256'] == previous['world_rgb_sha256']})
            previous = current
        key('Pause')
        time.sleep(.2)
        resumed = capture('resumed')
        if args.numeric_page == 0:
            delta = resumed['variables'][0] - previous['variables'][0]
            checks.append({'check': 'resumed_game_time_advances', 'actual_delta': delta, 'pass': delta >= 2})
        evidence['checks'] = checks
        if not all(x['pass'] for x in checks):
            raise RuntimeError('Native frame-control calibration checks failed; all captures retained')
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
