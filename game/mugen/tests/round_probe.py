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

from reference_probe import capture, digest, prepare, read_clipboard, read_clipboard_lines


def capture_cycle(run, prefix, source, pages, seconds, mode="quick"):
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
        launch=['wine','mugen.exe']+(['kfm','kfm','-s','kfm'] if mode=='quick' else [])+['-nojoy']
        process=subprocess.Popen(launch,cwd=run,env=env,stdout=log,stderr=log)
        try:
            if mode=='versus':
                time.sleep(4)
                owned=subprocess.check_output(['xdotool','search','--onlyvisible','--pid',str(process.pid)],text=True).split()
                if len(owned)!=1:
                    helper=prefix.parent/'reference-tools/wintext.exe'
                    if helper.exists():
                        result=subprocess.run(['wine',str(helper)],env=env,capture_output=True,text=True,timeout=5)
                        (run.parent/'load-error.txt').write_text(result.stdout+result.stderr)
                    raise RuntimeError('Expected one owned menu window; inspect load-error.txt')
                window=owned[0]
                for label,keys,delay in [('welcome',['F1'],1),('title',['Down','Return'],2),('select',['comma','f'],2),('stage',['Return'],3)]:
                    subprocess.run(['import','-window',window,str(run.parent/(label+'.png'))],check=True,stdout=log,stderr=log)
                    subprocess.run(['xdotool','key','--window',window,'--delay','120',*keys],check=True,stdout=log,stderr=log)
                    time.sleep(delay)
            time.sleep(seconds)
            windows=subprocess.run(['xdotool','search','--onlyvisible','--pid',str(process.pid)],capture_output=True,text=True,check=False).stdout.split()
            if len(windows)!=1:
                helper=prefix.parent/'reference-tools/wintext.exe'
                if helper.exists():
                    result=subprocess.run(['wine',str(helper)],env=env,capture_output=True,text=True,timeout=5)
                    (run.parent/'load-error.txt').write_text(result.stdout+result.stderr)
                raise RuntimeError('Expected exactly one owned native window; inspect load-error.txt')
            for attempt in range(120):
                if process.poll() is not None:raise RuntimeError('Native process exited during round capture')
                png=run.parent/('cycle-'+str(attempt)+'.png')
                subprocess.run(['import','-window',windows[0],str(png)],stdout=log,stderr=log,check=True,timeout=5)
                try:lines=read_clipboard_lines(png,run)
                except RuntimeError:continue
                for page in pages:
                    if page not in lines or page+6 not in lines:continue
                    values={first+i:value for first in [page,page+6] for i,value in enumerate(lines[first])}
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
        # The legacy whitespace matcher can consume the separator when this
        # is a section's first attribute. Preserve every previously valid
        # preparation while keeping the header on its own line.
        if not parts[i].endswith(('\n','\r')) and not parts[i+1].startswith(('\n','\r')):
            parts[i+1]='\n'+parts[i+1]
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
    mode=recipe.get('native_mode','quick')
    if mode not in ['quick','versus']:raise RuntimeError('Unsupported native launch mode')
    if mode=='versus':
        select=run/'data/select.def'
        select.write_text('[Characters]\nkfm, stages/kfm.def\n[ExtraStages]\nstages/kfm.def\n[Options]\narcade.maxmatches=1\nteam.maxmatches=1\n')
        evidence['native_mode']=mode
        evidence['select_sha256']=digest(select)
        if expected and any(expected.get(k)!=evidence[k] for k in ['native_mode','select_sha256']):raise RuntimeError('Pinned native mode inputs changed')
    dense = recipe.get('dense', False)
    cycle = capture_cycle(run,project/'temp/wine-reference',source,pages,args.seconds,mode) if dense else None
    menu_delta=0
    if expected and mode=='versus':
        if not dense or 0 not in cycle:raise RuntimeError('Versus verification needs the first-CNS timestamp')
        origin=next(row['variables']['0'] for row in expected['captures'] if row['page']==0)
        menu_delta=cycle[0][1][0]-origin
        evidence['menu_clock_delta']=menu_delta
    timestamp_offsets={0,1,2,3,4,5,7,8,9,10,11,12}
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
                comparable={index:(value-menu_delta if mode=='versus' and value>0 and index%20 in timestamp_offsets else value) for index,value in values.items()}
                if comparable != want:raise RuntimeError('Native round trace changed: '+str(values))
            print(args.fixture, observed_page, values, flush=True)
    (work/'evidence.json').write_text(json.dumps(evidence,indent=2)+'\n')
    if args.record:fixture.with_suffix('.json').write_text(json.dumps(evidence,indent=2)+'\n')


if __name__ == '__main__':main()
