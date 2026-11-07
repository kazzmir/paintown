#!/usr/bin/env python3
"""Report implementation gaps from production source and a small feature catalog.

No game, reference distribution, capture corpus, Go build or display is needed.
This checks feature availability, not execution-line or pixel-conformance coverage.
"""
import argparse
from collections import Counter
import importlib.util
import json
from pathlib import Path
import re
import sys

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[1]
CATALOG = ROOT / 'docs/STATE-MACHINE-FEATURES.json'
REPORT = ROOT / 'docs/PROGRESS.md'


def load_audit(name):
    # Reuse source-route definitions; their optional native inventory() is never
    # invoked. These modules do not open reference files at import time.
    spec = importlib.util.spec_from_file_location(name, ROOT / 'tests' / (name + '.py'))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def source(path):
    return (ROOT / path).read_text()


def location(path, needle):
    text = source(path)
    at = text.find(needle)
    return f'{path}:{text[:at].count(chr(10)) + 1}' if at >= 0 else None


def report():
    catalog = json.loads(CATALOG.read_text())
    if catalog['schema'] != 1:
        raise ValueError('Unsupported state-machine feature catalog schema')
    controller_audit = load_audit('controller_inventory')
    trigger_audit = load_audit('trigger_inventory')
    baseline = json.loads((ROOT / 'docs/CONTROLLER-BINDINGS.json').read_text())
    names = {row['name'] for row in baseline['controllers']}
    if len(names) != 89:
        raise ValueError('Controller vocabulary must contain the 89 beta families')
    registry = controller_audit.function(source('engine/statemachine.go'), 'registerCoreControllers')
    registered = dict(re.findall(r'sm\.controllers\["([^"]+)"\]\s*=\s*(\w+)', registry))
    compiler = controller_audit.function(source('engine/parameters.go'), 'compileCoreController')
    typed = {name for clause in re.findall(r'\bcase ([^:\n]+):', compiler)
             for name in re.findall(r'"([^"]+)"', clause)}
    errors = []
    extra = sorted(set(registered) - names)
    if set(extra) != set(catalog['controller_extensions']):
        errors.append('Review uncatalogued/removed controller extensions: ' + ', '.join(extra))
    gaps = catalog['controller_gaps']
    for name in set(gaps) - names:
        errors.append('Unknown controller gap: ' + name)
    controllers = []
    for name in sorted(names):
        handler = registered.get(name)
        route = 'typed' if name in typed else 'fallback' if handler else 'missing'
        status = 'partial' if name in gaps else 'present'
        if handler is None:
            status = 'missing'
        elif handler != 'func':
            try:
                body = controller_audit.function(source('engine/statemachine.go'), handler)
                if re.search(r'(?im)^\s*//\s*(?:stub|todo: implement)', body):
                    status = 'missing'
            except RuntimeError:
                status = 'missing'
        gap = gaps.get(name, {})
        controllers.append({'name': name, 'status': status, 'parameter_route': route,
                            'source': location('engine/statemachine.go', 'sm.controllers["' + name + '"]'),
                            'gap': gap.get('gap'), 'allocation': gap.get('allocation', 'item3')})
    vocabulary = re.search(r'const names = `([^`]+)`', source('evaluator/names.go'))[1].split()
    if len(set(vocabulary)) != 127:
        raise ValueError('Trigger vocabulary must contain the 127 beta families')
    routes = {name: set() for name in vocabulary}
    for path, functions in trigger_audit.ROUTES.items():
        for name in functions:
            try:
                body = trigger_audit.function(source(path), name)
            except RuntimeError as error:
                errors.append(str(error))
                continue
            for value in trigger_audit.cases(body):
                key = value.split()[0]
                if key in routes:
                    routes[key].add(path + ':' + name)
    for name, (path, method, literal) in trigger_audit.SPECIAL.items():
        try:
            if literal not in trigger_audit.function(source(path), method):
                errors.append('Explicit trigger route changed: ' + name)
            else:
                routes[name].add(path + ':' + method)
        except RuntimeError as error:
            errors.append(str(error))
    triggers = [{'name': name, 'status': 'present' if routes[name] else 'missing',
                 'sources': sorted(routes[name]),
                 'allocation': catalog['trigger_gap_allocations'].get(name, 'item3')}
                for name in sorted(routes)]
    features = []
    identifiers = set()
    for entry in catalog['mechanics']:
        entry = dict(entry)
        if entry['id'] in identifiers or entry['status'] not in {'implemented', 'partial', 'missing'}:
            raise ValueError('Invalid/duplicate mechanic: ' + entry['id'])
        identifiers.add(entry['id'])
        missing = []
        refs = []
        for ref in entry['sources']:
            path, separator, symbol = ref.partition(':')
            try:
                text = source(path)
                body = controller_audit.function(text, symbol) if separator else text
                refs.append(location(path, body.splitlines()[0]))
                if separator and re.search(r'(?im)^\s*//\s*(?:stub|todo: implement)', body):
                    missing.append(ref + ' is a stub')
            except (OSError, RuntimeError):
                missing.append(ref + ' is absent')
        if missing:
            entry['status'] = 'missing'
            entry['gap'] = '; '.join(missing)
            errors.extend(missing)
        entry['sources'] = refs
        features.append(entry)
    return {'target': 'M.U.G.E.N 1.1 beta 1',
            'scope': 'implementation availability and explicit functional gaps; not native conformance',
            'summary': {'controllers': dict(Counter(row['status'] for row in controllers)),
                        'controller_routes': dict(Counter(row['parameter_route'] for row in controllers)),
                        'triggers': dict(Counter(row['status'] for row in triggers)),
                        'mechanics': dict(Counter(row['status'] for row in features))},
            'controller_extensions': extra, 'controllers': controllers, 'triggers': triggers,
            'mechanics': features, 'segments': load_audit('segments').statuses(catalog), 'structural_errors': errors,
            'presentation_followups': catalog['presentation_followups']}


def summary_lines(value):
    s = value['summary']
    return [f"Controllers: {sum(x['parameter_route'] != 'missing' for x in value['controllers'])}/89 routes; "
            f"{s['controllers'].get('partial', 0)} known partial, {s['controllers'].get('missing', 0)} missing.",
            f"Triggers: {s['triggers'].get('present', 0)}/127 runtime routes; "
            f"{s['triggers'].get('missing', 0)} missing.",
            f"Project items: {s['mechanics'].get('implemented', 0)} implemented, "
            f"{s['mechanics'].get('partial', 0)} partial, {s['mechanics'].get('missing', 0)} missing."]


def markdown(value):
    lines = ['# Project progress', '',
             'This is the single completion ledger for the Go MUGEN 1.1 beta port.',
             'Checked items have a working implementation for the stated scope. Partial',
             'items name the remaining work. Counts below are feature/route counts, not',
             'an estimate of total effort or complete native compatibility.', '',
             *[f'- {line}' for line in summary_lines(value)], '',
             '## Completed scenario coverage', '',
             'Completed segments are excluded from default runs. Only a change to their',
             'covered source, a reported regression, or an explicit rerun reopens them.',
             'These scenarios passed at M4dr; reorganizing their entry points does not',
             'require running the historical data again.', '']
    for row in value['segments']:
        mark = 'x' if row['complete'] else ' '
        lines.append(f"- [{mark}] **{row['id']}**: {row['contract']}.")
    lines.extend(['', '## Project breakdown', ''])
    for group in dict.fromkeys(row['group'] for row in value['mechanics']):
        lines.extend(['### ' + group, ''])
        for row in value['mechanics']:
            if row['group'] != group:
                continue
            mark = 'x' if row['status'] == 'implemented' else ' '
            detail = '' if mark == 'x' else f" **{row['status']}** — {row.get('gap', '')}"
            if row.get('allocation') != 'item3':
                detail += ' (' + row['allocation'] + ')'
            lines.append(f"- [{mark}] {row['name']}.{detail}")
        lines.append('')
    lines.extend(['## Controllers and trigger gaps', '',
                  'All 89 documented controller families are registered. The source check',
                  'flags concrete missing/partial consumers below. A route alone does not',
                  'prove every parameter combination; untested permutations are not new',
                  'implementation tasks by themselves.', '',
                  '| Controller | Remaining implementation |', '| --- | --- |'])
    for row in value['controllers']:
        if row['status'] != 'present':
            lines.append(f"| {row['name']} | {row.get('gap') or 'Missing production route.'} |")
    missing = [row['name'] for row in value['triggers'] if row['status'] == 'missing']
    lines.extend(['', 'Missing trigger routes: ' + ', '.join('`' + x + '`' for x in missing) + '.', '',
                  'Stage/team context belongs with the later modes/assets allocation; edge',
                  'bindings remain state-machine work.', '',
                  '## How progress is updated', '',
                  '1. Pick an unchecked feature and implement a coherent batch.',
                  '2. Update its status in `STATE-MACHINE-FEATURES.json`.',
                  '3. Validate the affected connected scenario at the end of the batch.',
                  '4. Record its checkmark here, commit source/text, and remove scratch output.', '',
                  'Use `mugen.sh --coverage --write` to refresh this page and',
                  '`mugen.sh --check` to run pending/reopened segments only. Explicit',
                  '`--check --segment NAME` reruns one segment. No reference executable,',
                  'asset copies or capture matrices are needed for the coverage report.', '',
                  'The check detects removed source entry points, uncatalogued controllers',
                  'and missing trigger bindings. Its reviewed feature catalog is a backlog,',
                  'not a proof of arbitrary semantics. Full historical diagnostics are opt-in',
                  'and are not completion gates for unrelated work.', '',
                  'See [workflow](WORKFLOW.md), [implementation plan](ITEM-3-PLAN.md),',
                  '[test segments](../tests/SEGMENTS.md) and [handoff](HANDOFF.md).', ''])
    return '\n'.join(lines)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--json', action='store_true', help='Print complete coverage as JSON')
    parser.add_argument('--gaps', action='store_true', help='List concrete outstanding implementation work')
    parser.add_argument('--write', action='store_true', help='Refresh the small Markdown progress report')
    parser.add_argument('--check', action='store_true', help='Fail for broken source/catalog links or uncatalogued extensions')
    parser.add_argument('--require-complete', action='store_true', help='Also fail for partial/missing capabilities')
    args = parser.parse_args()
    try:
        value = report()
    except (OSError, ValueError, RuntimeError, KeyError) as error:
        print('Coverage error: ' + str(error), file=sys.stderr)
        return 2
    if args.write:
        REPORT.write_text(markdown(value))
    if args.json:
        print(json.dumps(value, indent=2))
    else:
        print('\n'.join(summary_lines(value)))
        if args.gaps:
            for group in ['mechanics', 'controllers', 'triggers']:
                for row in value[group]:
                    if row['status'] in {'partial', 'missing'}:
                        print(f"{group}/{row.get('id', row.get('name'))}: {row.get('gap') or 'runtime route missing'}")
        for error in value['structural_errors']:
            print('Coverage error: ' + error, file=sys.stderr)
    incomplete = any(row['status'] in {'partial', 'missing'}
                     for group in ['mechanics', 'controllers', 'triggers'] for row in value[group])
    return int(bool(value['structural_errors']) or (args.require_complete and incomplete))


if __name__ == '__main__':
    sys.exit(main())
