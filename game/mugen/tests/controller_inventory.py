#!/usr/bin/env python3
"""Reconcile beta controller names with compiler routes and known consumer gaps.

A source audit only: registration, a typed schema and behavioral acceptance are
separate facts. No controller is automatically labelled variant-complete.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
SOURCES = ['engine/statemachine.go', 'engine/parameters.go',
           'character/character.go', 'character/player.go',
           'character/explod.go', 'engine/explod.go', 'engine/modify_explod.go',
           'effect/scene.go', 'character/actor_order.go', 'system/effects.go', 'render/perspective.go',
           'render/alpha.go', 'render/nearest.go', 'render/linear.go',
           'engine/drawing.go', 'engine/trans.go', 'engine/palette_fx.go', 'engine/env_color.go', 'engine/env_shake.go', 'engine/shake.go', 'engine/timeline.go', 'engine/round_clock.go',
           'palette/state.go', 'palette/controller_effect.go', 'palette/hit_effect.go',
           'render/palette.go', 'character/helper.go', 'character/tick.go',
           'background/background.go', 'stage/stage.go', 'font/font.go', 'system/fight_state.go', 'system/hud.go',
           'engine/self_binding.go', 'engine/target_self_binding.go', 'character/binding.go', 'character/targets.go', 'character/world.go']
STUBS = {
    'afterimage': 'AfterImage', 'afterimagetime': 'AfterImageTime',
    'forcefeedback': 'ForceFeedback', 'gamemakeanim': 'GameMakeAnim',
    'makedust': 'MakeDust',
}
RENDER_FIELDS = {}
PARTIAL = {
	'modifyexplod': 'MODIFY-EXPLOD.md: native mutation/schema consumers; bounded shared alpha now matches; EFFECT-ORDERING.md: measured actor/effect priority and ties consumed; shadow/pause/projection sampling gates remain',
	'varset': 'native literal-float integer rejection found in M4cj setup diagnostic; residual schema closure pending',
    'projectile': 'projectile shadow/trail fields lack complete renderer consumers',
    'explod': 'EXPLOD-ATTACHMENT.md: native entity/legacy binding, motion and clocks; EFFECT-ORDERING.md: measured priorities/ties/creation order consumed; creation palette/random/angle/trans/shadow/capacity and broader variants open',
    'assertspecial': 'only a subset of literal flags have production consumers',
    'appendtoclipboard': 'stores raw text; params/formatting absent',
    'displaytoclipboard': 'stores raw text; params/formatting absent',
    'hitadd': 'updates ComboCount; no production reader of that field was found',
    'statetypeset': 'fallback literal handling; full schema/version/error matrix open',
}
BOUNDED = {
    'envshake': 'ENV-SHAKE.md: native schemas/evaluation, author-local amplitude, frequency normalization, pending/current clocks and actual fixed-overlay/viewport consumers; wider variants open',
    'envcolor': 'ENV-COLOR.md: native packed RGB/evaluation/global clocks, finite overlays and negative-duration stage suppression, actor/effect/foreground/palette production consumers; wider variants open',
    'allpalfx': 'GLOBAL-PALETTE-FX.md: native independent global clock, parameter merge and two-stage actor/effect/stage/HUD/portrait/font production drawing; wider variants remain open',
    'bgpalfx': 'GLOBAL-PALETTE-FX.md: native global clock and stage-only palette pass, independent cancellation/composition; sampled actors/HUD unchanged despite manual wording',
    'palfx': 'PALETTE-FX.md: native schemas/evaluation, helper ownership, expiry/sine, unclamped indexed/RGB production drawing; wider global/contact/creation combinations open',
    'angleset': 'ANGLES-OFFSET.md: native persistent float32 angle register, omitted zero and production drawing activation; wider combinations open',
    'angleadd': 'ANGLES-OFFSET.md: native persistent float32 angle addition and renderer consumer; wider combinations open',
    'anglemul': 'ANGLES-OFFSET.md: native persistent float32 multiplication and omitted-zero behavior; wider combinations open',
    'angledraw': 'ANGLES-OFFSET.md: native activation, cumulative frame scales, value/scale evaluation and production rotation; final raster gates explicit',
    'offset': 'ANGLES-OFFSET.md: native frame-local partial updates, constants/facing/profile geometry and production drawing; wider combinations open',

    'trans': 'TRANS.md: native schemas/evaluation, AIR defaults, per-actor overrides, expiry/frozen frame/transition and shared blend consumer; wider pause/RGBA combinations open',
    'bindtotarget': 'TARGET-SELF-BINDING.md: native selection, immediate caller position, reverse constraint, retained/quantized offsets and profiles; wider interactions open',
    'explodbindtime': 'EXPLOD-BINDING.md: native ID/time/alias/geometry controls; broader ownership/pause interactions open',
    'bindtoparent': 'SELF-BINDING.md: helper constraints, phase/velocity/facing/lifecycle; wider interactions open',
    'bindtoroot': 'SELF-BINDING.md: root constraints, phase/velocity/facing/lifecycle; wider interactions open',
}


def function(source, name):
    match = re.search(r'^func (?:\([^\n]+\) )?' + re.escape(name) + r'\(', source, re.M)
    if not match:
        raise RuntimeError('Missing audited function: ' + name)
    tail = source[match.start():]
    end = re.search(r'^func |^type |^var |^const ', tail[1:], re.M)
    return tail[:end.start()+1] if end else tail


def inventory():
    doc = ROOT/'temp/mugen-1.1b1/docs/sctrls.html'
    names = sorted(re.findall(r'<div class="section" id="([^"]+)">\s*<h2>', doc.read_text()))
    if len(names) != 89:
        raise RuntimeError('Unexpected beta controller vocabulary')
    sources = {p: (ROOT/p).read_text() for p in SOURCES}
    body = function(sources['engine/statemachine.go'], 'registerCoreControllers')
    registered = dict(re.findall(r'sm\.controllers\["([^"]+)"\]\s*=\s*(\w+)', body))
    body = function(sources['engine/parameters.go'], 'compileCoreController')
    typed = {name for clause in re.findall(r'\bcase ([^:\n]+):', body)
             for name in re.findall(r'"([^"]+)"', clause)}
    rows = []
    for name in names:
        route = 'typed' if name in typed else 'fallback' if name in registered else 'missing'
        status = 'route-only'
        evidence = []
        if name in STUBS:
            sink = function(sources['character/character.go'], STUBS[name])
            if 'Stub' not in sink and not re.search(r'// Logic:', sink):
                raise RuntimeError('Review changed stub classification: ' + name)
            status = 'stub'
            evidence.append('character/character.go:' + STUBS[name])
        elif name in RENDER_FIELDS:
            field = RENDER_FIELDS[name]
            if field in sources['character/player.go']:
                raise RuntimeError('Review new renderer consumer: ' + name)
            status = 'unconsumed-render-field'
            evidence.append('character/player.go lacks ' + field)
        elif name in PARTIAL:
            status = 'partial-consumer'
            evidence.append(PARTIAL[name])
        elif name in BOUNDED:
            status = 'bounded-native-consumer'
            evidence.append(BOUNDED[name])
        elif route == 'missing':
            status = 'missing-dispatcher'
        rows.append({'name': name, 'registered': name in registered,
                     'parameter_route': route, 'handler': registered.get(name),
                     'consumer_status': status, 'evidence': evidence,
                     'variants_complete': False})
    flags = set()
    for directory in ['character', 'match', 'system']:
        for path in (ROOT/directory).glob('*.go'):
            if not path.name.endswith('_test.go'):
                flags.update(re.findall(r'Specials\["([^"]+)"\]', path.read_text()))
    counts = {key: sum(row['consumer_status'] == key for row in rows)
              for key in ['stub', 'unconsumed-render-field', 'partial-consumer',
                          'missing-dispatcher', 'bounded-native-consumer', 'route-only']}
    return {'target': 'Elecbyte M.U.G.E.N 1.1 beta 1',
            'scope': 'source routes and audited gaps; not behavioral coverage',
            'manual_sha256': hashlib.sha256(doc.read_bytes()).hexdigest(),
            'source_sha256': {p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in SOURCES},
            'summary': {'manual': len(names),
                        'registered': sum(row['registered'] for row in rows),
                        'typed': sum(row['parameter_route'] == 'typed' for row in rows),
                        'fallback': sum(row['parameter_route'] == 'fallback' for row in rows),
                        'missing': [row['name'] for row in rows if not row['registered']],
                        'consumer_status_counts': counts,
                        'extra_registered_aliases': sorted(set(registered)-set(names)),
                        'observed_assertspecial_consumers': sorted(flags)},
            'controllers': rows}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--write', action='store_true')
    args = p.parse_args()
    value = inventory()
    encoded = json.dumps(value, indent=2) + '\n'
    path = ROOT/'docs/CONTROLLER-BINDINGS.json'
    if args.write:
        path.write_text(encoded)
    elif not path.exists() or path.read_text() != encoded:
        raise SystemExit('Controller inventory differs; review source then regenerate with --write')
    print(json.dumps(value['summary']))


if __name__ == '__main__':
    main()
