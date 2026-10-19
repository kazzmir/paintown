#!/usr/bin/env python3
"""Reconcile beta1 trigger vocabulary with candidate production runtime routes.

This is a source audit, not a behavioral conformance test. It deliberately
excludes parser/name-validation switches from runtime evidence.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
ROUTES = {
    'evaluator/evaluator.go': ['evaluateIdentifier'],
    'evaluator/runtime.go': ['identifier', 'callWithArguments', 'legacyWithArguments'],
    'character/context.go': ['LookupIdentifier', 'lookupViewport'],
    'character/world.go': ['lookupEntity', 'LookupFunction'],
    'character/width.go': ['lookupEdges'],
}
SPECIAL = {
    'command': ('evaluator/runtime.go', 'binary', 'id.Name, "command"'),
    'const': ('evaluator/runtime.go', 'callWithArguments', 'name == "const"'),
    'const240p': ('evaluator/runtime.go', 'callWithArguments', 'name == "const240p"'),
    'const480p': ('evaluator/runtime.go', 'callWithArguments', 'name == "const480p"'),
    'const720p': ('evaluator/runtime.go', 'callWithArguments', 'name == "const720p"'),
    'ifelse': ('evaluator/runtime.go', 'callWithArguments', 'name == "ifelse"'),
    'cond': ('evaluator/runtime.go', 'callWithArguments', 'name == "cond"'),
    'gethitvar': ('evaluator/runtime.go', 'callWithArguments', 'name == "gethitvar"'),
    'log': ('evaluator/runtime.go', 'callWithArguments', 'name == "log"'),
    'hitdefattr': ('character/context.go', 'LookupLegacy', 'name != "hitdefattr"'),
    'numprojid': ('character/projectile.go', 'lookupProjectileCount', 'name != "numprojid"'),
}


def function(source, name):
    match = re.search(r'^func (?:\([^\n]+\) )?' + re.escape(name) + r'\(', source, re.M)
    if not match:
        raise RuntimeError('Missing audited function: ' + name)
    tail = source[match.start():]
    end = re.search(r'^func |^type |^var |^const ', tail[1:], re.M)
    return tail[:end.start()+1] if end else tail


def cases(body):
    return {value for group in re.findall(r'\bcase ((?:"[^"\n]+"\s*,?\s*)+):', body)
            for value in re.findall(r'"([^"\n]+)"', group)}


def inventory():
    document = ROOT/'temp/mugen-1.1b1/docs/trigger.html'
    manual = document.read_text()
    sections = re.findall(r'<div class="section" id="([^"]+)">\s*<h2>', manual)
    names = sorted(name.removesuffix('-math') for name in sections)
    vocabulary = re.search(r'const names = `([^`]+)`', (ROOT/'evaluator/names.go').read_text())[1].split()
    if len(names) != 127 or set(names) != set(vocabulary):
        raise RuntimeError('Primary manual and recognized trigger vocabulary differ')
    sources = {path: (ROOT/path).read_text() for path in set(ROUTES) | {x[0] for x in SPECIAL.values()}}
    evidence = {name: set() for name in names}
    for path, functions in ROUTES.items():
        for name in functions:
            for value in cases(function(sources[path], name)):
                root = value.split()[0]
                if root in evidence:
                    evidence[root].add(path + ':' + name)
    for name, (path, method, literal) in SPECIAL.items():
        if literal not in function(sources[path], method):
            raise RuntimeError('Explicit routing evidence changed: ' + name)
        evidence[name].add(path + ':' + method)
    rows = [{'name': name, 'status': 'route-present' if evidence[name] else 'unbound',
             'routes': sorted(evidence[name]), 'variants_complete': False} for name in names]
    hitblock = manual.split('<div class="section" id="gethitvar">')[1].split('<div class="section"')[0]
    argument_list = re.search(r'<dd>(xveladd,.*?)</dd>', hitblock, re.S)[1]
    fields = set(re.sub(r'<[^>]*>', '', argument_list).replace('\n', ' ').replace('\r', ' ').strip().removesuffix('.').split(', '))
    fields = {name.strip() for name in fields}
    fields.update(re.findall(r'<li>([a-z][a-z.]+):', hitblock))
    supported = cases(function(sources['evaluator/runtime.go'], 'callWithArguments'))
    supported &= cases(function((ROOT/'character/character.go').read_text(), 'GetHitVar'))
    bound = sorted(fields & supported)
    accepted = cases(function((ROOT/'evaluator/validate.go').read_text(), 'knownGetHitSelector'))
    return {'target': 'M.U.G.E.N 1.1 beta 1', 'kind': 'source-routing-inventory',
            'manual_sha256': hashlib.sha256(document.read_bytes()).hexdigest(),
            'family_count': len(rows), 'route_present': sum(row['status'] == 'route-present' for row in rows),
            'unbound': [row['name'] for row in rows if row['status'] == 'unbound'],
            'triggers': rows,
            'gethitvar': {'documented_count': len(fields), 'bound_selectors': bound,
                         'unbound_selectors': sorted(fields-supported),
                         'parser_rejected_selectors': sorted(fields-accepted),
                         'accepted_but_unbound_selectors': sorted((fields & accepted)-supported),
                         'extra_bound_selectors': sorted(supported-fields)},
            'limits': ['Route-present does not mean native conformance or complete variants.',
                       'Context-dependent bottom is distinct from an absent runtime route.',
                       'Controller registration/stub and asset/mode coverage are separate inventories.']}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write', action='store_true', help='Refresh the source inventory after an intentional change')
    args = parser.parse_args()
    actual = inventory()
    path = ROOT/'docs/TRIGGER-BINDINGS.json'
    if args.write:
        path.write_text(json.dumps(actual, indent=2)+'\n')
    elif actual != json.loads(path.read_text()):
        raise RuntimeError('Runtime routing inventory is stale; review the change before --write')
    print(json.dumps({key: actual[key] for key in ['family_count','route_present','unbound','gethitvar']}))


if __name__ == '__main__':
    main()
