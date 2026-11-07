# Current handoff

Use [PROGRESS.md](PROGRESS.md) as the single completion checklist and
[WORKFLOW.md](WORKFLOW.md) as the implementation/test policy. The user explicitly
requested larger implementation batches, segment scenarios, no repeated completed
tests, concise docs, no new generated testing data in Git, and temp cleanup.

The last production Go commit is3aad3498a (M4dr). The one-on-one engine is playable:
stock character/stage selection, authored combat/recovery, rounds/results, sound
and rematches work. This workflow checkpoint changes reporting and test entry
points; it does not claim a new engine implementation or compatibility percentage.

Next implement the pending state-machine/controller work as coherent batches:

1. Finish Round/Fight/Win message scheduling and remaining controller schemas.
2. Implement clipboard formatting/debug display and combo-counter consumers.
3. Resolve retained concrete state/constraint defects, then run the affected
   connected segments and one playable end-state acceptance.

Use the project checklist for the rest of the port. Modes/teams/AI/BGM/broader
assets and final platform coverage are item4 and remain unallocated.

Existing timing observations are sufficient to proceed: Intro held through399
and released400; Round.time0/3 first displays401/404. Default and round-specific
components draw together. Short Round display does not set fight/control timing;
first RoundState2 is470, and ctrl.time0/3/6 first grants control470/473/476.
Fight display4 is470..473; fight.time3 shifts it473..476 independently of control.
Long Round display survives into active play. A positive Win/state180/phase4
transition was seen521. Do not restart the interrupted research queue.

The former queue stopped after8 of15 fine controls; the driver was deliberately
terminated after child cleanup. No data was promoted. User-authorized targeted
cleanup removed nine generated banks and recovered126.58 GiB. Canonical
`temp/mugen-1.1b1`, Go cache, development headers, Wine and private display/audio
tools remain. Old raw captures/prepared runs are gone; do not regenerate them.

Default --check runs pending/reopened segments only. --check --list shows their
status; --acceptance explicitly runs the complete match flow. The five selected
scenario sets passed at M4dr and are carried forward because production Go source
is unchanged. Known legacy raster/negative-camera failures are still recorded;
no broad rerun is needed for this documentation/tooling checkpoint.

Keep edits under game/mugen/, use local source/text commits, and do not push.
