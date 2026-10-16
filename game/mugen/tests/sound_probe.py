#!/usr/bin/env python3
"""Capture authored tones from the reference through a private null audio sink.

Requires an already running task-owned PulseAudio server under mugen/temp/.
Records only that server's mugen_reference.monitor, never the desktop or a mic.
The numeric/window portion uses reference_probe.py and its process ownership.
"""
import argparse
import array
import cmath
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shutil
import statistics
import subprocess
import sys


def measure(path, frequencies, rate=44100):
    pcm = array.array('h', path.read_bytes())
    if sys.byteorder != 'little':
        pcm.byteswap()
    size, step = rate // 5, rate // 10
    window = [.5 - .5 * math.cos(2 * math.pi * i / (size - 1)) for i in range(size)]
    weight = sum(window)
    oscillators = [[cmath.exp(-2j * math.pi * frequency * i / rate) * window[i]
                    for i in range(size)] for frequency in frequencies]
    groups = []
    started = False
    for offset in range(0, len(pcm) // 2 - size + 1, step):
        channels = [pcm[offset * 2 + channel:(offset + size) * 2:2] for channel in [0, 1]]
        rms = [math.sqrt(sum(value * value for value in channel) / size) for channel in channels]
        levels = [[2 * abs(sum(value * oscillator for value, oscillator in zip(channel, oscillators[tone]))) / weight
                   for channel in channels] for tone in range(len(frequencies))]
        power = sum(value * value for pair in levels for value in pair) / 2
        total = sum(value * value for value in rms)
        silence = max(rms) < 2
        if not silence and (max(value for pair in levels for value in pair) < 50 or power / max(1, total) < .96):
            continue
        if silence and not started:
            continue
        started = True
        flat = [0 if value < 5 else value for pair in levels for value in pair]
        if groups and max(abs(a - b) for a, b in zip(flat, groups[-1]['last'])) < 12:
            groups[-1]['values'].append(flat)
            groups[-1]['end'] = (offset + size) / rate
            groups[-1]['last'] = flat
        else:
            groups.append({'start': offset / rate, 'end': (offset + size) / rate,
                           'values': [flat], 'last': flat})
    result = []
    for group in groups:
        if len(group['values']) < 3:
            continue
        medians = [round(statistics.median(values), 1) for values in zip(*group['values'])]
        levels = {f'{frequency:g}': medians[tone * 2:tone * 2 + 2]
                  for tone, frequency in enumerate(frequencies)}
        if result and all(abs(a-b) < 12 for frequency in levels
                          for a,b in zip(levels[frequency], result[-1]['levels'][frequency])):
            result[-1]['end'] = round(group['end'], 3)
        else:
            result.append({'start': round(group['start'], 3), 'end': round(group['end'], 3), 'levels': levels})
    return result


def main():
    project = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True)
    parser.add_argument('--name', required=True)
    parser.add_argument('--record', action='store_true')
    parser.add_argument('--case', default='default')
    parser.add_argument('--mugenversion',choices=['1.0','1.1'],default='1.1')
    parser.add_argument('--seconds', type=float, default=12)
    parser.add_argument('--server', default=os.environ.get('PULSE_SERVER', ''))
    parser.add_argument('--frequencies', type=float, nargs='+')
    parser.add_argument('--hud-tone', action='store_true', help='Replace only disposable fight announcements with the authored one-second tone')
    args = parser.parse_args()
    expected_case = None
    if not args.record:
        expected_case = json.loads((project/'tests/testdata/reference'/(args.fixture+'.json')).read_text())['cases'][args.case]
        args.hud_tone = expected_case['audio'].get('hud_tone', False)
    # The Pulse client can otherwise silently select the host's default server.
    if not args.server.startswith('unix:') or not Path(args.server[5:]).resolve().is_relative_to(project / 'temp'):
        parser.error('An explicit task-owned Unix audio server under mugen/temp/ is required')
    if not all(value and all(c in 'abcdefghijklmnopqrstuvwxyz0123456789_-' for c in value)
               for value in [args.fixture, args.name, args.case]):
        parser.error('Use simple fixture, case and run names')
    if args.frequencies is None:
        args.frequencies = [441,882,1323] if args.record else expected_case['audio']['frequencies']
    if not 3 <= args.seconds <= 30 or not args.frequencies or any(not 1 <= value <= 18000 for value in args.frequencies):
        parser.error('Capture delay is 3..30 seconds; tone frequencies are 1..18000 Hz')
    work = project / 'temp/audio-runs' / args.name
    work.mkdir(parents=True, exist_ok=False)
    raw = work / 'capture.s16le'
    env = os.environ.copy()
    env['PULSE_SERVER'] = args.server
    command = [sys.executable, str(project / 'tests/reference_probe.py'), '--fixture', args.fixture,
               '--name', args.name, '--seconds', str(args.seconds)]
    hud_hashes = {}
    if args.hud_tone:
        # Patch only a task-owned distribution copy. The immutable tone bank is
        # copied into the character by reference_probe's ordinary extra loader.
        reference = work / 'reference'
        shutil.copytree(project / 'temp/mugen-1.1b1', reference)
        for fight in sorted(reference.glob('data/**/fight.def')):
            definition = fight.read_text(encoding='utf-8-sig')
            definition = re.sub(r'(?im)^\s*snd\s*=.*$', 'snd = chars/kfm/probe.snd', definition)
            definition = re.sub(r'(?im)^\s*(round[1-9]|fight|KO|DKO|TO)\.snd\s*=.*$',
                                lambda match: match.group(1) + '.snd = ' + ('4,0' if match.group(1).lower() == 'round1' else '-1,0'), definition)
            fight.write_text(definition)
            hud_hashes[str(fight.relative_to(reference))] = hashlib.sha256(fight.read_bytes()).hexdigest()
        if not hud_hashes:
            raise RuntimeError('No fight definitions available for the HUD tone recipe')
        command += ['--reference', str(reference)]
    if args.record:
        command += ['--record', '--bank', 'fvar', '--mugenversion', args.mugenversion, '--file', 'sound=probe.snd',
                    '--clipboard-in-cmd', '--pages', '0']
    else:
        command += ['--case', args.case]
    with raw.open('wb') as output, (work / 'recorder.log').open('w') as log:
        recorder = subprocess.Popen(['parec', '--device=mugen_reference.monitor', '--format=s16le',
                                     '--rate=44100', '--channels=2', '--latency-msec=20'],
                                    env=env, stdout=output, stderr=log)
        try:
            subprocess.run(command, env=env, check=True)
            if recorder.poll() is not None:
                raise RuntimeError('Private audio recorder exited early; inspect recorder.log')
        finally:
            recorder.terminate()
            recorder.wait(timeout=5)
    native = json.loads((project / 'temp/reference-runs' / args.name / 'evidence.json').read_text())
    if native.get('load_error'):
        raise RuntimeError('Audio fixture did not load; inspect reference evidence')
    segments = measure(raw, args.frequencies)
    if not segments or not any(any(value > 50 for pair in item['levels'].values() for value in pair) for item in segments):
        raise RuntimeError('No stable authored tones captured from private sink')
    audio = {'sample_rate': 44100, 'channels': 2, 'frequencies': args.frequencies,
             'pcm_sha256': hashlib.sha256(raw.read_bytes()).hexdigest(), 'segments': segments,
             'amplitude_tolerance': 8, 'time_tolerance': .35}
    configuration = (project / 'temp/reference-runs' / args.name / 'stock/data/mugen.cfg').read_bytes()
    text = configuration.decode('utf-8-sig')
    audio['config_sha256'] = hashlib.sha256(configuration).hexdigest()
    audio['settings'] = {key: int(re.search(r'(?im)^\s*' + key + r'\s*=\s*([0-9]+)', text).group(1))
                         for key in ['SampleRate', 'WavChannels', 'MasterVolume', 'WavVolume', 'StereoEffects', 'PanningWidth', 'ReverseStereo', 'GameWidth', 'GameHeight']}
    if args.hud_tone:
        audio['hud_tone'], audio['hud_fight_sha256'] = True, hud_hashes
    (work / 'measurements.json').write_text(json.dumps(audio, indent=2) + '\n')
    path = project / 'tests/testdata/reference' / (args.fixture + '.json')
    if args.record:
        catalogue = json.loads(path.read_text()) if path.exists() else {'cases': {}}
        previous = catalogue['cases'].get(args.case,{})
        if all(previous.get(key)==native.get(key) for key in ['cns_sha256','air_sha256','extra_sha256']):
            for key in ['checks', 'missing_resources']:
                if key in previous.get('audio', {}):
                    audio[key] = previous['audio'][key]
        native['audio'] = audio
        catalogue['cases'][args.case] = native
        path.write_text(json.dumps(catalogue, indent=2) + '\n')
    else:
        expected = json.loads(path.read_text())['cases'][args.case]['audio']
        if expected.get('settings') != audio['settings']:
            raise RuntimeError('Native sound configuration differs from pinned evidence')
        if expected.get('hud_fight_sha256', {}) != hud_hashes:
            raise RuntimeError('Native HUD tone recipe differs from pinned evidence')
        if expected['frequencies'] != args.frequencies or len(expected['segments']) != len(segments):
            raise RuntimeError('Native audio segment/frequency count differs from pinned evidence')
        for actual, wanted in zip(segments, expected['segments']):
            for frequency in wanted['levels']:
                if any(abs(a - b) > expected['amplitude_tolerance'] for a, b in zip(actual['levels'][frequency], wanted['levels'][frequency])):
                    raise RuntimeError('Native audio amplitude differs from pinned evidence')
            if abs((actual['start']-segments[0]['start'])-(wanted['start']-expected['segments'][0]['start'])) > expected.get('time_tolerance',.35):
                raise RuntimeError('Relative native audio cue timing differs from pinned evidence')
    print(('Recorded' if args.record else 'Verified') + f' {len(segments)} stable native audio segments in {work}')


if __name__ == '__main__':
    main()
