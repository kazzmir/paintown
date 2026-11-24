# Project progress

This is the single completion ledger for the Go MUGEN 1.1 beta port.
Checked items have a working implementation for the stated scope. Partial
items name the remaining work. Counts below are feature/route counts, not
an estimate of total effort or complete native compatibility.

- Controllers: 89/89 routes; 0 known partial, 0 missing.
- Triggers: 127/127 runtime routes; 0 missing.
- Project categories: 33 implemented, 2 partial, 0 missing.
- Remaining-scope implementation targets: 10 closed, 0 open.
- Separate acceptance followups: 4 passed, 2 open.

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
| **K8 — Native AI input decision policy** | implemented / passed | large / high |
| **K9 — Authored character palette keymaps through selection** | implemented / passed | medium / high |
| **K10 — Compatible TrueType automatic hinting and solid glyphs** | implemented / passed | medium / high |

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

- [x] **K8**: Match the target AI input policy through weighted fallback movement, defence, attack/cancel, press/release cadence, command admission and recovery, plus its learned-move movement/attack/guard planner, while retaining CNS eligibility and human/helper/reset ownership.
  Basis: Target handler/table review and connected human/CPU observation establish command-index attribution, separate ground/air banks, whiff/guard/hit timing, hitpause exclusion, five-percent repeat estimates and completed-record round retention. Compatible optional beta learned-data files are actually consumed. Native operation also reaches waypoint navigation, predictive guard and learned counter contexts with observed move data.
  Prerequisite: Resolved for the bounded input/learning/planner lifecycle. Independent seat random streams, timed non-cheating physical plans and session-only learned-data retention are explicit port policies; no seed-for-seed stochastic equivalence or automatic binary-cache writing is claimed.
  Completion: Production collection, bounded beta-cache reading, independent actor banks and revision/language/game-width session profiles feed opponent recognition, startup/range/reversal counter selection, source-pose preparation, predictive held guard and walk/jump waypoints. The existing connected journey covers human whiff/air/hit and guarded outcomes, startup3/active6/recovery12, hitpause, smoothing, physical and admitted counters through CNS/contact/recovery, waypoint completion, helper/human ownership and round/reload retention. All five connected segments pass with race detection for current source. Linux build and a live two-CPU match pass with visible normal/special attacks, damage and advancing clocks. Owned runtime/overlay/capture scratch is removed; no binary fixture or user configuration is committed.

- [x] **K9**: Carry authored Palette Keymap and availability through character portrait loading, manual/random-cell selection, outfit reservation, roster snapshots, fight PalNo and round/reload ownership. Preserve absent/partial-group defaults, first fields and 1–12 clamping; keep direct numeric palette requests separate from button ordinals.
  Basis: The shared native/Go Unicode font-content journey exposed different outfits from the same stock button. Target loader and reservation-handler review established authored mapping, defaults, clamping, legacy Files availability and used-outfit fallback. The production selection consumer now implements those rules without loading full fight banks for previews.
  Prerequisite: Resolved for the named manual-selection lifecycle by target handler review and one connected custom-map/held-Start/duplicate-fighter/fight/reload comparison. Non-cheating AI palette preference rules are not inferred. Start-only confirmation remains a port convenience, admitted on release so held Start can modify an attack.
  Completion: The existing localized asset/selection/team lifecycles pass with race detection through generated two-face TTC fonts, mapped numeric palettes, random-cell identity, duplicate outfit reservation, unchanged direct numeric requests, actual combat and reload. Native and rebuilt Linux match the custom Start mapping at CNS PalNo 6 before/after reload and retain distinct outfits for repeated picks. Linux build/application inspection passes; owned runtime data is removed.

- [x] **K10**: Connect compatible automatic-hinted glyph metrics and grayscale/monochrome rasterization to the existing TrueType text/layout consumer on Linux, with explicit portable fallback and bounded native resource ownership. Preserve bitmap fonts, font substitution, Unicode/TTC loading and authored height overrides.
  Basis: Direct reference font-library operation identifies its legacy unpatented engine and ordinary-font automatic-hinting path. Forcing automatic hinting in modern FreeType matches every measured glyph metric and complete text width in the existing shared regular/italic Unicode scene. Default bytecode hinting and unhinted outlines do not. The reference uses a real monochrome rasterizer for Blend=0 rather than thresholding its grayscale image. Antialiasing coverage still has version-dependent differences.
  Prerequisite: Resolved for the Linux automatic-hinting/metric and grayscale/monochrome consumer with explicit portable fallback. Native-resource ownership, the existing connected asset lifecycle and final application comparison pass. Residual cross-version edge coverage remains the separate U5 acceptance question; no nonzero tolerance is assumed.
  Completion: The optional Linux FreeType backend uses automatic hinting for ordinary TrueType faces, real monochrome masks for Blend=0 and bitmap bearings for trimmed borders. Memory faces borrow pinned Go bytes, use per-face synchronization and release native resources with their owner. Other platforms or unavailable libraries retain the reported Go outline fallback. The shared native/Go scene has matching glyph metrics, widths, baselines and all ten text-block bounds. The affected match-flow asset/menu/fight/reload segment passes with race detection; four unchanged segments remain checked. Font go vet, final Linux build, normal-match smoke and final shared-scene capture pass. No new Go dependency, FreeType build headers, binary fixture or user configuration is introduced. U5 stays open for the measured remaining rasterizer-version edge differences.


### Separate acceptance and evidence questions

These followups are separate from each implementation target's own
acceptance. Open questions do not imply missing controller/trigger routes.

| ID | Scope / status | Evidence needed to close it |
| --- | --- | --- |
| **U1** | Additional runtime interaction fidelity — passed | Direct target comparison of one overlapping throw/Pause/hitpause, bound-anchor turn, finite expiry and own-state recovery episode established K7. The corrected production consumers match its clock/entry/pose observations and pass the extended entity-to-round-reset scenario, affected connected segments with race detection and Linux build/application smoke. This closes the named lifecycle, not arbitrary runtime permutations. Prerequisite: Resolved for the named combined lifecycle through direct target observation and the existing connected entity path. Prior binding/facing/reset findings remain intact. |
| **U2** | Broader font/content acceptance — passed | Functional acceptance passes for the supported font/content contract: openly licensed TTF and generated two-face TTC inputs render Latin, Greek and Cyrillic text, honor the first collection face and authored heights, and traverse localized storyboard/menu/selection/VS/fight/rematch/reload alongside bitmap/SFF/ACT/SND resources. The native/Go journey exposed K9 palette selection, now corrected and accepted. The affected connected segment passes with race detection and the rebuilt Linux window flow is inspected. This does not establish arbitrary-pack compatibility or pixel-exact font rasterization; named font precision remains U5. Prerequisite: Resolved with openly licensed module font inputs and the connected native/Go content comparison. Generated collections/captures/runtime overlays are disposable; no font payload or binary fixture is committed. Unavailable licensed/CJK payloads remain deferred. |
| **U3** | Native Options/training behavior and presentation — passed | Direct target menu/Training observation and handler review established K5/K6. Both are implemented and accepted through the existing Options-to-gameplay and Training lifecycles, race-checked connected segments and final Linux window/PNG inspection. Session-only persistence, startup-binding reset and portable input editing are explicit port policies; bounded AI acceptance is recorded separately under U4, while raster/audio precision remains U5. Prerequisite: Resolved for the named Linux Options/Training behavior and usable utility presentation. Target input/team/default navigation, value admission, dummy modes/policy, pause/step/minimize and return/reentry were exercised or traced to their handlers. |
| **U4** | Native AI decisions beyond AI.Cheat — passed | K8 closes the bounded native AI lifecycle: indexed weighted fallback, live ground/air move collection, optional learned-data loading, independent session retention and learned navigation/guard/counter consumers. Target observation reaches all five decision contexts; the connected Go journey reaches real guard, physical/admitted counter contact and recovery, walk/jump completion and ownership/reset. Affected connected segments pass with races and the rebuilt Linux application completes live two-CPU combat. This establishes the named decision lifecycle with permitted random variability, not identical random sequences or universal character strategy. Prerequisite: Resolved through target handler/table review, connected native collection/planner observation and production Linux acceptance. Preserve independent seat streams, timed non-cheating plans and session-only retention as explicit port policies. |
| **U5** | Additional raster/device-audio precision — unverified | K10 now closes compatible automatic-hinted metrics and real solid-glyph rendering on Linux. The shared regular/italic Unicode scene matches all ten text-block bounds and placement, alongside negative bearings, overhangs and baseline/advance checks in the existing asset lifecycle. Affected connected acceptance passes with races; font go vet, Linux build, normal-match smoke and shared native/Go application captures pass. Remaining differences are rasterizer-version edge coverage: blended mean absolute error 0.687/255 over ink pixels, with an isolated maximum 127; solid masks differ at 23 of 3106 ink pixels. All differences lie within two pixels of the reference silhouette. These measurements describe one bounded scene, not an accepted universal tolerance. U5 remains unverified until the user accepts the demonstrated rendering or requests strict legacy-raster reproduction. Earlier camera/sprite/alpha/KO-echo findings stay closed. Prerequisite: User review chooses whether to accept the demonstrated modern/legacy edge differences. Exact glyph metrics, text placement and the compatible production backend are implemented and validated; no nonzero pixel tolerance is silently assumed. The portable outline fallback and unavailable licensed/CJK payloads do not inherit this Linux precision acceptance. |
| **H1** | Physical Linux controller acceptance — awaiting-prerequisite | Exercise menu/configuration, selection, combat and unplug/reconnect on hardware; preserve ownership, neutral/deadzone and held/edge behavior. Include native mapping after K4. Injected devices cannot close H1. Prerequisite: An available controller and someone able to exercise it; no connected device is assumed. |

### Bounded assessment findings

The documentation/source review is complete for these boundaries.
The baseline reused existing evidence; subsequent target comparisons and scope changes are recorded above.

| Review | Finding | Allocation |
| --- | --- | --- |
| **A1 — Runtime ordering** | The bounded U1 comparison established K7: external contact entry waits through attacker hitpause, ordinary Pause owner allowance is retained during hitpause, and TargetBind duration advances through global Pause. Those consumers now match the connected target turn/release/recovery trace and pass entity/round-reset acceptance. Existing helper, facing, binding and SuperPause findings remain closed for their stated scope. | K7, U1 |
| **A2 — Assets, fonts and media** | Supported format families have connected functional acceptance under U2, including Unicode TTF/TTC through localized scenes, menus, gameplay and reload. K9 closes palette mapping/reservation. U5 establishes K10, now connecting automatic-hinted metrics and real solid rendering with explicit portable fallback. Shared-scene metrics and placement match; residual rasterizer-version edge differences require explicit U5 review. Missing licensed/CJK payloads and physical CD remain deferred. | K9, U2 |
| **A3 — Rendering and audio** | Reference observation corrected the layerno inference: tested result values 0/1/2 all draw in front. Actual gaps were caption viewport projection and fresh result-scene/pose policy. Other calibrated raster/audio findings stay closed. K10 closes the compatible TrueType backend; U5 now awaits acceptance of measured cross-version glyph-edge differences. | K1, K2, K10, U5 |
| **A4 — Options, training and AI** | K3 AI.Cheat admission, K5/K6 Options/Training and U3 acceptance are closed. K8/U4 now close indexed weighted fallback, command-attributed live ground/air learning, optional beta cache loading, independent session retention and the navigation/guard/counter planner. Target context observation, connected CMD/CNS/contact/recovery/reset acceptance, race-checked segments and live Linux CPU combat pass. Independent random streams, non-cheating timed plans and session-only model storage remain explicit port policies. | K3, K5, K6, K8, U3, U4 |
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
- K8/U4 closure: live command-attributed ground/air learning, bounded beta cache loading, independent revision/language/game-width session banks and learned navigation/guard/counter consumers now pass the existing connected lifecycle. Direct target observation reaches all five contexts. Physical/admitted counters and walk/jump completion, affected race-checked segments and live Linux two-CPU combat pass. Independent random streams, timed non-cheating plans and session-only storage remain explicit port policies. K1-K9 are closed; U5 precision and H1 hardware remain open.
- U5 reference refinement establishes K10: direct font-library operation identifies legacy automatic hinting for ordinary fonts. Modern forced automatic hinting matches all measured shared-scene glyph metrics and widths; the previous bytecode-hinting assumption was incorrect. Blend=0 also needs actual monochrome rasterization. This adds one confirmed implementation target within U5; residual version-dependent antialiasing remains an acceptance question, with no tolerance assumed.
- K10 implementation closure: optional Linux automatic hinting and true monochrome rendering now match shared-scene metrics/placement and pass the existing asset/menu/fight/reload lifecycle with races, font vet and Linux application inspection. U5 remains open solely for explicit acceptance of measured version-dependent edge coverage. No binary payload or new Go module dependency is added.

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
- [x] Complete font, media and asset compatibility. (item4)
- [ ] Remaining native runtime, rendering, audio and AI fidelity. **partial** — K1/K2 results, K3 command admission, K5/K6 Options/Training, K7 runtime ordering, K8/U4 learned AI and K10 compatible TrueType metrics/raster consumers are closed for their named contracts. U5 now awaits explicit acceptance of small version-dependent glyph-edge differences; exact shared-scene metrics and placement pass. Earlier camera, sprite/shadow, alpha and KO-echo calibrations remain closed. (compatibility next; known-difference batch complete (user 2026-11-08))

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
