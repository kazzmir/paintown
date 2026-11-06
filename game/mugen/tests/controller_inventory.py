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
SOURCES = ['engine/dust.go', 'character/dust.go', 'engine/game_animation.go', 'character/game_animation.go', 'character/ko_sound.go', 'character/life.go', 'character/power.go', 'character/hit_damage.go', 'character/reversal.go', 'character/loader.go', 'evaluator/validate.go', 'engine/assert_special.go', 'engine/projectile.go', 'character/projectile.go', 'character/hit_chain.go', 'engine/after_image.go', 'character/after_image.go', 'character/retire.go', 'config/config.go', 'match/match.go', 'cns/hit_shake.go', 'engine/statemachine.go', 'engine/parameters.go',
           'character/character.go', 'character/player.go',
           'character/explod.go', 'engine/explod.go', 'engine/modify_explod.go',
           'effect/scene.go', 'character/actor_order.go', 'system/effects.go', 'render/perspective.go',
           'render/shadow.go', 'stage/loader.go', 'render/alpha.go', 'render/nearest.go', 'render/linear.go',
           'engine/drawing.go', 'engine/trans.go', 'engine/palette_fx.go', 'engine/env_color.go', 'engine/env_shake.go', 'engine/shake.go', 'engine/timeline.go', 'engine/round_clock.go',
           'palette/state.go', 'palette/controller_effect.go', 'palette/hit_effect.go',
           'render/palette.go', 'character/helper.go', 'character/tick.go',
           'background/background.go', 'stage/stage.go', 'font/font.go', 'system/fight_state.go', 'system/hud.go',
           'engine/self_binding.go', 'engine/target_self_binding.go', 'character/binding.go', 'character/targets.go', 'character/world.go']
STUBS = {}
RENDER_FIELDS = {}
PARTIAL = {
	'modifyexplod': 'MODIFY-EXPLOD.md: native mutation/schema consumers; bounded shared alpha now matches; EFFECT-ORDERING.md: measured actor/effect priority and ties consumed; SHADOWS.md adds shared shadow drawing; MODIFY-EXPLOD-ENUMS.md: independently measured quoted modes skip alpha compilation/evaluation and retain drawing; broader pause/projection variants and unwaived raster gates remain',
	'varset': 'native literal-float integer rejection found in M4cj setup diagnostic; residual schema closure pending',
    'projectile': 'PROJECTILE-TRAILS.md: typed nested schemas, ordered admission, global context ownership/release, saved palettes and phase/pause history consumers; SHADOWS.md adds measured current-only stage shadows, alpha/fade/geometry/flags/layer consumers; wider resource/profile variants and unwaived raster differences remain',
    'explod': 'EXPLOD-ATTACHMENT.md: native entity/legacy binding, motion and clocks; EFFECT-ORDERING.md: measured priorities/ties/creation order consumed; SHADOWS.md adds typed creation shadow validation/evaluation/order and drawing; EXPLOD-CREATION-PALETTES.md and EXPLOD-PALETTE-CONTEXTS.md add typed creation ownership/remapping, common/localFX separation and mapping replacement; EXPLOD-CREATION-DRAWING.md adds typed creation angles/trans/alpha and conditional priority evaluation; EXPLOD-CAPACITY-RANDOM.md adds typed ordered random displacements, global root/helper/retired-owner and hitspark admission, zero-range draw phases and config binding; broader variants and strict sampling differences remain',
    'assertspecial': 'ASSERT-SPECIAL.md: native literal schema including accepted undocumented noko, global background/foreground/bar consumers and per-game-tick reset under frozen Pause/SuperPause; ASSERT-SPECIAL-CONTROLS.md adds native posture-specific automatic guard entry, scripted-guard preservation, attacker unguardable and nowalk entry with owned keyboard replay/render evidence; NO-KO.md adds measured per-entity non-reviving death latches, current-pass global NoKO assertions, self-KO entry/corpse-contact and paused-KO lifecycle; NO-KO-AUDIO.md adds measured committed-death KOvoice/echo gating with contact-independent channel0 interruption and native/private-application PCM; broader helper/projectile/pause/music/round variants remain; SHADOWS.md adds shared shadow suppression',
    'appendtoclipboard': 'stores raw text; params/formatting absent',
    'displaytoclipboard': 'stores raw text; params/formatting absent',
    'hitadd': 'updates ComboCount; no production reader of that field was found',
    'statetypeset': 'fallback literal handling; full schema/version/error matrix open',
}
BOUNDED = {
    'makedust': 'SYSTEM-EFFECTS.md: typed measured int/float tuple schemas, spacing/aggregate round-phase clock, axis-interleaved RNG, common120 producer, ordinary shared pool/noNumExplod, helper retirement, facing offset without sprite flip, palette/layer/pause/lifetime consumers;166 native programs/1728 values observed, three shared early-camera numeric failures and early paused-pose world differences remain strict; broader camera/KO-slowdown/resource/round variants remain',
    'gamemakeanim': 'SYSTEM-EFFECTS.md: typed native schema/defaults/error/order, split displacement RNG before late under, common-resource production cues, caller/parameter profile math, helper/slot2, facing/global-unit random offsets, Pause/SuperPause-independent animation clock and under-actor drawing; 84 native programs/900 values pass,70 of72 strict world observers exact with two fractional color failures unwaived; independent admission, negative action, sub-game-pixel quantization and broader ownership/palette/foreground variants remain',
    'forcefeedback': 'FORCE-FEEDBACK.md: exact supplied beta compiler lowers this name to Null;18 native programs prove ignored parameters/ordinary triggers across1.0/1.1 profiles; no host/device dispatch',
    'afterimage': 'AFTERIMAGE.md: typed native schema/order, recording/palette/render and global root/helper context admission/lifetime; PROJECTILE-TRAILS.md adds projectile admission/lifetime/palette/phase coverage; broader resource/profile variants open',
    'afterimagetime': 'AFTERIMAGE.md: native canonical/alias schema, missing-context evaluation gate and retained history/expiry/resume consumer; wider variants open',
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
    # Global flags use a timeline sink rather than a persistent actor-map read.
    for flag, field in [('globalnoshadow', 'GlobalNoShadow'), ('nobg', 'NoBackground'),
                        ('nofg', 'NoForeground'), ('nobardisplay', 'NoBarDisplay'), ('noko', 'NoKO')]:
        issuer = 'case "' + flag + '":' in sources['character/character.go']
        sink = 'm.Timeline.' + field
        if issuer and any(sink in sources[path] for path in ['system/fight_state.go', 'system/hud.go', 'match/match.go']):
            flags.add(flag)
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
