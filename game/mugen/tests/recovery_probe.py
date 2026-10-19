#!/usr/bin/env python3
"""Replay frozen recovery recipes on one owned beta1 window, without focus changes."""
import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time

from reference_probe import digest, page_source, prepare, read_clipboard


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True)
    parser.add_argument('--name', required=True)
    parser.add_argument('--record', action='store_true')
    args = parser.parse_args()
    if not all(re.fullmatch(r'[a-z0-9-]+', x) for x in [args.fixture, args.name]):
        parser.error('Use simple fixture/run names')
    root = Path(__file__).resolve().parents[1]
    fixture = root/'tests/testdata/reference'/args.fixture
    recipe_path = fixture.with_suffix('.input.json')
    recipe = json.loads(recipe_path.read_text())
    reference = root/'temp/mugen-1.1b1'
    work = root/'temp/recovery-input-runs'/args.name
    work.mkdir(parents=True, exist_ok=False)
    run = work/'stock'
    source = prepare(reference, run, fixture, version='1.1', fixture_constants=True)
    shutil.copyfile(fixture.with_suffix('.stage.def'), run/'stages/probe-stage.def')
    (run/'chars/kfm/probe.cns').write_text(source)
    commands = run/'chars/kfm/probe.cmd'
    commands.write_text(page_source(commands.read_text(), 0, 'fvar', 0))
    prepared = {str(p.relative_to(run)): digest(p) for p in [
        run/'chars/kfm/probe.cns', commands, run/'chars/kfm/probe.air',
        run/'chars/kfm/kfm.def', run/'data/mugen.cfg', run/'stages/probe-stage.def']}
    (work/'prepared-inputs.json').write_text(json.dumps(prepared, indent=2)+'\n')
    authored = {ext: digest(fixture.with_suffix(ext)) for ext in ['.cns','.cmd','.air','.stage.def','.input.json']}
    prefix = root/'temp/wine-reference'
    env = dict(os.environ, WINEARCH='win32', WINEPREFIX=str(prefix), WINEDEBUG='-all')
    output = work/'recovery.png'
    events = []
    held = set()
    with (work/'native.log').open('w') as log:
        process = subprocess.Popen(['wine','mugen.exe','kfm','kfm','-s','probe-stage','-nojoy'], cwd=run, env=env, stdout=log, stderr=log)
        window = None
        def key(action, keys):
            subprocess.run(['xdotool', action, '--window', window, *keys], check=True, stdout=log, stderr=log)
            events.append({'pid': process.pid, 'window': window, 'action': action, 'keys': keys})
        try:
            time.sleep(recipe['settle_seconds'])
            if process.poll() is not None:
                raise RuntimeError('Owned beta1 process exited early')
            windows = subprocess.run(['xdotool','search','--onlyvisible','--pid',str(process.pid)], capture_output=True, text=True, check=False).stdout.split()
            if len(windows) != 1:
                raise RuntimeError('Expected one visible owned beta1 window')
            window = windows[0]
            for item in recipe['events']:
                keys = item['keys']
                if not keys or any(not re.fullmatch('[A-Za-z0-9_]+', k) for k in keys):
                    raise RuntimeError('Invalid recipe key')
                key('keydown',keys); held.update(keys)
                time.sleep(item['hold_seconds'])
                key('keyup',keys); held.difference_update(keys)
                time.sleep(item['gap_seconds'])
            time.sleep(recipe['tail_seconds'])
            subprocess.run(['import','-window',window,str(output)],check=True,stdout=log,stderr=log,timeout=5)
            shutil.copyfile(run/'mugen.log', work/'mugen.log')
        finally:
            if window and held:
                key('keyup', sorted(held))
            subprocess.run(['wineserver','-k'],env=env,stdout=log,stderr=log,check=False)
            try:
                process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                process.kill(); process.wait()
    variables = read_clipboard(output, reference, 0)
    evidence = {'engine_sha256': digest(reference/'mugen.exe'), 'cns_sha256': authored['.cns'],
                'cmd_sha256': authored['.cmd'], 'air_sha256': authored['.air'],
                'stage_fixture_sha256': authored['.stage.def'], 'recipe_sha256': authored['.input.json'],
                'probe_sha256': digest(Path(__file__)), 'reference_probe_sha256': digest(root/'tests/reference_probe.py'),
                'fixture_constants': True, 'bank': 'fvar', 'mugenversion': '1.1',
                'stage': 'probe-stage', 'fixture_stage': True, 'owned_pid': process.pid,
                'owned_window': window, 'events': events,
                'captures': [{'page': 0, 'path': output.name, 'sha256': digest(output), 'variables': variables}]}
    (work/'evidence.json').write_text(json.dumps(evidence, indent=2)+'\n')
    if not args.record:
        expected = json.loads(fixture.with_suffix('.json').read_text())
        for k in [k for k in evidence if k.endswith('_sha256')]:
            if expected[k] != evidence[k]:
                raise RuntimeError('Changed input/probe: '+k)
        want = expected['captures'][0]['variables']
        if {str(k): v for k,v in variables.items()} != want:
            raise RuntimeError('Native recovery observations changed')
    print(args.fixture, variables, flush=True)


if __name__ == '__main__':
    main()
