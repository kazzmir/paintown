#!/usr/bin/env python3
"""Capture owned round-clock variables with a pinned disposable fight recipe."""
import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time

from reference_probe import capture, digest, prepare, read_clipboard


def capture_cycle(run, prefix, source, pages, seconds):
    """Rotate two clipboard lines per tick; gather all pages from one process."""
    commands = run/'chars/kfm/probe.cmd'
    base = commands.read_text()
    for slot,page in enumerate(pages):
        for chunk in range(2):
            start = page+chunk*6
            base += ('\n[State -2, round trace dump]\ntype='+('DisplayToClipboard' if chunk==0 else 'AppendToClipboard')+
                     '\ntrigger1=GameTime%'+str(len(pages))+'='+str(slot)+'\nignorehitpause=1\ntext="P'+str(start)+':'+','.join(['%d']*6)+'\\n"\nparams='+','.join('Var('+str(i)+')' for i in range(start,start+6))+'\n')
    commands.write_text(base)
    (run/'chars/kfm/probe.cns').write_text(source)
    env=os.environ.copy();env.update(WINEARCH='win32',WINEPREFIX=str(prefix),WINEDEBUG='-all')
    results={}
    with (run.parent/'animation-probe.log').open('w') as log:
        process=subprocess.Popen(['wine','mugen.exe','kfm','kfm','-s','kfm','-nojoy'],cwd=run,env=env,stdout=log,stderr=log)
        try:
            time.sleep(seconds)
            windows=subprocess.run(['xdotool','search','--onlyvisible','--pid',str(process.pid)],capture_output=True,text=True,check=False).stdout.split()
            if len(windows)!=1:raise RuntimeError('Expected exactly one owned native window')
            for attempt in range(120):
                if process.poll() is not None:raise RuntimeError('Native process exited during round capture')
                png=run.parent/('cycle-'+str(attempt)+'.png')
                subprocess.run(['import','-window',windows[0],str(png)],stdout=log,stderr=log,check=True,timeout=5)
                for page in pages:
                    try:values=read_clipboard(png,run,page)
                    except RuntimeError:continue
                    if page not in results:results[page]=(png,values)
                if len(results)==len(pages):break
                time.sleep(.023)
            if len(results)!=len(pages):raise RuntimeError('Incomplete rotating round clipboard: '+str(sorted(results)))
            shutil.copyfile(run/'mugen.log',run.parent/'animation.log')
        finally:
            subprocess.run(['wineserver','-k'],env=env,stdout=log,stderr=log,check=False)
            try:process.wait(timeout=3)
            except subprocess.TimeoutExpired:process.kill();process.wait()
    return results


def override(source, section, key, value):
    parts = re.split(r'(?im)(^\s*\[[^\r\n]+\]\s*$)', source)
    found = 0
    for i in range(1, len(parts), 2):
        if parts[i].strip().lower() != '['+section.lower()+']':
            continue
        parts[i+1], count = re.subn(r'(?im)^\s*'+re.escape(key)+r'\s*=.*$', key+' = '+str(value), parts[i+1])
        if count != 1:
            raise RuntimeError('Expected exactly one recipe field: '+section+'.'+key)
        found += 1
    if found != 1:
        raise RuntimeError('Expected exactly one recipe section: '+section)
    return ''.join(parts)


def idle():
    clients = [row for row in subprocess.check_output(['ps','-eo','pid,stat,comm'], text=True).splitlines()[1:]
               if row.split()[-1] in {'mugen.exe','mugen-go'} or row.split()[-1].endswith('.test')]
    if clients:
        raise RuntimeError('Private display is already in use: '+str(clients))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture', required=True)
    parser.add_argument('--name', required=True)
    parser.add_argument('--record', action='store_true')
    parser.add_argument('--seconds', type=int, default=9)
    parser.add_argument('--pages', type=int, nargs='+')
    args = parser.parse_args()
    if not all(re.fullmatch('[a-z0-9-]+', x) for x in [args.fixture,args.name]) or not 3 <= args.seconds <= 30:
        parser.error('Use simple names and a bounded capture delay')
    project = Path(__file__).resolve().parents[1]
    fixture = project/'tests/testdata/reference'/args.fixture
    recipe = json.loads(fixture.with_suffix('.round.json').read_text())
    pages = args.pages or recipe.get('pages', [0,12,24,36,48])
    if any(page < 0 or page > 48 for page in pages):
        parser.error('Variable page exceeds the integer bank')
    reference = project/'temp/mugen-1.1b1'
    work = project/'temp/reference-runs'/args.name
    idle()
    work.mkdir(parents=True, exist_ok=False)
    run = work/'stock'
    source = prepare(reference, run, fixture, version='1.1', fixture_constants=True)
    originals, prepared = {}, {}
    for relative in ['data/fight.def','data/mugen1/fight.def']:
        path = run/relative
        originals[relative] = digest(path)
        text = path.read_text(encoding='utf-8-sig')
        for field,value in recipe.get('fight', {}).items():
            section,key = field.split('.',1)
            text = override(text,section,key,value)
        path.write_text(text)
        prepared[relative] = digest(path)
    config = run/'data/mugen.cfg'
    text = override(config.read_text(encoding='utf-8-sig'), 'Options', 'Time', recipe.get('round_time',99))
    config.write_text(text)
    evidence = {'engine_sha256':digest(reference/'mugen.exe'), 'fixture_sha256':{ext:digest(fixture.with_suffix(ext)) for ext in ['.cns','.air','.cmd','.round.json']},
                'original_fight_sha256':originals,'prepared_fight_sha256':prepared,'config_sha256':digest(config),'captures':[]}
    expected = None if args.record else json.loads(fixture.with_suffix('.json').read_text())
    if expected:
        for field in ['engine_sha256','fixture_sha256','original_fight_sha256','prepared_fight_sha256','config_sha256']:
            if expected[field] != evidence[field]:raise RuntimeError('Pinned round recipe changed: '+field)
    dense = recipe.get('dense', False)
    cycle = capture_cycle(run,project/'temp/wine-reference',source,pages,args.seconds) if dense else None
    for page in ([0] if dense else pages):
        idle()
        if not dense:png = capture(run,project/'temp/wine-reference',source,page,args.seconds,'var',True,'kfm',0)
        for observed_page in (pages if dense else [page]):
            if dense:png,values = cycle[observed_page]
            else:values = read_clipboard(png,reference,observed_page)
            evidence['captures'].append({'page':observed_page,'path':png.name,'sha256':digest(png),'variables':values})
            (work/'evidence.json').write_text(json.dumps(evidence,indent=2)+'\n')
            if expected:
                raw = next(row['variables'] for row in expected['captures'] if row['page']==observed_page)
                want = {int(index):value for index,value in raw.items()}
                if values != want:raise RuntimeError('Native round trace changed: '+str(values))
            print(args.fixture, observed_page, values, flush=True)
    (work/'evidence.json').write_text(json.dumps(evidence,indent=2)+'\n')
    if args.record:fixture.with_suffix('.json').write_text(json.dumps(evidence,indent=2)+'\n')


if __name__ == '__main__':main()
