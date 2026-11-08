# Project progress

This is the single completion ledger for the Go MUGEN 1.1 beta port.
Checked items have a working implementation for the stated scope. Partial
items name the remaining work. Counts below are feature/route counts, not
an estimate of total effort or complete native compatibility.

- Controllers: 89/89 routes; 0 known partial, 0 missing.
- Triggers: 127/127 runtime routes; 0 missing.
- Project items: 32 implemented, 2 partial, 0 missing.

## Known difference checkpoints

These checkmarks close the documented discrepancies for the observed scope.
Broader compatibility and deferred capabilities remain listed below.

- [x] **Offscreen camera return**: Unclipped participant midpoint fixes the mixed-return tick; all eleven beta camera/zoom samples match through time 120.
- [x] **Indexed sprite and shadow alignment**: Fixed 3/8 game-pixel calibration and unsnapped addressing reproduce the bounded beta rotated-sprite/shadow RGB crops exactly in the Linux application.
- [x] **Finite and overlapping alpha composition**: Delta-first byte interpolation and foreground-over-HUD composition reproduce the bounded RGBA overlap crop exactly. Connected AIR transitions, pause/expiry, letterboxing and final global PalFX pass.
- [x] **Ordinary KO echo playback**: Native and Go device captures both pass the existing frozen seven-segment acceptance, including the formerly missing quiet interval. All three Go cue levels match exactly; maximum relative-start difference is 0.05 seconds, within the unchanged 0.35 second device-capture criterion. The historical mismatch no longer reproduces; no scheduler change was needed.

## Completed scenario coverage

Completed segments are excluded from default runs. Only a change to their
covered source, a reported regression, or an explicit rerun reopens them.
Checkmarks record successful runs against the covered source fingerprint;
source changes reopen only the affected segments.

- [x] **state-flow**: entry → negative states → chained transitions → persistence/loop diagnostics.
- [x] **combat**: logical input → attack/contact → damage/state continuation; jump → CNS landing recovery.
- [x] **entities**: helper creation → self binding/Pause/body contact → retirement/orphan effects → hit-paused throw/recovery → round identity reset.
- [x] **effects-pause**: Pause/SuperPause → actor/background/effect clocks → resources/power → reset.
- [x] **match-flow**: title/selection → stock input-driven fight → KO/time-over → results → rematch/exit.

## Project breakdown

### State machine

- [x] State loading, compilation and source diagnostics.
- [x] Typed expressions, variables and ordered evaluation.
- [x] Trigger groups and short circuit evaluation.
- [x] State entry attributes, time and control.
- [x] Negative states and same-tick state continuation.
- [x] Controller persistence and hit-pause eligibility.
- [x] Borrowed states, animation and SelfState ownership.
- [x] Remaining variable and StateTypeSet schemas.
- [x] Remaining stage, viewport and team trigger bindings. (item4 context)

### Characters and combat

- [x] Root/helper update, collision, physics and clocks.
- [x] Helper creation, identity, retirement and round cleanup.
- [x] Targets and parent/root/target binding.
- [x] Hit/guard contacts, damage and reaction state transitions.
- [x] Hit, fall, landing and recovery state progression.
- [x] Death latching, KO and damage grace.
- [x] Combo count lifecycle and HUD consumer.
- [x] Cold-start screen constraint ordering.

### Controllers and effects

- [x] Pause/SuperPause, movement allowances and clock ownership.
- [x] Projectiles, Explods, trails, palettes and transforms.
- [x] Formatted clipboard and production debug display.

### Match and presentation

- [x] One-on-one rounds, results, reset and rematch.
- [x] Title, player/stage selection, versus and fight handoff.
- [x] One-on-one Round/Fight/Win presentation scheduling.
- [x] Storyboard loading, playback and canvas scaling.
- [x] Stage cameras, moving floors, linked/animated parallax and reflections. (1.1 functional gap (resumed audit))

### Assets and sound

- [x] Stock character/stage, SFF/AIR/CNS/CMD loading.
- [x] Sound effects, channels and production mixing.
- [x] Background music and NoMusic integration. (item4)
- [ ] Complete font, media and asset compatibility. **partial** — Physical CD (.da) playback remains unavailable. ZIP character packs, streamed integer/float WAV and other file music, fonts, storyboard/AIR consumers and legacy ACT outfits/portraits work for the connected scenarios. Missing licensed fonts use a reported fallback; legacy-font native equivalence, native asset-rejection and exact raster parity remain unproven. (item4)
- [ ] Remaining native rendering, audio and AI fidelity. **partial** — Documented camera, indexed sprite/shadow, finite/overlap alpha and ordinary KO-echo discrepancies are closed for the checked scope. Broader pause/hitpause, bindings, crop/raster/filtering combinations, device-level sample timing and native AI policy remain unverified compatibility work. (compatibility next; known-difference batch complete (user 2026-11-08))

### Modes and platforms

- [x] Opponent AI and authored CMD command flow. (item4)
- [x] Arcade/survival progression, continue/ending and training/watch rules. (item4)
- [x] Team/simul/turns lifecycle, partner context and recovery. (item4)
- [x] Linux build and final playable acceptance. (Linux now; other targets deferred (user 2026-11-07))

## Controllers and trigger gaps

All 89 documented controller families are registered. The source check
flags concrete missing/partial consumers below. A route alone does not
prove every parameter combination; untested permutations are not new
implementation tasks by themselves.

| Controller | Remaining implementation |
| --- | --- |

Missing trigger routes: none.

Stage, viewport and team runtime context are implemented; remaining work is listed above.

## How progress is updated

1. Pick an unchecked feature and implement a coherent batch.
2. Update its status in `STATE-MACHINE-FEATURES.json`.
3. Validate the affected connected scenario at the end of the batch.
4. Record its checkmark here, commit source/text, and remove scratch output.

Use `mugen.sh --coverage --write` to refresh this page and
`mugen.sh --check` to run pending/reopened segments only. Explicit
`--check --segment NAME` reruns one segment. No reference executable,
asset copies or capture matrices are needed for the coverage report.

The check detects removed source entry points, uncatalogued controllers
and missing trigger bindings. Its reviewed feature catalog is a backlog,
not a proof of arbitrary semantics. Full historical diagnostics are opt-in
and are not completion gates for unrelated work.

See [workflow](WORKFLOW.md), [implementation plan](ITEM-3-PLAN.md),
[test segments](../tests/SEGMENTS.md) and [handoff](HANDOFF.md).
