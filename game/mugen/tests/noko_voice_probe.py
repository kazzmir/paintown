#!/usr/bin/env python3
"""Compare NoKO application PCM with immutable native tones and timing."""
import argparse
import hashlib
import json
from pathlib import Path
import re

from sound_probe import measure


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True)
    parser.add_argument('--go-run', required=True)
    parser.add_argument('--name', required=True)
    args = parser.parse_args()
    if not re.fullmatch('nokovoice-[a-z-]+', args.fixture) or any(
            not re.fullmatch('[a-z0-9_-]+', value) for value in [args.go_run, args.name]):
        parser.error('Use authored NoKO voice fixtures and simple run names')
    root = Path(__file__).resolve().parents[1]
    bank = root / 'tests/testdata/reference'
    expected = json.loads((bank / (args.fixture + '.json')).read_text())['cases']['default']
    native = root / expected['native_run']
    native_audio = root / expected['native_audio_run']
    if any(not p.resolve().is_relative_to(root / 'temp') for p in [native, native_audio]):
        raise RuntimeError('Native artifact path escaped the workspace')
    for ext, key in [('.cns', 'cns_sha256'), ('.air', 'air_sha256')]:
        if digest(bank / (args.fixture + ext)) != expected[key]:
            raise RuntimeError('Authored source changed: ' + ext)
    alias = expected['sound_bank_alias']
    if alias != 'ko-sound.extra/probe.snd' or digest(bank / alias) != expected['extra_sha256']['probe.snd']:
        raise RuntimeError('Canonical tone bank alias changed')
    original = json.loads((native / 'evidence.json').read_text())
    if original['captures'] != expected['captures'] or digest(native / 'stock/mugen.exe') != expected['engine_sha256']:
        raise RuntimeError('Native capture/executable evidence changed')
    for capture in original['captures']:
        if digest(native / capture['path']) != capture['sha256']:
            raise RuntimeError('Native numeric capture changed')
    recipe = json.loads((bank / 'nokovoice-native-config-finalization.json').read_text())['programs'][args.fixture]
    wanted = expected['audio']
    if digest(native_audio / 'capture.s16le') != wanted['pcm_sha256']:
        raise RuntimeError('Native PCM changed')
    if digest(native / 'stock/data/mugen.cfg') != recipe['final_sha256']:
        raise RuntimeError('Native configuration changed')
    run = root / 'temp/app-runs' / args.go_run
    evidence = json.loads((run / 'evidence.json').read_text())
    if evidence.get('error') or evidence['players'] != ['chars/kfm/kfm.def'] * 2 or evidence['stage'] != 'stages/kfm.def':
        raise RuntimeError('Application failed or selected another fixture')
    if digest(run / 'mugen-go') != evidence['binary_sha256'] or digest(run / 'stock/data/mugen.cfg') != evidence['config_sha256']:
        raise RuntimeError('Application executable/configuration changed')
    for stock in [native / 'stock', run / 'stock']:
        if digest(stock / 'chars/kfm/probe.snd') != recipe['bank_sha256']:
            raise RuntimeError('Prepared tone bank changed')
        if digest(stock / 'data/quiet-probe.snd') != recipe['quiet_hud_bank_sha256']:
            raise RuntimeError('Prepared announcement bank changed')
        for relative, value in recipe['quiet_fight_sha256'].items():
            if digest(stock / relative) != value:
                raise RuntimeError('Prepared announcement recipe changed')
    audio = evidence['audio']
    raw = run / audio['path']
    if audio['sample_rate'] != 44100 or audio['channels'] != 2 or digest(raw) != audio['sha256'] or raw.stat().st_size != audio['frames'] * 4:
        raise RuntimeError('Application PCM format/hash changed')
    segments = measure(raw, wanted['frequencies'], window_ms=wanted['analysis_window_ms'], step_ms=wanted['analysis_step_ms'])
    failures = []
    if len(segments) != len(wanted['segments']):
        failures.append('segment count: %d native %d' % (len(segments), len(wanted['segments'])))
    for index, (actual, reference) in enumerate(zip(segments, wanted['segments'])):
        for frequency, levels in reference['levels'].items():
            if any(abs(a-b) > wanted['amplitude_tolerance'] for a,b in zip(actual['levels'][frequency], levels)):
                failures.append('segment %d tone %s amplitude differs' % (index, frequency))
        relative_actual = actual['start'] - segments[0]['start']
        relative_native = reference['start'] - wanted['segments'][0]['start']
        if abs(relative_actual-relative_native) > wanted['time_tolerance']:
            failures.append('segment %d relative start differs' % index)
    report = {'fixture': args.fixture, 'go_run': args.go_run, 'binary_sha256': evidence['binary_sha256'],
              'pcm_sha256': audio['sha256'], 'segments': segments, 'native_segments': wanted['segments'],
              'amplitude_tolerance': wanted['amplitude_tolerance'], 'time_tolerance': wanted['time_tolerance'],
              'failures': failures}
    work = root / 'temp/noko-voice-runs' / args.name
    work.mkdir(parents=True, exist_ok=False)
    (work / 'measurements.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))
    raise SystemExit(int(bool(failures)))


if __name__ == '__main__':
    main()
