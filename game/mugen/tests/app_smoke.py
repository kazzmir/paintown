#!/usr/bin/env python3
"""Build/run the Go application in a disposable data copy; capture its own window.

Requires Go, DISPLAY, xdotool and ImageMagick import. No keyboard/pointer/focus
operations are performed. Captures are inspection artifacts, not parity tests.
"""
import argparse
import array
import hashlib
import json
import math
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
    parser.add_argument('--match', nargs=2, help='Quick-match character DEF paths in the disposable distribution')
    parser.add_argument('--stage', default='stages/kfm.def')
    parser.add_argument('--audio', action='store_true', help='Also record the task-owned private null sink, using a private ALSA configuration')
    args = parser.parse_args()
    if args.audio:
        server = os.environ.get('PULSE_SERVER', '')
        alsa = os.environ.get('ALSA_CONFIG_PATH', '')
        if (not server.startswith('unix:') or not Path(server[5:]).resolve().is_relative_to(project / 'temp') or
                not alsa or not Path(alsa).resolve().is_relative_to(project / 'temp')):
            parser.error('Audio smoke requires explicit private PULSE_SERVER and ALSA_CONFIG_PATH under mugen/temp/')
    if not re.fullmatch('[a-z0-9_-]+', args.name):
        parser.error('Use a simple lowercase run name')
    if not args.seconds or args.seconds != sorted(set(args.seconds)) or not 1 <= min(args.seconds) <= max(args.seconds) <= 60:
        parser.error('Capture times must be increasing unique seconds from 1 through 60')
    if Path(args.motif).is_absolute() or '..' in Path(args.motif).parts or not re.fullmatch('[a-zA-Z0-9_./-]+', args.motif):
        parser.error('Motif must be a relative path within the disposable distribution')
    for path in (args.match or []) + [args.stage]:
        if Path(path).is_absolute() or '..' in Path(path).parts or not re.fullmatch('[a-zA-Z0-9_./-]+', path):
            parser.error('Match paths must be relative paths within the disposable distribution')
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
    if args.match:
        metadata.update(players=args.match, stage=args.stage)
    with (work / 'run.log').open('w') as log:
        command = [str(binary), '--data-dir', str(stock)]
        if args.match:
            command += ['--quick-match', ','.join(args.match), '--stage', args.stage]
        process = subprocess.Popen(command, cwd=project.parents[1], env=env, stdout=log, stderr=log)
        started = time.monotonic()
        recorder, recording = None, None
        try:
            if args.audio:
                recording = (work / 'capture.s16le').open('wb')
                recorder = subprocess.Popen(['parec', '--device=mugen_reference.monitor', '--format=s16le',
                                             '--rate=44100', '--channels=2', '--latency-msec=20'],
                                            env=env, stdout=recording, stderr=log)
            for seconds in args.seconds:
                time.sleep(max(0, started + seconds - time.monotonic()))
                if process.poll() is not None:
                    raise RuntimeError('Application exited early: ' + str(process.returncode))
                if recorder is not None and recorder.poll() is not None:
                    raise RuntimeError('Private audio recorder exited early')
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
            if recorder is not None:
                recorder.terminate()
                recorder.wait(timeout=5)
            if recording is not None:
                recording.close()
                raw = work / 'capture.s16le'
                pcm = array.array('h', raw.read_bytes())
                metadata['audio'] = {'path': raw.name, 'sha256': digest(raw), 'sample_rate': 44100, 'channels': 2,
                                     'frames': len(pcm)//2, 'peak': max((abs(value) for value in pcm), default=0),
                                     'rms': round(math.sqrt(sum(value*value for value in pcm)/max(1,len(pcm))), 2)}
                if metadata['audio']['peak'] == 0:
                    metadata['error'] = 'No device PCM captured from the private sink'
            (work / 'evidence.json').write_text(json.dumps(metadata, indent=2) + '\n')
            if recording is not None and metadata['audio']['peak'] == 0:
                raise RuntimeError(metadata['error'])


if __name__ == '__main__':
    main()
