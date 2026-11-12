#!/usr/bin/env python3
"""Consolidated scenario suites. Completed suites stay out of default runs."""
import argparse
from collections import deque
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[1]
REPO = ROOT.parents[1]
CATALOG = ROOT / 'docs/STATE-MACHINE-FEATURES.json'


def fingerprint(segment):
    paths = {ROOT / 'mugen.mod', ROOT / 'mugen.sum'}
    for directory in segment['source_dirs']:
        paths.update(p for p in (ROOT / directory).glob('*.go') if not p.name.endswith('_test.go'))
    paths.update(ROOT / row['file'] for row in segment['tests'])
    digest = hashlib.sha256()
    for p in sorted(paths):
        digest.update(str(p.relative_to(ROOT)).encode() + b'\0')
        digest.update(p.read_bytes())
    return digest.hexdigest()


def statuses(catalog):
    result = []
    for entry in catalog['segments']:
        entry = dict(entry)
        entry['current_fingerprint'] = fingerprint(entry)
        entry['complete'] = (entry.get('completed', False) and
                             entry.get('passed_source') == entry['current_fingerprint'])
        entry['reason'] = ('completed' if entry['complete'] else
                           'covered source changed' if entry.get('completed') else
                           entry.get('reason', 'pending'))
        result.append(entry)
    return result


def validate_selection(segment):
    for row in segment['tests']:
        text = (ROOT / row['file']).read_text()
        if not re.search(r'^func ' + re.escape(row['name']) + r'\(t \*testing.T\)', text, re.M):
            raise ValueError('Segment scenario is missing: ' + row['file'] + ':' + row['name'])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--segment', choices=['state-flow', 'combat', 'entities', 'effects-pause', 'match-flow'])
    parser.add_argument('--list', action='store_true', help='List segment contracts and completion')
    parser.add_argument('--plan', action='store_true', help='Show selected scenarios without running Go')
    parser.add_argument('--reopen', choices=['state-flow', 'combat', 'entities', 'effects-pause', 'match-flow'])
    parser.add_argument('--reason', default='explicitly reopened')
    args, extra = parser.parse_known_args()
    catalog = json.loads(CATALOG.read_text())
    if args.reopen:
        for entry in catalog['segments']:
            if entry['id'] == args.reopen:
                entry['completed'] = False
                entry['reason'] = args.reason
        CATALOG.write_text(json.dumps(catalog, indent=2) + '\n')
        subprocess.run([sys.executable, str(ROOT / 'tools/state_coverage.py'), '--write'], check=True)
        return 0
    rows = statuses(catalog)
    for entry in rows:
        validate_selection(entry)
    if args.list:
        for entry in rows:
            print(f"[{'x' if entry['complete'] else ' '}] {entry['id']}: {entry['contract']} ({entry['reason']})")
        return 0
    selected = [x for x in rows if x['id'] == args.segment] if args.segment else [x for x in rows if not x['complete']]
    if not selected:
        print('All segment scenarios are complete for the current source. No completed tests rerun.')
        return 0
    initial_fingerprints = {entry['id']: entry['current_fingerprint'] for entry in selected}
    packages = sorted({'./game/mugen/' + str(Path(row['file']).parent)
                       for entry in selected for row in entry['tests']})
    names = sorted({row['name'] for entry in selected for row in entry['tests']})
    # A single serial Go invocation runs the selected connected scenarios. It
    # reuses stock resources in place and creates no native copies/captures.
    command = ['go', 'test', '-json', '-modfile=' + str(ROOT / 'mugen.mod'), '-mod=readonly',
               '-p', '1', '-count=1', '-timeout=10m', '-run', '^(' + '|'.join(names) + ')$',
               *packages, *extra]
    print('Segments:', ', '.join(x['id'] for x in selected), flush=True)
    if args.plan:
        for entry in selected:
            for row in entry['tests']:
                print(entry['id'] + ': ' + row['file'] + ':' + row['name'])
        return 0
    (ROOT / 'temp/_tmp').mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    with tempfile.TemporaryDirectory(prefix='segments-', dir=ROOT / 'temp/_tmp') as scratch:
        env['GOTMPDIR'] = scratch
        env['TMPDIR'] = scratch
        passed = set()
        tails = {}
        process = subprocess.Popen(command, cwd=REPO, env=env, stdout=subprocess.PIPE, text=True)
        for line in process.stdout:
            try:
                event = json.loads(line)
            except ValueError:
                print(line, end='')
                continue
            if event.get('Action') == 'pass' and event.get('Test') in names:
                package = event.get('Package', '').split('/game/mugen/', 1)[-1]
                passed.add((package, event['Test']))
            if event.get('Action') in {'pass', 'fail'} and not event.get('Test'):
                print(event.get('Package', '') + ': ' + event['Action'], flush=True)
            elif event.get('Action') == 'output':
                output = event.get('Output', '')
                key = (event.get('Package', ''), event.get('Test', ''))
                if event.get('Test'):
                    tails.setdefault(key, deque(maxlen=16)).append(output)
                elif not output.startswith(('PASS', 'FAIL', 'ok\t')):
                    print(output, end='', flush=True)
            elif event.get('Action') == 'fail' and event.get('Test'):
                key = (event.get('Package', ''), event['Test'])
                print('Failed scenario: ' + event['Test'], flush=True)
                for output in tails.get(key, []):
                    if not output.startswith(('=== RUN', '=== PAUSE', '=== CONT')):
                        print(output, end='', flush=True)
            elif event.get('Action') == 'build-output':
                print(event.get('Output', ''), end='', flush=True)
        code = process.wait()
    selected_ids = {entry['id'] for entry in selected}
    for entry in catalog['segments']:
        if entry['id'] not in selected_ids:
            continue
        complete = (all((str(Path(row['file']).parent), row['name']) in passed for row in entry['tests'])
                    and fingerprint(entry) == initial_fingerprints[entry['id']])
        entry['completed'] = complete
        entry['passed_source'] = fingerprint(entry) if complete else ''
        entry['validation'] = 'consolidated segment run' if complete else 'segment run failed or incomplete'
    if not all(entry['completed'] for entry in catalog['segments'] if entry['id'] in selected_ids):
        code = code or 1
    CATALOG.write_text(json.dumps(catalog, indent=2) + '\n')
    subprocess.run([sys.executable, str(ROOT / 'tools/state_coverage.py'), '--write'], check=True)
    return code


if __name__ == '__main__':
    sys.exit(main())
