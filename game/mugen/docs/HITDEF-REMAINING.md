# Completed HitDef allocation

The user authorized all 27 keys remaining after 24b3ebd0, with local checkpoint
commits and continued iteration until complete or blocked. All authored work
stays under game/mugen/. No push or new binary file is authorized.

The allocation is complete through M4bt. All 81 documented keys have typed
schemas: 72 numeric plus nine literal. Final scoped races, native replays and
production captures pass. Across seven batches: 248 programs, 3,144 numeric
observations, 36 load rejections and 992 frozen input hashes. See
[HITDEF-KEYS.json](HITDEF-KEYS.json) and [HANDOFF.md](HANDOFF.md). Broader engine
and interaction gates remain open. The counts below record checkpoint history.

Initial count: 54 expression keys, 18 remaining numeric keys and nine literal
keys out of the manual's 81 entries. Literal enum/flag fields need schemas,
not numeric expression evaluation. Counts do not establish complete behavior
or a whole-engine percentage. Each closure needs native/default/error and
consumer evidence, or a demonstrated limitation in the target executable.

| Batch | Keys | Status |
| --- | --- | --- |
| M4bn | guard.dist | Complete; GUARD-DISTANCE.md, 15 programs/204 observations/four native load rejections. |
| M4bo | attr, hitflag, guardflag, affectteam, animtype, air.animtype, fall.animtype, ground.type, air.type | Complete; HIT-LITERALS.md, 65 programs/684 observations/eight load rejections. |
| M4bp | air.juggle, chainID, nochainID | Complete; HIT-CHAINS-JUGGLE.md, 57 programs/660 observations/eight load rejections. |
| M4bq | mindist, maxdist, snap | Complete; HIT-CONSTRAINTS.md, 39 programs/588 observations/seven native load rejections. |
| M4br | palfx.time, palfx.mul, palfx.add | Complete; HIT-PALETTE.md, 24 programs/324 observations/three load rejections, plus 19 rendered color/portrait comparisons. |
| M4bs | envshake.time, envshake.freq, envshake.ampl, envshake.phase | Complete; HIT-SHAKE.md, 26 native programs and 17 rigid-pixel comparisons. |
| M4bt | fall.envshake.time, fall.envshake.freq, fall.envshake.ampl, fall.envshake.phase | Complete; FALL-SHAKE.md, 22 programs/336 observations/three load rejections and 16 rigid render comparisons. |

After M4bn: 55 expression keys; 26 keys remain (17 numeric, nine literal).
Checkpoint boundaries may change if native findings require dependencies.
Use focused checks per batch and full scoped races at enum/combat and final
integration boundaries. Preserve the previous integration's diagnosed failure
and passing targeted correction records; do not label its full log a pass.
All engine/mode/asset/renderer gates outside these keys remain open.

After M4bo: 55 numeric expression keys + nine literal schemas =64/81 typed
keys. Seventeen numeric keys remain; native measurements may expose ignored
parameters that require explicit limitation accounting rather than evaluation.

After M4bp: 58 numeric expression keys + nine literal schemas =67/81 typed
keys (82.7%). Fourteen keys remain in the authorized allocation.

After M4bq:61 numeric + nine literal =70/81 typed keys (86.4%); eleven
palette/shake keys remain. Continue automatically.

After M4br:64 numeric + nine literal =73/81 typed keys (90.1%); eight
contact/fall shake keys remain. Continue automatically through final integration.

After M4bs:68 numeric + nine literal =77/81 typed keys (95.1%); four stored-fall shake keys remain.

After M4bt:72 numeric + nine literal =81/81 typed keys (100%); zero keys remain
in this allocation. This does not imply full HitDef interaction or engine parity.
