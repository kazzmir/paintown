#!/usr/bin/env python3
"""Locate the original RGB spark marker in an owned native window capture.

The calibration is deliberately limited to these static 320x240 scenes:
ground zoffset 200, blank fighters and the original formats.sff
RGB ramp. It measures geometry, not full framebuffer or blend/timing parity.
"""
import argparse
from collections import deque
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
import sys

from PIL import Image


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def marker(path, expect_none=False):
    image = Image.open(path).convert('RGB')
    width, height = image.size
    mask = bytearray(blue == 75 and 0 <= green <= 201 and 0 <= red <= 217
                     for red, green, blue in image.getdata())
    components = []
    for start in range(width * (height // 4), len(mask)):
        if not mask[start]:
            continue
        mask[start] = 0
        queue, points = deque([start]), []
        while queue:
            offset = queue.popleft()
            x, y = offset % width, offset // width
            points.append((x, y))
            for adjacent in ([offset - 1] if x else []) + ([offset + 1] if x + 1 < width else []) + ([offset - width] if y else []) + ([offset + width] if y + 1 < height else []):
                if mask[adjacent]:
                    mask[adjacent] = 0
                    queue.append(adjacent)
        left, top = min(x for x,y in points), min(y for x,y in points)
        right, bottom = max(x for x,y in points)+1, max(y for x,y in points)+1
        if len(points) >= 80 and len(points) == (right-left)*(bottom-top):
            components.append((left,top,right,bottom))
    if expect_none:
        if components:
            raise RuntimeError('Disabled spark unexpectedly displayed its original marker')
        return {'window': [width,height], 'visible': False}
    if len(components) != 1:
        raise RuntimeError('Expected one solid original RGB marker core, found ' + str(components))
    left,top,right,bottom = components[0]
    pixels = image.load()
    flipped = pixels[left,top][0] > pixels[right-1,top][0]
    if not pixels[left,top][1] <= 8 or not pixels[left,bottom-1][1] >= 190:
        raise RuntimeError('Marker does not have the authored vertical ramp')
    # The flipped marker's right core edge coincides with its geometric axis.
    # Non-flipped scenes need an independently measured filter-edge correction.
    if not flipped:
        raise RuntimeError('This calibration currently covers the flipped marker')
    scale = height / 240
    return {'window': [width,height], 'visible': True, 'core': [left,top,right,bottom], 'facing': -1,
            'core_sha256': hashlib.sha256(image.crop((left,top,right,bottom)).tobytes()).hexdigest(),
            'pixel_scale': scale, 'frame_anchor_240p': [(right-width/2)/scale, top/scale-200]}


def main():
    project = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fixture',required=True)
    parser.add_argument('--name',required=True)
    parser.add_argument('--action',type=int,required=True)
    parser.add_argument('--record',action='store_true')
    parser.add_argument('--common',action='store_true')
    parser.add_argument('--fixture-stage',action='store_true',help='Use the authored fixed-camera stage sidecar; reported anchors remain screen-relative')
    parser.add_argument('--expect-none',action='store_true')
    parser.add_argument('--adopt-run',help='Promote an existing source-hash-checked owned capture without another native launch')
    args=parser.parse_args()
    if not all(re.fullmatch('[a-z0-9_-]+',name) for name in [args.fixture,args.name]+([args.adopt_run] if args.adopt_run else [])):
        parser.error('Use simple fixture and run names')
    stem=project/'tests/testdata/reference'/args.fixture
    expected=json.loads(stem.with_suffix('.json').read_text()) if not args.record else None
    if expected is not None:
        args.common=expected['spark']['common']
        args.fixture_stage=expected.get('fixture_stage',False)
        args.expect_none=not expected['spark']['visible']
        if args.action!=expected['spark']['action']:
            parser.error('Action differs from pinned marker evidence')
    work=project/'temp/spark-runs'/args.name
    work.mkdir(parents=True,exist_ok=False)
    hashes={}
    command=[sys.executable,str(project/'tests/reference_probe.py'),'--fixture',args.fixture,'--name',args.name,
             '--fixture-constants','--file','sprite=probe.sff','--mugenversion','1.1','--bank','fvar',
             '--clipboard-in-cmd','--pages','0','--seconds','6']
    if args.fixture_stage:
        command+=['--fixture-stage']
    if args.common:
        if args.adopt_run:
            parser.error('Adoption is only supported for unmodified own-bank scenes')
        reference=work/'reference'
        shutil.copytree(project/'temp/mugen-1.1b1',reference)
        for name,source in [('marker-fx.sff',stem.with_suffix('.extra')/'probe.sff'),('marker-fx.air',stem.with_suffix('.air'))]:
            target=reference/'data'/name
            shutil.copyfile(source,target)
            hashes['data/'+name]=digest(target)
        for fight in sorted(reference.glob('data/**/fight.def')):
            definition=fight.read_text(encoding='utf-8-sig')
            for key,value in [('fightfx.sff','data/marker-fx.sff'),('fightfx.air','data/marker-fx.air')]:
                definition=re.sub(r'(?im)^\s*'+re.escape(key)+r'\s*=.*$',key+' = '+value,definition)
            fight.write_text(definition)
            hashes[str(fight.relative_to(reference))]=digest(fight)
        command+=['--reference',str(reference)]
    if args.record:
        command+=['--record']
    if args.adopt_run:
        run=project/'temp/reference-runs'/args.adopt_run
    else:
        subprocess.run(command,check=True)
        run=project/'temp/reference-runs'/args.name
    evidence=json.loads((run/'evidence.json').read_text())
    if evidence.get('load_error'):
        raise RuntimeError('Native spark scene failed to load')
    for suffix,key in [('.cns','cns_sha256'),('.air','air_sha256')]:
        if digest(stem.with_suffix(suffix))!=evidence[key]:
            raise RuntimeError('Source changed since the adopted capture')
    if digest(stem.with_suffix('.extra')/'probe.sff')!=evidence['extra_sha256']['probe.sff']:
        raise RuntimeError('Original marker bank changed since capture')
    if args.fixture_stage and digest(stem.with_suffix('.stage.def'))!=evidence.get('stage_fixture_sha256'):
        raise RuntimeError('Authored stage changed since capture')
    spark=marker(run/'animation-page0.png',args.expect_none or args.action<0)
    spark.update(action=args.action,common=args.common,recipe_sha256=hashes)
    (work/'measurements.json').write_text(json.dumps(spark,indent=2)+'\n')
    if args.record:
        evidence['spark']=spark
        stem.with_suffix('.json').write_text(json.dumps(evidence,indent=2)+'\n')
    elif spark!=expected['spark']:
        raise RuntimeError('Native marker geometry/filter core differs from pinned evidence')
    print(('Recorded' if args.record else 'Verified')+' native spark marker '+str(spark.get('frame_anchor_240p','disabled')))


if __name__=='__main__':
    main()
