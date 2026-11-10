# Project progress

This is the single completion ledger for the Go MUGEN 1.1 beta port.
Checked items have a working implementation for the stated scope. Partial
items name the remaining work. Counts below are feature/route counts, not
an estimate of total effort or complete native compatibility.

- Controllers: 89/89 routes; 0 known partial, 0 missing.
- Triggers: 127/127 runtime routes; 0 missing.
- Project items: 32 implemented, 3 partial, 0 missing.

## Known difference checkpoints

These checkmarks close the documented discrepancies for the observed scope.
Broader compatibility and deferred capabilities remain listed below.
Scope and validation details remain in the [feature catalog](STATE-MACHINE-FEATURES.json)
and Git history; this checklist records completion without repeating run logs.

- [x] **Offscreen camera return**
- [x] **Indexed sprite and shadow alignment**
- [x] **Finite and overlapping alpha composition**
- [x] **Ordinary KO echo playback**
- [x] **SF2 selection and legacy character loading**
- [x] **Rendered selection/VS and legacy bounce recovery**
- [x] **SF2 later-round initialization script repairs**
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
- [ ] **Documented debug and fight runtime controls**: Continuing batch: supplied readme/CFG identifies unwired life/power/stand/time-over, AI, restart/reload and presentation controls. Life/power/stand/time-over now use character/match consumers with the AllowDebugKeys gate. Connected runtime lifecycle acceptance will run after the coherent batch, not per key. AI toggles now change copied live roster ownership, active roots and future resets without rewriting campaign templates; charged AI plans are retired on ownership changes. Training menu ownership uses the same setter. CFG AllowDebugKeys=0 now admits hotkeys only while debug mode is visible, as documented by the supplied CFG. Speedup/VRetrace and Ctrl-S/Ctrl-V reach the runtime scheduler without changing CNS tick units; Ctrl-L gates bars independently of announcements. F4 now restarts the current round through normal entity/effect/sound cleanup. A pre-state5900 carry/score snapshot avoids repeated entry side effects, double RoundsExisted increments, spent Turns members and already-committed result leakage; process time/RNG continue. End-state acceptance remains pending for this continuing batch. Shift-F4 reloads character/stage/fight banks into fresh presentation caches while retaining the current round/score, starting carry, AI ownership, process clock and continuing RNG. Loading-time roster rotation is suppressed on reload; archive ownership and campaign templates remain intact. Replacing an expanded ZIP still requires returning to title to retire its expansion. The existing clipboard/combat/debug scenario now continues through key gates, life/power/stand, AI/pacing/HUD, time-over, paused restart, actual KO into round two, helper retirement, preserved score and on-disk character reload. That full connected episode passes with race detection; broader match-flow completion remains pending until the continuing presentation/input batch ends. Ctrl-C now draws actual root/helper/projectile collision geometry with the world camera/zoom/window/shake transform and reports live target IDs, remaining juggle and resolved HitBy masks. Draw offsets and sprite rotation stay separate from collision geometry. The existing runtime episode includes the rendered toggle; acceptance awaits the next combined run.
- [ ] **Options input configuration through gameplay**: Options/F1 now opens a session keyboard editor with per-player enable, replace/add/disable bindings, raw navigation while disabled and return to the same Options state. It shares YAML binding setters/fallbacks and does not write local configuration. Existing title/options/selection/two-round match flow edits an attack alternative and uses it through stock CNS; full end-state acceptance pending for this continuing batch. F2 now exposes per-player gamepad assignment/name/layout, enable/deadzone and replacement/alternative/disabled bindings. Capture sees unbound standard/raw buttons and axes; release admission prevents the activation key or a newly bound held control from operating the menu again. The title-to-match episode captures an unbound raw attack alternative and uses it in round two. Physical device acceptance remains partial. Raw-axis capture records the released baseline (including -1 trigger rests) and chooses the movement polarity. Device/layout changes rearm capture; standard layouts use normalized sticks and named buttons. The same connected Options/gameplay episode includes a raw trigger baseline and release.
- [ ] **Options layout and font consumers in HD motifs**: Options rows/columns/cursor/footer now follow motif dimensions, authored Option Info title offset/font/text reach drawing, and fixed English labels use uppercase for bitmap banks without lowercase glyphs. A reusable cursor pixel replaces per-draw GPU allocation. The connected Options/input/gameplay episode checks actual centered row pixels; combined acceptance pending.
- [ ] **Direct training, watch and partner match entry**: CLI --mode selects versus/watch/training/coop on the existing quick-match path; --p3/--p4 use normal simul rosters. Watch/partners get CPU ownership, while co-op P3 maps the second human through the existing adapter. Debug.StartStage remains the omitted-stage default. The existing team/selection/HUD/co-op episode now loads and renders a direct four-participant watch request; final acceptance pending.

## Completed scenario coverage

Completed segments are excluded from default runs. Only a change to their
covered source, a reported regression, or an explicit rerun reopens them.
Checkmarks record successful runs against the covered source fingerprint;
source changes reopen only the affected segments.

- [x] **state-flow**: entry → negative states → chained transitions → persistence/loop diagnostics.
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
- [ ] Complete font, media and asset compatibility. **partial** — Broader engine font, media and asset acceptance remains unverified. Physical CD playback is unavailable; absent licensed fonts use a reported fallback. Arbitrary content compatibility and raster parity are not established. Script defects that also reproduce in native are content findings, not engine implementation tasks; no further SF2 mod repairs are planned. (item4)
- [ ] Remaining native rendering, audio and AI fidelity. **partial** — Documented camera, indexed sprite/shadow, finite/overlap alpha and ordinary KO-echo discrepancies are closed for the checked scope. Broader pause/hitpause, bindings, crop/raster/filtering combinations, device-level sample timing and native AI policy remain unverified compatibility work. (compatibility next; known-difference batch complete (user 2026-11-08))

### Modes and platforms

- [ ] Player input devices and configuration. **partial** — Portable gamepad buttons, axes and YAML alternatives now feed menus and combat. Physical Linux controller acceptance and native legacy joystick-number translation remain unverified; controller models/connection types requested. Keyboard configuration and gamepad software flow are implemented, but this row stays partial until device acceptance. (item4)
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
