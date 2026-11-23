# Project progress

This is the single completion ledger for the Go MUGEN 1.1 beta port.
Checked items have a working implementation for the stated scope. Partial
items name the remaining work. Counts below are feature/route counts, not
an estimate of total effort or complete native compatibility.

- Controllers: 89/89 routes; 0 known partial, 0 missing.
- Triggers: 127/127 runtime routes; 0 missing.
- Project categories: 32 implemented, 3 partial, 0 missing.
- Remaining-scope implementation targets: 2 closed, 2 open.
- Separate acceptance followups: 0 passed, 6 open.

## Remaining implementation and acceptance

Linux 1.1 beta 1 remaining-scope baseline from documentation, production consumers and bounded reference observation. Implementation targets and acceptance/evidence followups are separate. Scope corrections are explicit below; closure applies to the named connected behavior, not exhaustive semantic or whole-engine conformance.

This is a baseline of remaining work, not a reset of completed features.
Targets close only after implementation and their named acceptance pass.
Sizes are planning classes; confidence applies to the estimate, not proof
of native equivalence. Unknown behavior is not counted as a code defect.

| Target | Implementation / acceptance | Size / confidence |
| --- | --- | --- |
| **K1 — Result text composition and fitted viewport** | implemented / passed | small / high |
| **K2 — Fresh result scene and Survival pose policy** | implemented / passed | medium / high |
| **K3 — AI.Cheat command admission policy** | missing / unverified | large / low |
| **K4 — Legacy joystick configuration consumer** | missing / unverified | medium / low |

- [x] **K1**: Preserve the beta's observed front-composed result captions and project them through the game viewport.
  Basis: Native Win/Survival overlap regions are identical for authored layerno 0/1/2: all draw in front. The missing layer consumer was an incorrect source-only inference. Native text uses game-viewport fitting; the old port drew directly in the motif canvas.
  Prerequisite: Resolved by bounded native Win/Survival single/co-op scenes; connected match-flow acceptance passed with race detection.
  Completion: The campaign/result lifecycle checks caption color/fit/letterboxing, visibility and fades without altering the committed fight. Match-flow acceptance closes the target.

- [x] **K2**: Create separate restored human actors for Win/Survival results; choose state170 below the Survival threshold and state180 at/above it.
  Basis: Native resets presentation actors to full base life, RoundNo1/RoundState0, zero Win/enemy context and cleared variables. Single centers; a co-op pair faces inward at -50/+50 base units. State5900 controllers are not replayed. The old port resumed the defeated match and retained opponents/HUD.
  Prerequisite: Resolved by bounded native Win/Survival single/co-op scenes; connected match-flow acceptance passed with race detection.
  Completion: The existing mode lifecycle checks separate world/actor/palette ownership over shared banks, both threshold outcomes, restored partners in mixed coordinates, no enemies/HUD, ongoing animation, clean exit and unchanged committed fight state. Match-flow acceptance closes the target.

- [ ] **K3**: Connect AI.Cheat to AI command admission while retaining ordinary CMD/CNS eligibility when disabled.
  Basis: The target CFG describes optional command activation without simulated key presses. AICheat is parsed but has no runtime reads; ai.go always emits timed physical-input plans.
  Prerequisite: Establish enabled/disabled admission, charge handling and helper/AI ownership with a bounded target comparison. Do not invent the bypass policy; random move selection is separate U4 work.
  Completion: One AI command-to-contact/recovery lifecycle distinguishes enabled/disabled policy, preserves human/helper recognition and CNS eligibility, and resets cleanly. Validate affected existing segments at batch end.

- [ ] **K4**: Honor legacy P1/P2.Joystick.type and bindings through explicit native-code translation, retaining YAML/session override precedence.
  Basis: Target CFG/readme expose joystick enable settings and numeric bindings. Joystick.type is ignored; stored P1/P2Joystick fields have no adapter reads. Portable/raw YAML controls are implemented separately.
  Prerequisite: Establish the target code namespace and platform/device mapping. Native numbers are not SDK indices; physical confirmation belongs to H1.
  Completion: A configuration-to-menu-to-combat/reconnect flow honors legacy enable/bindings and explicit overrides, preserves keyboard alternatives/seat ownership, and leaves YAML untouched. Validate translation in match-flow, then H1 on hardware.


### Separate acceptance and evidence questions

These followups are separate from each implementation target's own
acceptance. Open questions do not imply missing controller/trigger routes.

| ID | Scope / status | Evidence needed to close it |
| --- | --- | --- |
| **U1** | Additional runtime interaction fidelity — unverified | Reuse existing pause/binding/facing/reset findings. For broader acceptance, compare one overlapping pause/hitpause, bound-anchor turn, release/recovery episode. Create a code target only for a demonstrated mismatch. Prerequisite: A precise target observation for the combined lifecycle; this review found no additional missing consumer. |
| **U2** | Broader font/content acceptance — unverified | Compare one localized asset/storyboard/menu/fight/reload flow with licensed Unicode/collection fonts and mixed asset families. Record real parser/render defects, separating missing payloads and native-reproducing content defects. Prerequisite: Suitable licensed font/content input and target output. TTF/TTC readers exist; fallback glyph coverage and metrics do not prove parity. |
| **U3** | Native Options/training behavior and presentation — unverified | Compare one menu-to-training flow covering setting effects, dummy ownership/guard/stance/distance/jam, pause/resume and return. Preserve intentional session-only edits; quantify any mismatch before scheduling code. Prerequisite: Operate the relevant Options/training controls and compare their effects. Current native observations cover campaign/results, not native Options/training acceptance. |
| **U4** | Native AI decisions beyond AI.Cheat — unverified | State observable target decision rules and permitted variability separately from K3 admission. Successful AI combat does not establish matching native probabilities. Prerequisite: Bounded reference evidence sufficient for a finite decision contract; exact stochastic equivalence has no reliable estimate. |
| **U5** | Additional raster/device-audio precision — unverified | Keep calibrated camera, sprite/shadow, alpha and ordinary KO-echo findings closed. Name a remaining discrepancy and tolerance before a targeted comparison; generic combinations are not code tasks. Prerequisite: A specific mismatch or explicitly scoped precision target. Whole-frame/sample-exact equivalence remains unverified. |
| **H1** | Physical Linux controller acceptance — awaiting-prerequisite | Exercise menu/configuration, selection, combat and unplug/reconnect on hardware; preserve ownership, neutral/deadzone and held/edge behavior. Include native mapping after K4. Injected devices cannot close H1. Prerequisite: An available controller and someone able to exercise it; no connected device is assumed. |

### Bounded assessment findings

The documentation/source review is complete for these boundaries.
Existing evidence was reused; no new native or gameplay runs are inferred.

| Review | Finding | Allocation |
| --- | --- | --- |
| **A1 — Runtime ordering** | Shared timeline, actor/helper ordering, pause allowances, binding, facing and reset have consumers and scoped connected/native evidence. No new missing consumer was established; broader interaction acceptance remains explicit. | U1 |
| **A2 — Assets, fonts and media** | Text/AIR, SFF v1/ACT/PCX, SFF v2 raw/indexed/PNG, FNT v1/v2 bitmap, TTF/TTC, SND/WAV, BGM and archive/resource flows have readers and scoped evidence. Bitmap v2 ASCII limits are documented target behavior. No new missing format family was established. | U2 |
| **A3 — Rendering and audio** | Reference observation corrected the layerno inference: tested result values 0/1/2 all draw in front. Actual gaps were caption viewport projection and fresh result-scene/pose policy. Other calibrated raster/audio findings stay closed. | K1, K2, U5 |
| **A4 — Options, training and AI** | Displayed difficulty/life/time/speed/SFX/BGM/input settings reach consumers. Training/readme and runtime debug controls have functional paths and connected evidence. AI.Cheat has no runtime reader. Native UI/dummy and stochastic AI parity remain unverified. | K3, U3, U4 |
| **A5 — Input compatibility** | Standard/raw gamepads, keyboard alternatives, session editing and ownership/hotplug have injected-device evidence. Legacy enable settings/numeric bindings do not reach the adapter. Physical/native numbering acceptance is separate. | K4, H1 |

### Deferred capabilities

- Physical CD playback.
- Unavailable licensed/CJK font payloads; the Unicode reader exists, but absent fonts are not supplied by the engine.
- Other-platform runtime acceptance.
- Standalone music-file CLI playback.

New discoveries must be recorded as explicit scope changes with evidence.
They do not silently enlarge this baseline or reopen unrelated completion.

- K1 corrected by direct target observation: absent layerno reads did not prove a behavioral defect. Tested Win/Survival values 0/1/2 share front composition; K1 now covers that policy and the demonstrated viewport-projection gap.
- K2 expanded within the results batch: target results initialize a separate human-only presentation, including restored life/context, zeroed variables and partner placement. Continuing completed-fight actors was incorrect. This supersedes the earlier result-clock implementation without reopening unrelated mechanics.

## Known difference checkpoints

These checkmarks close the documented discrepancies for the observed scope.
Broader compatibility and deferred capabilities remain listed below.
Scope and validation details remain in the [feature catalog](STATE-MACHINE-FEATURES.json)
and Git history; this checklist records completion without repeating run logs.

- [x] **Offscreen camera return**
- [x] **Indexed sprite and shadow alignment**
- [x] **Finite and overlapping alpha composition**
- [x] **Ordinary KO echo playback**
- [x] **Legacy selection and character loading**
- [x] **Rendered selection/VS and legacy bounce recovery**
- [x] **Legacy later-round initialization diagnosis**
- [x] **Crossover command and walking turns**
- [x] **Stable held guard and movement exits**
- [x] **Visible incoming character-intro effects**
- [x] **Borrowed-state effect creation context**
- [x] **Automatic air-jump state entry and budget**
- [x] **Declared button commands do not auto-repeat through raw fallback**
- [x] **Base combat stats and life-adjustment scaling**
- [x] **Guarding against independently moving attacks**
- [x] **Runtime numeric fallback through state recovery**
- [x] **Animation selection and duplicate AIR compatibility**
- [x] **Held chords, release and command-buffer compatibility**
- [x] **Character and motif definition precedence**
- [x] **Language selection across definition assets**
- [x] **Target selection and parent-variable expression admission**
- [x] **Helper-bank admission before parameter evaluation**
- [x] **Authored long charge and motion history**
- [x] **Borrowed helper context through deferred admission**
- [x] **Timed AI plans for authored long commands**
- [x] **Variable register and range write admission**
- [x] **Pause and SuperPause numeric fallback through recovery**
- [x] **CMD defaults, duplicate settings and loading contract**
- [x] **State entry and persistence first-value precedence**
- [x] **Scalar control/effect fallback and minimum life adjustments**
- [x] **Target numeric fallback after relationship and ID admission**
- [x] **Protection, sound and palette fallback through contact recovery**
- [x] **HitOverride slot admission, clearing and reaction recovery**
- [x] **Nested VM failures stop evaluation before partial writes**
- [x] **Legacy and v2 font definition precedence**
- [x] **Stage, background and fight layout field precedence**
- [x] **Motif and storyboard declaration admission**
- [x] **Storyboard presentation inside a widescreen motif**
- [x] **Escaped quote admission in state debug text**
- [x] **AI plans for charged chords and strict command steps**
- [x] **Authored victory background clear color**
- [x] **Recovery constant dimensions across borrowed and redirected states**
- [x] **First character storyboards and portrait sprites**
- [x] **Roster entry presentation through selection handoff**
- [x] **Localized roster limits through Arcade scheduling**
- [x] **Idle title demo and intro cycling**
- [x] **Authored TrueType font height overrides**
- [x] **Legacy configuration first-field admission before YAML overrides**
- [x] **Shadow and quick-versus configuration consumers**
- [x] **Background simulation and audio policy**
- [x] **Portable gamepad input through menus and combat**
- [x] **Player keyboard enable settings and controller-only co-op**
- [x] **User pause, frame-step and training dummy controls**
- [x] **Fight debug life, AI, restart/reload and presentation controls**
- [x] **Options input configuration through gameplay**
- [x] **Options layout and font consumers in HD motifs**
- [x] **Direct training, watch and partner match entry**
- [x] **High-resolution screenpack and palette compatibility**
- [x] **Sparse HUD components and legacy text admission**
- [x] **Debug player activation and target PNG screenshot format**
- [x] **Usable Options in artwork-only menu motifs**
- [x] **Legacy projectile syntax and empty effect lifecycle**
- [x] **Lazy character and campaign storyboard admission**
- [x] **Victory portrait window in motif coordinates**
- [x] **Authored result and campaign font colors**
- [x] **Victory quote wrapping within its authored anchor**
- [x] **Victory fades through campaign handoff**
- [x] **Menu selection and versus font color consumers**
- [x] **HUD font color and optional tuple components**
- [x] **Configured campaign and survival result screens**
- [x] **Separate Win/Survival result scene lifecycle**
- [x] **Automatic campaign stages without a manual stage pool**

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
- [ ] Complete font, media and asset compatibility. **partial** — Reviewed format families have readers and scoped acceptance. Broader font/content acceptance is U2; missing licensed payloads and native-reproducing content defects are not missing consumers. Physical CD and unavailable licensed/CJK payloads remain deferred. (item4)
- [ ] Remaining native runtime, rendering, audio and AI fidelity. **partial** — K1 result composition/viewport and K2 fresh result/threshold scenes are closed for the checked scope. K3 AI.Cheat command policy remains missing. U1/U3/U4/U5 retain runtime, UI/dummy, stochastic AI and precision uncertainty; calibrated camera, sprite/shadow, alpha and ordinary KO-echo findings remain closed. (compatibility next; known-difference batch complete (user 2026-11-08))

### Modes and platforms

- [ ] Player input devices and configuration. **partial** — Portable/raw gamepad and keyboard software paths are implemented. K4 is the missing legacy enable/numeric-binding consumer; H1 is physical Linux acceptance. Establish native mappings before translation; injected devices cannot prove hardware/native compatibility. (item4)
- [x] Opponent AI and authored CMD command flow. (item4)
- [x] Arcade/survival/demo progression, continue/ending and training/watch rules. (item4)
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

1. Pick a scoped implementation target and resolve its prerequisites.
2. Implement a coherent batch and update implementation status in `STATE-MACHINE-FEATURES.json`.
3. Validate the affected connected scenario at the end of the batch.
4. Record acceptance separately; close the target only when both pass.
5. Refresh this ledger, commit source/text, and remove scratch output.

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
