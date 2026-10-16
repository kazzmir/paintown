#!/usr/bin/env python3
"""Build/run the Go application in a disposable data copy; capture its own window.

Requires Go, DISPLAY, xdotool and ImageMagick import. No keyboard/pointer/focus
operations are performed. Captures are inspection artifacts, not parity tests.
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


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    project = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--name', required=True, help='Fresh run directory name')
    parser.add_argument('--reference', type=Path, default=project / 'temp/mugen-1.1b1')
    parser.add_argument('--motif', default='data/kfm/system.def')
    parser.add_argument('--seconds', type=int, nargs='+', default=[2, 6, 16])
    args = parser.parse_args()
    if not re.fullmatch('[a-z0-9_-]+', args.name):
        parser.error('Use a simple lowercase run name')
    if not args.seconds or args.seconds != sorted(set(args.seconds)) or not 1 <= min(args.seconds) <= max(args.seconds) <= 60:
        parser.error('Capture times must be increasing unique seconds from 1 through 60')
    if Path(args.motif).is_absolute() or '..' in Path(args.motif).parts or not re.fullmatch('[a-zA-Z0-9_./-]+', args.motif):
        parser.error('Motif must be a relative path within the disposable distribution')
    work = project / 'temp/app-runs' / args.name
    work.mkdir(parents=True, exist_ok=False)
    stock = work / 'stock'
    shutil.copytree(args.reference, stock)
    if not (stock / args.motif).is_file():
        parser.error('Requested motif is absent from the supplied distribution')
    config = stock / 'data/mugen.cfg'
    source = config.read_text()
    for key, value in [('motif', args.motif), ('width', '320'), ('height', '240')]:
        source = re.sub(r'(?im)^\s*' + key + r'\s*=.*$', lambda match: key + ' = ' + value, source)
    config.write_text(source)
    # First readable override wins: exclude ambient user/executable settings.
    (stock / 'mugen.yaml').write_text('{}\n')
    env = os.environ.copy()
    for key, suffix in [('GOCACHE', 'go-build'), ('GOTMPDIR', 'tmp'), ('TMPDIR', 'tmp')]:
        path = project / 'temp' / suffix
        path.mkdir(parents=True, exist_ok=True)
        env[key] = str(path)
    env['GOFLAGS'] = '-modfile=' + str(project / 'mugen.mod') + ' -mod=readonly'
    env['GOPROXY'] = 'off'
    binary = work / 'mugen-go'
    subprocess.run(['go', 'build', '-o', str(binary), './game/mugen/system'], cwd=project.parents[1], env=env, check=True)
    commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=project, text=True).strip()
    dirty = bool(subprocess.check_output(['git', 'status', '--porcelain', '--', 'game/mugen'], cwd=project.parents[1], text=True).strip())
    metadata = {'commit': commit, 'dirty': dirty, 'binary_sha256': digest(binary), 'config_sha256': digest(config),
                'motif': args.motif, 'captures': []}
    with (work / 'run.log').open('w') as log:
        process = subprocess.Popen([str(binary), '--data-dir', str(stock)], cwd=project.parents[1], env=env, stdout=log, stderr=log)
        started = time.monotonic()
        try:
            for seconds in args.seconds:
                time.sleep(max(0, started + seconds - time.monotonic()))
                if process.poll() is not None:
                    raise RuntimeError('Application exited early: ' + str(process.returncode))
                windows = subprocess.run(['xdotool', 'search', '--onlyvisible', '--pid', str(process.pid)],
                                         text=True, capture_output=True, check=False).stdout.split()
                if len(windows) != 1:
                    raise RuntimeError('Expected exactly one visible window belonging to the spawned application')
                output = work / ('at-' + str(seconds) + 's.png')
                subprocess.run(['import', '-window', windows[0], str(output)], check=True, timeout=5, stdout=log, stderr=log)
                metadata['captures'].append({'seconds': seconds, 'path': output.name, 'sha256': digest(output)})
                print('Captured', output, flush=True)
        except Exception as error:
            metadata['error'] = str(error)
            raise
        finally:
            process.terminate()
            try:
                process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
            (work / 'evidence.json').write_text(json.dumps(metadata, indent=2) + '\n')


if __name__ == '__main__':
    main()
