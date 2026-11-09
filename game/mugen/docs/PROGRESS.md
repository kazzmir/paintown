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
- [x] **SF2 selection and legacy character loading**: Saved roster handoff and an independent automatic opponent fix the reported selection failure. Authored layout/cursors/portrait colors and legacy font/CNS forms load. All 12 roster entries compile 1,743 state programs; Linux Arcade Ryu/Dhalsim and Guile/Honda fights run with input, HUD and audio. The four boss slots are also offscreen in the native pack. Full roster gameplay remains unverified.
- [x] **Rendered selection/VS and legacy bounce recovery**: Corrected character-to-motif portrait scaling and authored VS portraits, windows, names and artwork. Connected checks now inspect pixels and cover omitted-constant bounce → lying down → get-up → control. All five segments pass with races; Linux stock/SF2 screens and a 60-second Honda/Dhalsim run were inspected.
- [x] **SF2 later-round initialization script repairs**: The native beta also stalls with the supplied scripts that omit later-round state5900 exits. The playable-copy tool now discovers all ten matching scripts instead of repairing only Ryu/Dhalsim. --update extends existing copies while preserving configuration and local edits. All ten initialization programs exit on round two; full Guile/Chun-Li intro → KO/reset → round-two control → walking passed. The retained working copy is updated; original pack and engine state semantics are preserved.
- [x] **Crossover command and walking turns**: Matched supplied Elecbyte behavior: command evaluation sees the old heading, controlled ground state entry sees the opponent-facing heading and authored velocity, and walking uses turn action 5 without resetting its state clock. The connected flow covers NoAutoTurn release, physical input and stage-disabled turning. Locked attacks retain facing. Linux Ryu/Dhalsim jump-over and continued input were inspected; all segments are checked with races.
- [x] **Stable held guard and movement exits**: Held guard retains its state clock instead of falling through into ordinary walking/crouching. Native beta confirms Ctrl=1 during standing/crouching guard, immediate forward movement on release and up-back jumping. The existing mixed-coordinate combat scenario continues through posture changes, a real blocked punch/recovery, re-guard, release and jump/landing. Four changed-source segments pass with races; state-flow stays checked. Linux rebuilt and visible guard/posture/release inspected; diagnostic scratch removed.
- [x] **Visible incoming character-intro effects**: Explod creation accepts the legacy velocity spelling used by stock KFM. The incoming block now moves onscreen before impact, followed by the broken pieces. Native beta and rebuilt Linux pre-impact scenes were inspected. The existing title-to-match scenario checks actual incoming-block pixels, break, control and complete rounds; all five connected segments pass with races. Diagnostic scratch removed.
- [x] **Borrowed-state effect creation context**: Explods created in an opponent’s state retain the creating coordinate profile for offsets, motion and private-sprite scale while using the issuer’s banks. Deferred creation preserves animation/admission gates. The connected mixed-coordinate entity flow continues through Pause, SelfState, expiry and round reset. All five affected segments pass with races; rebuilt Linux position and sprite dimensions agree with the beta scene. The related guard-distance observation matched existing behavior. Scratch removed; no mod scripts changed.
- [x] **Automatic air-jump state entry and budget**: The engine now selects common state45 from fresh airborne up input, honoring control, minimum height and the per-flight limit. Low/held input cannot activate later at height. Common CNS owns directional velocity and animation; grounded S/C restores the budget before guard can return early. The mixed-coordinate jump/body/Pause/reset scenario spans two flights. Four changed-source segments pass with races; state-flow stays checked. Native and rebuilt Linux keyboard flows each accepted two air jumps and returned to idle. Scratch removed.
- [x] **Declared button commands do not auto-repeat through raw fallback**: Character.Command now honors a declared label’s matched window and cannot reactivate it from a held raw button. The existing player/helper command lifecycle covers attack/recovery, release/repress, explicit held/released input and authored buffering before stock AI combat. Four affected segments pass with races; state-flow stays checked. Native beta and rebuilt Linux stock KFM each attack once while held and a second time after repress. Scratch removed; no user configuration or mod scripts changed.
- [x] **Base combat stats and life-adjustment scaling**: HitDef now combines base attack/defence with runtime adjustments. Negative, nonabsolute TargetLifeAdd/LifeAdd share defence reductions, including SuperPause; healing and absolute additions stay unscaled. Native TargetLifeAdd ignores issuer attack despite its manual. The existing mixed-coordinate combat scenario matches twelve native observations, then completes recovery and round reset. All five segments are checked with races; rebuilt Linux reproduces the same twelve values. Scratch removed; no mod scripts or user configuration changed.
- [x] **Guarding against independently moving attacks**: InGuardDist includes opposing helpers and live projectiles after their owner returns to idle. Projectile defaults use size.proj.attack.dist; explicit guard.dist retains precedence and coordinate units. Native approach/pass/retirement observations and owned-keyboard fireball blocking agree with rebuilt Linux: 5 chip damage and a guard event rather than a hit. The existing mixed-coordinate combat scenario continues through recovery, damage adjustments and round reset. Four changed-source segments pass with races; state-flow stays checked. Shared assets were reused and diagnostic scratch removed.
- [x] **Runtime numeric fallback through state recovery**: Confirmed beta zero fallback for ordinary invalid numeric results in position/velocity controllers, life/control/power updates, state transitions, and entry ctrl/velset. Argument evaluation continues; omitted attributes retain their defaults. Genuine VM failures still return source-context errors without partly applying a vector. The existing combat scenario spans movement, life/power, same-tick entry, SelfState recovery and fatal diagnostics. All five connected segments pass with races; rebuilt Linux matches all twelve native observations. Shared assets were reused and diagnostic scratch removed.

## Completed scenario coverage

Completed segments are excluded from default runs. Only a change to their
covered source, a reported regression, or an explicit rerun reopens them.
Checkmarks record successful runs against the covered source fingerprint;
source changes reopen only the affected segments.

- [x] **state-flow**: entry → negative states → chained transitions → persistence/loop diagnostics.
- [x] **combat**: logical input → attack/contact → damage/stat scaling/life adjustments/runtime fallback/recovery/reset → held/projectile/helper guard/release → jump/air-jump budget/landing/reset → sweep/legacy bounce/recovery.
- [x] **entities**: helper creation → self binding/Pause/body contact → retirement/orphan effects → hit-paused throw/recovery → borrowed-state effect/Pause/SelfState/expiry → round identity reset.
- [x] **effects-pause**: Pause/SuperPause → actor/background/effect clocks → resources/power → reset.
- [x] **match-flow**: title/selection → rendered VS → stock input-driven fight → KO/time-over → results → rematch/exit.

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
- [ ] Complete font, media and asset compatibility. **partial** — Broader engine font, media and asset acceptance remains unverified. Physical CD playback is unavailable; absent licensed fonts use a reported fallback. Arbitrary content compatibility and raster parity are not established. Script defects that also reproduce in native are content findings, not engine implementation tasks; no further SF2 mod repairs are planned. (item4)
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
