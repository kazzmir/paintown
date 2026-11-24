# Project progress

This is the single completion ledger for the Go MUGEN 1.1 beta port.
Checked items have a working implementation for the stated scope. Partial
items name the remaining work. Counts below are feature/route counts, not
an estimate of total effort or complete native compatibility.

- Controllers: 89/89 routes; 0 known partial, 0 missing.
- Triggers: 127/127 runtime routes; 0 missing.
- Project categories: 33 implemented, 2 partial, 0 missing.
- Remaining-scope implementation targets: 8 closed, 1 open.
- Separate acceptance followups: 3 passed, 3 open.

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
| **K3 — AI.Cheat command admission policy** | implemented / passed | large / high |
| **K4 — Legacy joystick configuration consumer** | implemented / passed | medium / high |
| **K5 — Options behavior and utility presentation** | implemented / passed | medium / high |
| **K6 — Training dummy policy and menu lifecycle** | implemented / passed | medium / high |
| **K7 — Overlapping pause, contact entry and bound recovery** | implemented / passed | medium / high |
| **K8 — Native AI input decision policy** | partial / unverified | large / medium |
| **K9 — Authored character palette keymaps through selection** | implemented / passed | medium / high |

- [x] **K1**: Preserve the beta's observed front-composed result captions and project them through the game viewport.
  Basis: Native Win/Survival overlap regions are identical for authored layerno 0/1/2: all draw in front. The missing layer consumer was an incorrect source-only inference. Native text uses game-viewport fitting; the old port drew directly in the motif canvas.
  Prerequisite: Resolved by bounded native Win/Survival single/co-op scenes; connected match-flow acceptance passed with race detection.
  Completion: The campaign/result lifecycle checks caption color/fit/letterboxing, visibility and fades without altering the committed fight. Match-flow acceptance closes the target.

- [x] **K2**: Create separate restored human actors for Win/Survival results; choose state170 below the Survival threshold and state180 at/above it.
  Basis: Native resets presentation actors to full base life, RoundNo1/RoundState0, zero Win/enemy context and cleared variables. Single centers; a co-op pair faces inward at -50/+50 base units. State5900 controllers are not replayed. The old port resumed the defeated match and retained opponents/HUD.
  Prerequisite: Resolved by bounded native Win/Survival single/co-op scenes; connected match-flow acceptance passed with race detection.
  Completion: The existing mode lifecycle checks separate world/actor/palette ownership over shared banks, both threshold outcomes, restored partners in mixed coordinates, no enemies/HUD, ongoing animation, clean exit and unchanged committed fight state. Match-flow acceptance closes the target.

- [x] **K3**: Connect AI.Cheat to AI command admission while retaining ordinary CMD/CNS eligibility when disabled.
  Basis: Target CFG permits AI command input without key presses. Target command-admission code selects declared command indices separately from physical keys and shares them with key-controlled helpers; bounded observation admits a long-charge label before its physical charge can finish.
  Prerequisite: Resolved by target command-admission review and the connected command/combat lifecycle. Native random move-selection probabilities remain U4.
  Completion: AI.Cheat reaches the production match consumer. Disabled mode retains timed CMD input. Enabled mode admits declared labels independently of remaps/physical charge, preserves CNS eligibility and helper/human ownership, and completes contact/recovery and ownership handoff. Affected connected segments pass with race detection.

- [x] **K4**: Honor legacy P1/P2.Joystick.type and bindings through explicit native-code translation, retaining YAML/session override precedence.
  Basis: Target CFG/readme and target executable joystick initialization/event handling establish enable policy, four 32-code device blocks, axis-pair ordering, hat aliases and capability-dependent button offsets. Linux SDK layout review establishes its distinct combined button/hat namespace.
  Prerequisite: Resolved for the current Linux software scope through target-handler review and evdev capability translation matched to SDK device identity/counts. Physical/native calibration remains H1; other-platform legacy capability adapters remain deferred.
  Completion: CFG enable/numeric bindings feed the production input consumer before YAML/session overrides. The existing configuration/menu/stock combat/rematch/reconnect lifecycle passes with race detection, including differing axis/hat counts, disabled input, keyboard alternatives, ownership and unchanged YAML. Hardware acceptance is separate H1.

- [x] **K5**: Complete Options settings, team/input/default actions and bounded utility presentation, retaining session-only load/save.
  Basis: Target Options inspection and handler review establish missing master volume/team/default actions, clamped value admission, discrete time limits, full speed range and independent utility rows. Team KO ordinal zero ignores surviving AI partners after a human leader is KOed; it is not an any-member-KO rule. Team factory reset uses 200% advantage, distinct from the shipped CFG value.
  Prerequisite: Resolved by direct target Options/input/team observation and handler review; portable input editing and session-only persistence remain intentional port policies.
  Completion: The existing Options/input/team/default/session lifecycle checks value bounds, held navigation, master/SFX/BGM gains, keyboard/gamepad snapshots and startup reset, native team factory values and CFG KO policy through gameplay. Affected connected segments pass with race detection; Linux Options/Training flow and resized utility presentation pass without writing user configuration.

- [x] **K6**: Match observed Training mode-specific rows, AI-level ownership, guard/stance/spacing/jam input policy, pause/presentation and selection return.
  Basis: Target Training lifecycle and handler review establish an AI-level row, mode-specific row counts, axis-distance bands, six-tick button toggles, descending-phase air-jump input and return to selection with fresh settings.
  Prerequisite: Resolved by direct target Training menu/stock-fighter observation and cooperative input-handler review. Native stochastic AI decisions remain separate U4.
  Completion: The existing mode lifecycle passes through Manual input, cooperative low guard/contact/recovery/refill, axis spacing, six-on/six-off jam, jump/air-jump/landing, independent AI level, frozen redraws, pause/step/minimize and selection/reentry reset. Affected connected segments pass with race detection; final Linux application flow and paused PNG pixels pass.

- [x] **K7**: Match deferred contact-state entry, ordinary Pause allowance across hitpause and TargetBind expiry during global Pause through anchor turning, release and own-state recovery.
  Basis: One target throw/Pause/hitpause/turn episode establishes deferred external entry, ordinary Pause movetime retained during owner hitpause, and finite TargetBind aging through the victim’s global pause. An unbounded binding in the same lifecycle continues following the allowed anchor, distinguishing expiry from frozen placement. Captured clocks, animation, velocity, facing and positions resolve the consumers.
  Prerequisite: Resolved by direct target observation of the connected throw/Pause/hitpause/turn/recovery lifecycle. A longer pause and an unbounded constraint distinguish allowance consumption and finite expiry; existing helper/SuperPause policies stay separate.
  Completion: The existing entity scenario checks native entry animation/velocity, owner/victim clocks, binding flags, facing and positions through overlapping pause, finite expiry, TargetDrop, SelfState, restored control and round reset. All affected connected segments pass with race detection; all five checkmarks are current. Linux build and owned stock-match application smoke pass without changing user configuration or root module metadata.

- [ ] **K8**: Match the target AI input policy through weighted fallback movement, defence, attack/cancel, press/release cadence, command admission and recovery, plus its learned-move movement/attack/guard planner, while retaining CNS eligibility and human/helper/reset ownership.
  Basis: Reviewed fallback and authored command-table indexing remain implemented. A connected target human/CPU episode confirms successful command checks cause move trials, ground/air banks remain separate, hitpause stops collection time and completed records survive round restart. The port now collects those live episodes with bounded banks, context attribution, attack/body geometry, whiff/guard/hit timing and slowly updated statistics. The three timing groups are contact outcomes, correcting the earlier pose inference. Learned-data loading, prediction/selection and complete planner acceptance remain pending. Compatible optional beta learned-data loading and independent session banks are now connected; missing/incompatible caches fall back to live learning. Reversal active windows also enter the learner. Completed data survives round/rematch/reload while changed source revisions select new profiles.
  Prerequisite: Finish movement/attack/guard prediction and command-plan consumers, then validate the coherent AI batch. Model storage is session-only; no automatic generated binary writes are introduced. Keep individual actors independent and invalidate changed source profiles.
  Completion: The new connected human whiff/air/hit episode matches native punch startup3, last-active6 and recovery12, excludes hitpause, retains outcome histories and five-percent repeat estimates, and keeps completed ground/air records through fresh round actors. Linux compile and the extended existing AI/combat scenario pass. Full segment acceptance is pending the coherent planner batch; K8/U4 stay open. The existing connected scenario also decodes a tiny in-memory native-format record and checks independent model retention through reload. No binary fixture or user configuration is committed. Planner acceptance remains pending. Learned movement/attack/guard consumers now execute through opponent recognition, startup/range selection, pose preparation, normal CMD or admitted labels, predictive guard and Start-triggered waypoints. The connected planner scenario reaches guarded contact and physical counter preparation/contact/recovery; affected segment and final runtime acceptance remain pending.

- [x] **K9**: Carry authored Palette Keymap and availability through character portrait loading, manual/random-cell selection, outfit reservation, roster snapshots, fight PalNo and round/reload ownership. Preserve absent/partial-group defaults, first fields and 1–12 clamping; keep direct numeric palette requests separate from button ordinals.
  Basis: The shared native/Go Unicode font-content journey exposed different outfits from the same stock button. Target loader and reservation-handler review established authored mapping, defaults, clamping, legacy Files availability and used-outfit fallback. The production selection consumer now implements those rules without loading full fight banks for previews.
  Prerequisite: Resolved for the named manual-selection lifecycle by target handler review and one connected custom-map/held-Start/duplicate-fighter/fight/reload comparison. Non-cheating AI palette preference rules are not inferred. Start-only confirmation remains a port convenience, admitted on release so held Start can modify an attack.
  Completion: The existing localized asset/selection/team lifecycles pass with race detection through generated two-face TTC fonts, mapped numeric palettes, random-cell identity, duplicate outfit reservation, unchanged direct numeric requests, actual combat and reload. Native and rebuilt Linux match the custom Start mapping at CNS PalNo 6 before/after reload and retain distinct outfits for repeated picks. Linux build/application inspection passes; owned runtime data is removed.


### Separate acceptance and evidence questions

These followups are separate from each implementation target's own
acceptance. Open questions do not imply missing controller/trigger routes.

| ID | Scope / status | Evidence needed to close it |
| --- | --- | --- |
| **U1** | Additional runtime interaction fidelity — passed | Direct target comparison of one overlapping throw/Pause/hitpause, bound-anchor turn, finite expiry and own-state recovery episode established K7. The corrected production consumers match its clock/entry/pose observations and pass the extended entity-to-round-reset scenario, affected connected segments with race detection and Linux build/application smoke. This closes the named lifecycle, not arbitrary runtime permutations. Prerequisite: Resolved for the named combined lifecycle through direct target observation and the existing connected entity path. Prior binding/facing/reset findings remain intact. |
| **U2** | Broader font/content acceptance — passed | Functional acceptance passes for the supported font/content contract: openly licensed TTF and generated two-face TTC inputs render Latin, Greek and Cyrillic text, honor the first collection face and authored heights, and traverse localized storyboard/menu/selection/VS/fight/rematch/reload alongside bitmap/SFF/ACT/SND resources. The native/Go journey exposed K9 palette selection, now corrected and accepted. The affected connected segment passes with race detection and the rebuilt Linux window flow is inspected. This does not establish arbitrary-pack compatibility or pixel-exact font rasterization; named font precision remains U5. Prerequisite: Resolved with openly licensed module font inputs and the connected native/Go content comparison. Generated collections/captures/runtime overlays are disposable; no font payload or binary fixture is committed. Unavailable licensed/CJK payloads remain deferred. |
| **U3** | Native Options/training behavior and presentation — passed | Direct target menu/Training observation and handler review established K5/K6. Both are implemented and accepted through the existing Options-to-gameplay and Training lifecycles, race-checked connected segments and final Linux window/PNG inspection. Session-only persistence, startup-binding reset and portable input editing are explicit port policies; sample-exact raster/audio and stochastic AI decisions remain separate U5/U4. Prerequisite: Resolved for the named Linux Options/Training behavior and usable utility presentation. Target input/team/default navigation, value admission, dummy modes/policy, pause/step/minimize and return/reentry were exercised or traced to their handlers. |
| **U4** | Native AI decisions beyond AI.Cheat — unverified | K8 weighted fallback contexts pass connected movement/defence/attack/recovery/ownership checks, actual CMD combat, affected race-checked segments and Linux application smoke. Native live move tables and the learned movement/attack/guard planner remain a confirmed implementation gap. Accept that connected planner lifecycle before closing U4; fallback fighting/specials do not establish full native AI equivalence. Authored command-index identity is now resolved through target table inspection and connected parser/AI admission; this prerequisite does not close learned move collection or prediction. Prerequisite: Resolve and implement K8 learned move metadata/planner consumers, then compare one connected target episode with permitted random variability. Preserve the independent-stream and timed non-cheating-plan port policies; do not infer seed-for-seed stochastic equivalence. |
| **U5** | Additional raster/device-audio precision — unverified | Whole-text TrueType layout is corrected: include negative bearings and final overhangs, pad the surface at its left edge, truncate after centering and include the final pixel at a right anchor. The shared target text surface/capture confirms these rules; the existing timed storyboard asset lifecycle checks italic/Unicode extents and aligned/clipped placement. Affected match-flow acceptance passes with race detection; unchanged segments remain checked. The Linux build, shared-font window inspection and stock-match application smoke pass; owned scratch is removed. Glyph hinting remains unresolved, so this does not close U5 or claim whole-font pixel equivalence. Keep earlier camera, sprite/shadow, alpha and ordinary KO-echo findings closed. Prerequisite: The shared openly licensed 24-pixel font scene still isolates hinted glyph-edge/baseline and advance differences. The Go outline renderer rounds advance metrics but does not execute TrueType glyph hint instructions; the system FreeType comparison also differs from the target raster. Resolve a compatible hinting path or an explicitly accepted raster/metric tolerance before claiming closure; do not apply per-glyph or baseline offsets to hide the discrepancy. |
| **H1** | Physical Linux controller acceptance — awaiting-prerequisite | Exercise menu/configuration, selection, combat and unplug/reconnect on hardware; preserve ownership, neutral/deadzone and held/edge behavior. Include native mapping after K4. Injected devices cannot close H1. Prerequisite: An available controller and someone able to exercise it; no connected device is assumed. |

### Bounded assessment findings

The documentation/source review is complete for these boundaries.
The baseline reused existing evidence; subsequent target comparisons and scope changes are recorded above.

| Review | Finding | Allocation |
| --- | --- | --- |
| **A1 — Runtime ordering** | The bounded U1 comparison established K7: external contact entry waits through attacker hitpause, ordinary Pause owner allowance is retained during hitpause, and TargetBind duration advances through global Pause. Those consumers now match the connected target turn/release/recovery trace and pass entity/round-reset acceptance. Existing helper, facing, binding and SuperPause findings remain closed for their stated scope. | K7, U1 |
| **A2 — Assets, fonts and media** | Supported format families have connected functional acceptance. U2 now passes a licensed Unicode TTF/TTC localized scene/menu/fight/reload lifecycle; its native comparison exposed K9 character palette mapping/reservation, now implemented and accepted. Missing licensed/CJK payloads and physical CD remain deferred; native-reproducing script defects are not engine gaps. U5 whole-text bearing/overhang/alignment rules are corrected and accepted in the existing asset lifecycle; native glyph hinting, including advance and baseline precision, remains open. | K9, U2 |
| **A3 — Rendering and audio** | Reference observation corrected the layerno inference: tested result values 0/1/2 all draw in front. Actual gaps were caption viewport projection and fresh result-scene/pose policy. Other calibrated raster/audio findings stay closed. | K1, K2, U5 |
| **A4 — Options, training and AI** | K3 AI.Cheat admission, K5/K6 Options/Training and U3 acceptance remain closed. K8 now implements and tests weighted fallback movement, defence, attack/cancel, button cadence, recovery and ownership/reset. Direct target handler/live-combat review also establishes learned move tables and a separate movement/attack/guard planner; that confirmed remaining consumer keeps K8 partial and U4 open. Independent random streams and non-cheating timed CMD plans are explicit port policies. Command admission now preserves the reviewed target index table, including recovery and unmapped slots; K8 remains partial for actual learned move planning. | K3, K5, K6, K8, U3, U4 |
| **A5 — Input compatibility** | Legacy CFG enable/numeric bindings, named/raw overrides, keyboard alternatives, session editing and ownership/hotplug have connected injected-device acceptance. Target-handler review establishes native numbering; Linux evdev metadata supplies the SDK button/hat boundary. Physical/native calibration remains H1. | K4, H1 |

### Deferred capabilities

- Physical CD playback.
- Unavailable licensed/CJK font payloads; the Unicode reader exists, but absent fonts are not supplied by the engine.
- Other-platform runtime acceptance.
- Standalone music-file CLI playback.

New discoveries must be recorded as explicit scope changes with evidence.
They do not silently enlarge this baseline or reopen unrelated completion.

- K1 corrected by direct target observation: absent layerno reads did not prove a behavioral defect. Tested Win/Survival values 0/1/2 share front composition; K1 now covers that policy and the demonstrated viewport-projection gap.
- K2 expanded within the results batch: target results initialize a separate human-only presentation, including restored life/context, zeroed variables and partner placement. Continuing completed-fight actors was incorrect. This supersedes the earlier result-clock implementation without reopening unrelated mechanics.
- U3 target comparison established two additional implementation targets, K5 Options behavior/presentation and K6 Training dummy/menu lifecycle. This explicitly expands the original four-target baseline to six; both now pass their connected acceptance and close U3. Session-only persistence and portable input editing remain intentional port policies.
- K5 includes the target team-setting consumer: CFG ordinal 0 means Team loses, 1 means AI keeps fighting. The default policy excludes an AI partner from human-led Simul survival, rather than ending on any member KO. Team factory reset restores 200% advantage. These findings are part of the Options lifecycle, not separate targets.
- U1 combined lifecycle comparison established K7: external contact entry, ordinary Pause owner allowance during hitpause and TargetBind expiry through global Pause differed in Go, changing throw motion and release. An unbounded reference constraint distinguishes expiry from frozen placement. This explicitly expands the six-target scope to seven; K7 implementation/connected acceptance and U1 now pass.
- U4 handler/consumer review establishes K8: the port’s fixed-cooldown heuristic does not implement the target’s weighted difficulty/distance/state-dependent input decisions. This explicitly expands the seven-target scope to eight; K8 is partial and U4 remains unverified pending a bounded decision contract and connected acceptance.
- K8 refinement: reviewed weighted fallback contexts are implemented and accepted. Direct handler/live-combat observation also confirms move tables populate during play and feed separate learned movement/attack/guard planning; those missing consumers remain inside K8, without adding a ninth target. U4 stays open; non-cheating timed CMD plans and independent random streams remain explicit port policies.
- U2 refinement explicitly adds K9: a shared native/Go font-content flow exposed ignored character Palette Keymap selection, independently confirmed in the target loader. This expands the implementation map from eight targets to nine. Unicode TTF/TTC availability is resolved; small font hinting/centering differences are recorded under the existing U5 precision followup, not another implementation target.
- K9 and U2 closure: authored palette mapping, held-Start selection and available-outfit reservation now match the reviewed target path through gameplay/reload. Licensed Unicode TTF/TTC content acceptance passes in the existing connected lifecycle and native/Linux application journey. This closes the supported asset/font functional category; font raster/centering precision remains U5 and the learned AI planner remains K8/U4. No additional target or binary fixture was added.
- U5 precision refinement: direct target text-library surfaces establish complete text extents, negative-bearing padding, centered truncation and inclusive right anchors. Those layout rules are corrected within the existing followup, without adding an implementation target. Compatible TrueType hinting remains unresolved; modern system FreeType alone does not reproduce the target raster. Prior format/content acceptance and camera/sprite/audio calibrations remain closed.
- K8 command-index prerequisite resolved: direct loaded-table inspection confirms first-declaration indices for ordinary labels, alternative reuse, reserved recovery index zero and an unmapped declaration slot. AI now samples that indexed namespace and ignores unmapped selections. This is a correction within K8, not another target or closure of learned move planning.
- K8 collection checkpoint: successful command queries are attributed within each controller to actual state transitions, then live actor episodes populate bounded ground/air move banks. Direct target observation corrects the timing-group interpretation from poses to whiff/guard/hit outcomes. Collection timing/geometry, hitpause exclusion, smoothing and round retention pass the existing connected AI/combat journey. Learned-data loading and planner consumers remain open within K8; full segment validation waits for that coherent batch.

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

- [ ] **state-flow**: entry → negative states → chained transitions → persistence/loop diagnostics.
- [ ] **combat**: logical input → attack/contact → damage/stat scaling/life adjustments/runtime fallback/recovery/reset → held/projectile/helper guard/release → jump/air-jump budget/landing/reset → sweep/legacy bounce/recovery.
- [ ] **entities**: helper creation → self binding/Pause/body contact → retirement/orphan effects → hit-paused throw/recovery → borrowed-state effect/Pause/SelfState/expiry → round identity reset.
- [ ] **effects-pause**: Pause/SuperPause → actor/background/effect clocks → resources/power → reset.
- [ ] **match-flow**: title/selection → rendered VS → stock input-driven fight → KO/time-over → results → rematch/exit.

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
- [x] Complete font, media and asset compatibility. (item4)
- [ ] Remaining native runtime, rendering, audio and AI fidelity. **partial** — K1/K2 result scenes, K3 AI.Cheat admission, K5/K6 Options/Training and K7 overlapping runtime ordering are closed for their named behavior and acceptance. K8 native AI decision policy remains partial under U4; U5 retains precision uncertainty. Calibrated camera, sprite/shadow, alpha and ordinary KO-echo findings remain closed. (compatibility next; known-difference batch complete (user 2026-11-08))

### Modes and platforms

- [ ] Player input devices and configuration. **partial** — Legacy CFG enable/numeric bindings and portable/raw YAML/session controls reach the Linux input consumer. K4 software acceptance is complete. H1 remains physical Linux/native mapping and calibration acceptance; injected layouts cannot close it. Other-platform legacy capability adapters are deferred. (item4)
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
