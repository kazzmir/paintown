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
- [ ] **Recovery constant dimensions across borrowed and redirected states**: Implemented a shared CNS dimensional classification so recovery multipliers stay unitless while velocities/distances scale; redirected hit X offsets now follow the same geometry policy as Y offsets. Native mixed-coordinate clipboard shows recovery multipliers .5/.2 and friction .85 unchanged while -4.5 Y addition becomes -1.125 and height60 becomes15 at 320/1280. Existing mixed-entity Pause/SelfState/recovery flow extended. Batch acceptance pending.
- [ ] **First character storyboards and portrait sprites**: Selection now retains first character Arcade fields across repeated groups, including explicit blanks, and first duplicate portrait sprites, matching normal archive lookups. The existing packed/legacy asset lifecycle contains ignored later missing stories and a later duplicate large portrait, followed by intro/VS/fight/reset/ending and pack retirement. Batch acceptance pending.
- [ ] **Roster entry presentation through selection handoff**: Confirmed selections retain value snapshots of their portrait/story/entry metadata alongside match rosters. Handoff no longer finds the first same-DEF row and substitutes its music or intro; backing out clears both snapshots and rosters. Existing packed intro-to-fight and team selection/VS/HUD/exit lifecycles exercise metadata changes after confirmation. Batch acceptance pending.
- [ ] **Localized roster limits through Arcade scheduling**: Arcade parses select.def limits with the same configured language as roster selection. Singleton Options fields keep first values across groups without deduplicating character/stage rows. The existing localized asset/selection/VS/fight/rematch lifecycle now schedules an order-two entry using localized limits and an ignored later duplicate. Batch acceptance pending.

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
