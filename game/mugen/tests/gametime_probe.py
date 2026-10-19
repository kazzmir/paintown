#!/usr/bin/env python3
"""Observe one native process across two Versus entries and an intervening menu."""
import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time

from reference_probe import digest, prepare, read_clipboard
from round_probe import idle


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--name', required=True)
    parser.add_argument('--record', action='store_true')
    args = parser.parse_args()
    if not re.fullmatch('[a-z0-9-]+', args.name):
        parser.error('Use a simple fresh run name')
    project = Path(__file__).resolve().parents[1]
    reference = project/'temp/mugen-1.1b1'
    fixture = project/'tests/testdata/reference/engine-gametime'
    recipe = json.loads(fixture.with_suffix('.clock.json').read_text())
    idle()
    work = project/'temp/reference-runs'/args.name
    work.mkdir(parents=True, exist_ok=False)
    run = work/'stock'
    source = prepare(reference, run, fixture, version='1.1', fixture_constants=True)
    (run/'chars/kfm/probe.cns').write_text(source)
    select = run/'data/select.def'
    select.write_text('[Characters]\nkfm, stages/kfm.def\n[ExtraStages]\nstages/kfm.def\n[Options]\narcade.maxmatches=1\nteam.maxmatches=1\n')
    evidence = {'engine_sha256': digest(reference/'mugen.exe'),
                'fixture_sha256': {ext: digest(fixture.with_suffix(ext)) for ext in ['.cns','.cmd','.air','.clock.json']},
                'probe_sha256': digest(Path(__file__)), 'select_sha256': digest(select),
                'prepared_sha256': {p: digest(run/p) for p in ['chars/kfm/kfm.def','chars/kfm/probe.cns','chars/kfm/probe.cmd','chars/kfm/probe.air','data/mugen.cfg']},
                'events': [], 'captures': []}
    (work/'prepared.json').write_text(json.dumps(evidence, indent=2)+'\n')
    env = os.environ.copy()
    env.update(WINEARCH='win32', WINEPREFIX=str(project/'temp/wine-reference'), WINEDEBUG='-all')
    with (work/'run.log').open('w') as log:
        process = subprocess.Popen(['wine','mugen.exe','-nojoy'], cwd=run, env=env, stdout=log, stderr=log)
        try:
            time.sleep(4)
            windows = subprocess.check_output(['xdotool','search','--onlyvisible','--pid',str(process.pid)], text=True).split()
            if len(windows) != 1:
                raise RuntimeError('Expected exactly one owned native window')
            window = windows[0]
            for phase in ['entry','reentry']:
                for index, action in enumerate(recipe[phase]):
                    png = work/(phase+'-before-'+str(index)+'.png')
                    subprocess.run(['import','-window',window,str(png)], check=True, stdout=log, stderr=log, timeout=5)
                    subprocess.run(['xdotool','key','--window',window,'--delay','120',*action['keys']], check=True, stdout=log, stderr=log)
                    evidence['events'].append({'phase':phase,'index':index,'keys':action['keys'],'pid':process.pid,'window':window,'before_sha256':digest(png)})
                    time.sleep(action['wait'])
                png = work/(phase+'.png')
                subprocess.run(['import','-window',window,str(png)], check=True, stdout=log, stderr=log, timeout=5)
                values = read_clipboard(png, reference, 0)
                evidence['captures'].append({'phase':phase,'path':png.name,'sha256':digest(png),'variables':values})
                (work/'evidence.json').write_text(json.dumps(evidence, indent=2)+'\n')
            shutil.copyfile(run/'mugen.log',work/'native.log')
        finally:
            subprocess.run(['wineserver','-k'], env=env, stdout=log, stderr=log, check=False)
            try:
                process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
    idle()
    first, second = [row['variables'] for row in evidence['captures']]
    for values in [first,second]:
        if not (values[0] > 0 and values[1]-values[0] == values[2] and
                [values[i] for i in [3,4,5,8,9,11]] == [1,2,1,60,0,0] and values[6] == values[1]):
            raise RuntimeError('Unexpected native clock relationships: '+str(values))
    if second[0] <= first[1]+120:
        raise RuntimeError('Native process clock did not include intervening menus')
    if args.record:
        if fixture.with_suffix('.json').exists():
            raise RuntimeError('Refusing to replace an existing native observation')
        fixture.with_suffix('.json').write_text(json.dumps(evidence, indent=2)+'\n')
    else:
        expected = json.loads(fixture.with_suffix('.json').read_text())
        for key in ['engine_sha256','fixture_sha256','probe_sha256','select_sha256','prepared_sha256']:
            if expected[key] != evidence[key]:
                raise RuntimeError('Pinned native clock input changed: '+key)
    print(json.dumps({'first':first,'second':second,'same_process':True}))


if __name__ == '__main__':
    main()
