# Project progress

This is the single completion ledger for the Go MUGEN 1.1 beta port.
Checked items have a working implementation for the stated scope. Partial
items name the remaining work. Counts below are feature/route counts, not
an estimate of total effort or complete native compatibility.

- Controllers: 89/89 routes; 1 known partial, 0 missing.
- Triggers: 122/127 runtime routes; 5 missing.
- Project items: 25 implemented, 6 partial, 2 missing.

## Completed scenario coverage

Completed segments are excluded from default runs. Only a change to their
covered source, a reported regression, or an explicit rerun reopens them.
Checkmarks record successful runs against the covered source fingerprint;
source changes reopen only the affected segments.

- [x] **state-flow**: entry → negative states → chained transitions → persistence/loop diagnostics.
- [x] **combat**: logical input → attack/contact → damage/state continuation; jump → CNS landing recovery.
- [x] **entities**: helper creation → binding/retirement → orphan effect expiry → round identity reset.
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
- [ ] Remaining stage, viewport and team trigger bindings. **partial** — Bind IsHomeTeam, P3Name, P4Name, StageVar and TeamMode with the later modes/team context. (item4 context)

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

### Assets and sound

- [x] Stock character/stage, SFF/AIR/CNS/CMD loading.
- [x] Sound effects, channels and production mixing.
- [ ] Background music and NoMusic integration. **missing** — Implement BGM playback and the NoMusic consumer. (item4 dependency)
- [ ] Complete font/asset compatibility and loading errors. **partial** — Finish broader font/asset formats and errors; stock resource support exists. (item4)
- [ ] Remaining renderer and audio fidelity issues. **partial** — Resolve remaining color/shadow/alpha precision and KO-echo playback differences; stock bitmap label placement is corrected. (item3 presentation / item4 final conformance)

### Modes and platforms

- [ ] Full opponent AI compatibility. **partial** — Expand the basic opponent policy to the required AI behavior. (item4)
- [ ] Arcade progression and complete mode rules. **partial** — Implement complete progression and rules beyond the playable one-on-one flow. (item4)
- [ ] Team/simul/turns lifecycle and partner context. **missing** — Implement team roster, partner lifecycle, team modes and associated triggers. (item4)
- [ ] Additional platform builds and final playable acceptance. **partial** — Linux is exercised; validate the other supported platforms and final game flow. (item4)

## Controllers and trigger gaps

All 89 documented controller families are registered. The source check
flags concrete missing/partial consumers below. A route alone does not
prove every parameter combination; untested permutations are not new
implementation tasks by themselves.

| Controller | Remaining implementation |
| --- | --- |
| assertspecial | NoMusic has no BGM consumer. |

Missing trigger routes: `ishometeam`, `p3name`, `p4name`, `stagevar`, `teammode`.

Remaining stage/team context belongs with the later modes/assets allocation.

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
