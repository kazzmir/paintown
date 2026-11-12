#!/usr/bin/env python3
"""Audit frozen M4ca/M4cb source, native clipboards and owned input provenance."""
import argparse
import json
from pathlib import Path
import re

from reference_probe import digest, page_source, read_clipboard


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    reference = root/'temp/mugen-1.1b1'
    corpus = root/'tests/testdata/reference'
    native_hash = 'd5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e'
    assert digest(reference/'mugen.exe') == native_hash
    records = []
    for folder in ['reference-runs', 'recovery-input-runs']:
        for path in (root/'temp'/folder).glob('*/evidence.json'):
            if path.parent.name.startswith(('m4ca-', 'm4cb-')):
                records.append((path, json.loads(path.read_text())))
    results = []
    for pattern in ['metadata-*.json','recoverytimer-*.json','recoverymash-*.json','recoveryroute-*.json']:
        for path in sorted(corpus.glob(pattern)):
            if path.name.endswith('.input.json'):
                continue
            expected = json.loads(path.read_text())
            fixture = path.with_suffix('')
            keys = {'.cns':'cns_sha256','.cmd':'cmd_sha256','.air':'air_sha256','.stage.def':'stage_fixture_sha256'}
            if expected.get('recipe_sha256'):
                keys['.input.json'] = 'recipe_sha256'
            for suffix, key in keys.items():
                assert digest(fixture.with_suffix(suffix)) == expected[key], (path,key)
            def matches(record):
                if any(record.get(key) != expected.get(key) for key in keys.values()):
                    return False
                if expected.get('expected_load_error'):
                    return expected['expected_load_error'] in record.get('load_error','')
                return ([x['variables'] for x in record.get('captures',[])] ==
                        [x['variables'] for x in expected['captures']])
            candidates = [(p,e) for p,e in records if matches(e)]
            assert candidates, ('No matching native evidence',path)
            record_path, evidence = candidates[-1]
            work, run = record_path.parent, record_path.parent/'stock'
            assert evidence['engine_sha256'] == digest(run/'mugen.exe') == native_hash
            constants = '' if expected.get('fixture_constants') else re.split(r'(?im)^\s*\[statedef', (reference/'chars/kfm/kfm.cns').read_text())[0]
            assert (run/'chars/kfm/probe.cns').read_text() == constants+'\n'+fixture.with_suffix('.cns').read_text(), path
            command_prefix = re.split(r'(?im)^\s*\[statedef',(reference/'chars/kfm/kfm.cmd').read_text())[0]
            delay = 0 if expected.get('recipe_sha256') else 30
            commands = command_prefix+'\n'+fixture.with_suffix('.cmd').read_text()
            assert (run/'chars/kfm/probe.cmd').read_text() == page_source(commands,0,'fvar',delay), path
            assert digest(run/'chars/kfm/probe.air') == expected['air_sha256']
            assert digest(run/'stages/probe-stage.def') == expected['stage_fixture_sha256']
            config = re.sub(r'(?im)^Debug\s*=\s*0\s*$', 'Debug = 1',(reference/'data/mugen.cfg').read_text())
            actual = (run/'data/mugen.cfg').read_text()
            # The reference persists this single startup flag, including on rejection.
            actual = re.sub(r'(?im)^FirstRun[ \t]*=[ \t]*0[ \t]*$', 'FirstRun = 1',actual)
            assert actual == config, ('Unexpected native config mutation',record_path)
            if expected.get('recipe_sha256'):
                assert evidence['probe_sha256'] == digest(root/'tests/recovery_probe.py')
                assert evidence['reference_probe_sha256'] == digest(root/'tests/reference_probe.py')
                held = set()
                recipe = json.loads(fixture.with_suffix('.input.json').read_text())
                assert len(evidence['events']) == 2*len(recipe['events'])
                for event in evidence['events']:
                    assert event['pid'] == evidence['owned_pid'] and event['window'] == evidence['owned_window']
                    keys = set(event['keys'])
                    if event['action'] == 'keydown':
                        assert not held & keys
                        held |= keys
                    else:
                        assert event['action'] == 'keyup' and keys <= held
                        held -= keys
                assert not held
            for capture in evidence['captures']:
                png = work/capture['path']
                assert digest(png) == capture['sha256']
                assert {str(k):v for k,v in read_clipboard(png,reference,capture['page']).items()} == capture['variables']
            results.append({'fixture':path.name,'record':str(record_path.relative_to(root)),
                            'values':sum(len(x['variables']) for x in evidence['captures']),
                            'rejected':bool(evidence.get('load_error'))})
    output = {'programs':len(results),'values':sum(x['values'] for x in results),
              'rejections':sum(x['rejected'] for x in results),'checks':results}
    if args.output:
        args.output.write_text(json.dumps(output,indent=2)+'\n')
    print(json.dumps({key:output[key] for key in ['programs','values','rejections']}))


if __name__ == '__main__':
    main()
