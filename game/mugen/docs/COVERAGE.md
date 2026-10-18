# Current behavior coverage ledger

Updated 2026-10-18 through M4bt stored fall shake, M4bs contact shake, M4br hit palette effects, M4bq constraints, M4bp chains/juggle, M4bo literal schemas, M4bn guard distance, M4bm downed-hit bounce, M4bl HitFallSet, M4bk fall vectors, M4bj trip recovery, M4bi fall/recovery, M4bh air/down timing, M4bg ground/guard timing and M4bf storyboard
canvas scaling, after upstream merge 99b4605c. This ledger reconciles current source/tests with the historical
inventory. A row describes demonstrated behavior and its remaining gate; it
does not promote an entire controller from one passing parameter/default.

| Area | Current implementation/evidence | Remaining gate |
| --- | --- | --- |
| Language and compiler | Typed int32/float32/bottom, versioned expressions, assignment, tuples, redirect contexts, precompiled core parameters. `evaluator/`, `engine/parameters.go`, BOTTOM.md. | Full controller schemas, all trigger bindings/variants/errors and their interactions. |
| State/world scheduling | Shared executor and production world; native state entry, persistence, negative states, ordering and physics. WORLD.md, PHYSICS.md, ACTOR-ORDER.md. | All Statedef/default/version cases and broad paused/foreign/entity combinations. |
| Input | Logical input shared with production, stock keyboard mappings, command matching. INPUT.md, `system/flow_test.go`. | Full command timing/modifier/facing cases and gamepad adapters. |
| Identity/helpers | Match-owned IDs, ownership, spawning, retirement, root/parent/target redirects. ENTITIES.md, HELPERS.md, RETIREMENT.md. | Full projectile/team/multi-entity behavior and parameter interactions. |
| Targets/throws | Acquired target IDs/generations, custom state/animation ownership, binding and stock throw controls. TARGETS.md, OWNERSHIP.md, BINDING.md, THROWS.md. | Full foreign/helper/pause/projectile combinations and all controller variants. |
| Hit/guard selection | Native hit flags, posture, reciprocal priority, heading and draw priority. HIT-FLAGS.md, COMBAT-PRIORITY.md, CONTACT-FACING.md. | Broader juggle/chain/constraint interactions, reversals, teams and multi-entity combinations. |
| Damage/power/KO vectors | Typed expressions, power defaults/rules, lethal caps, fatal guard normalization, fall damage and measured KO velocities. HIT-POWER.md, HIT-LETHALITY.md. | Fall recovery, all GetHitVar selectors and absolute KO/round clocks. |
| HitDef timing | M4be compiles pausetime/guard.pausetime with native source defaults, reversed component evaluation, truncation, bottom/negative handling and malformed-input checks. HIT-PAUSE.md. M4bg adds ground.hittime/ground.slidetime and their four dependent guard timings, including source defaults and measured order. HIT-GROUND-TIMING.md. M4bh compiles air/down timers, default 20/explicit 0 and bounded launch/down/countdown selection. HIT-AIR-DOWN-TIMING.md. | Full recovery/timing interactions, broad foreign/helper/projectile/Pause/SuperPause combinations and absolute KO/round clocks are separate batches. |
| Fall/recovery eligibility | M4bi compiles fall/air.fall/fall.recover/fall.recovertime; native boolean selection, repeated source default, delay default/coercion and order. GetHitVar(fall.time/fall.recovertime) and CanRecover share the receiver clock, including shake and H-state continuation controls. HIT-FALL-RECOVERY.md. | Full recovery/bounce physics, juggle/chains and broader HitFallSet/contact/entity/pause combinations. |
| Trip recovery | M4bj removes the recurring trip-state heuristic, chooses native Trip entry at contact, stores air.type and corrects groundtype/airtype values. Thirteen native programs and real mixed-profile down+B sweeps complete lying down, get-up and idle/control. TRIP-RECOVERY.md. | Broader reaction-type/recovery-command routing, liedown/mashing clocks and entity/contact/pause combinations. |
| Fall velocity/gravity | M4bk compiles fall.xvelocity/fall.yvelocity/yaccel; native omission versus zero, float/bottom/order/default/profile rules, plus actual HitFallVel and scripted gravity consumers. FALL-VECTORS.md. | Full recovery and broader bounce/HitFallSet/foreign/helper/projectile/team/contact/pause combinations. |
| HitFallSet | M4bl compiles optional value with native preserve/raw integer/coercion rules and retained clock/query/velocity consumers. The beta ignores documented velocity fields, including invalid expressions. HITFALLSET.md. | Broad foreign/entity/contact/pause combinations and full recovery remain open. |
| Downed-hit bounce | M4bm compiles down.bounce; native zero/nonzero/float/bottom/order rules, zero-launch boundary, contact fall-vector reset and stock bounce/get-up consumers. PosFreeze survives same-tick transitions; first idle observation retains facing. DOWN-BOUNCE.md. | Full recovery, all auto-turn/PosFreeze edges and broad entity/contact/pause/foreign combinations. |
| Literal masks/enums | M4bo schemas, selected/stored reaction types, untouched/guard type metadata, None contacts, HitDefAttr and trip defence entry. HIT-LITERALS.md. | Broad team/entity/foreign/pause combinations and all other fall-defence/state-number rules. |
| Chains/juggle | M4bp integer schemas, predecessor ownership/ID, guard/None/idle context, per-move and per-projectile costs, non-fall budget preservation and chainid query. HIT-CHAINS-JUGGLE.md. | Broad helper/foreign/team/third-party/recovery/pause and full projectile interactions. |
| Distance/snap constraints | M4bq strict integer schemas, source replacement/order, per-axis intervals, deferred offsets/GetHitVar and timed binding, facing/profile/CNS-write/moving-axis controls. HIT-CONSTRAINTS.md. | Broad entity/projectile/foreign/override/pause and moving timed-bind combinations. |
| Guard distance | M4bn compiles guard.dist with native integer/coercion/bottom/order rules, preserves omitted/negative range, resets it on state entry and verifies facing/boundary/height/profile consumers. GUARD-DISTANCE.md. | Broad input/entity/foreign/pause combinations and other AttackDist variants. |
| Projectiles/reversals | M4bp adds a narrow single-contact projectile adapter with native parameter names, animation, retirement/removal time, owner target credit and juggle cost. ReversalDef still stores parameter data (`character/character.go`). | Dedicated lifecycle/cancellation/contact queries and consumed reversal rules with native evidence. |
| Contact shake | M4bs native schemas/defaults/order, fractional amplitude, asymmetric rounding, issuing-frame duration, global clock, profile conversions, ScreenPosY and world-only rendering. M4bt adds stored fall payload, typed getters, positive-duration consumption and guard/fall=0 rules. HIT-SHAKE.md, FALL-SHAKE.md. | Standalone EnvShake and broad request/entity/pause/zoom combinations. |
| Effects/palettes | Typed Explod/RemoveExplod and palette remapping have bounded evidence. M4br adds native HitDef palette schemas, shared clocks, cache/portrait isolation and exact opaque indexed color comparisons. EXPLODS.md, RGB-CONSUMERS.md, HIT-PALETTE.md. | ModifyExplod/ExplodBindTime are unregistered. AfterImage, AfterImageTime, EnvShake, EnvColor, Trans, PalFX/AllPalFX/BGPalFX and several system effects still reach empty Character methods. |
| Drawing/camera | Shared world/HUD paths, horizontal camera, mixed coordinates and exact bounded sprite crops. CAMERA.md, COORDINATES.md, SPRITE-SAMPLING.md. | Six M4az alpha failures, two M4bd spark raster failures; broad vertical camera/zoom/angle/background/effect parity. |
| Assets/fonts | Stock SFF v1/v2/v2.01, sound, bitmap/outline fonts and resource lookup; corpus/malformed-input tests. `tests/`, BUILD.md. | All format/animation/resource variants; absent Windows font payloads use a visible fallback and lack native glyph/metric parity. |
| Audio | Production command queue/mixer, channel ownership, pan/volume/voice/cues and owned native PCM controls. AUDIO.md. | BGM, broader configuration/resource/clock combinations; ForceFeedback remains an empty method. |
| Stock match/UI | Title/selection/versus/fight/result/rematch/exit tested through production. M4bf storyboard localcoord and game/window scaling match six native placement controls. STORYBOARD-CANVAS.md, MATCH.md, `system/flow_test.go`. | Exact KO/round persistence/draw/reset clocks. Full modes/AI/progression; win/continue/game-over types retain black-screen stubs. |
| Other explicit stubs | VarRandom, GameMakeAnim and MakeDust have empty Character methods (`character/character.go`). | Implement runtime behavior, schemas and native controls. BindToParent/Root's legacy methods are empty, but typed binding paths must be assessed through `engine/binding.go`, not those fallback bodies alone. |

## Quantitative scopes

At the start of M4be: 87/89 documented controller names registered (97.8%),
127/127 trigger names recognized (100%), 216/216 loaded stock state programs
compile (100%), and 36/81 documented HitDef keys have expression routing
(44.4%). M4be added the two pause keys (38/81). M4bg adds six ground/guard timing
keys (44/81). M4bh adds the two air/down keys (46/81). M4bi adds four
fall/recovery keys (50/81). M4bk adds three fall velocity/gravity keys:
53/81 (65.4%). M4bm adds down.bounce (54/81). M4bn adds guard.dist:
**55/81 (67.9%)**, leaving 26 keys literal or absent at M4bn. M4bo adds nine literal schemas:
55 numeric + nine literal = **64/81 typed keys (79.0%)**, with seventeen
numeric keys remaining. M4bp adds three integer expressions: **58/81 (71.6%)** numeric, plus nine
literal schemas = **67/81 typed keys (82.7%)**. M4bq adds three constraint schemas: **61/81 numeric (75.3%)**, plus nine
literals = **70/81 typed keys (86.4%)**. M4br adds three palette schemas: **64/81 numeric (79.0%)** plus nine
literals = **73/81 typed keys (90.1%)**. M4bs adds four contact-shake schemas and the shared clock/render consumer: **68/81 numeric (84.0%)**, plus nine literals = **77/81 typed keys (95.1%)**. M4bt adds those four fields: **72/81 numeric (88.9%)**, plus nine literals = **81/81 typed keys (100%)**. [HITDEF-KEYS.json](HITDEF-KEYS.json) pins the manual/source audit. Full interactions remain open.
The other metrics are unchanged. See
[../README.md](../README.md) for numerator/denominator sources and
[FALL-SHAKE.md](FALL-SHAKE.md) for the latest native-backed scope.
Registration includes partial and empty behavior. No aggregate behavioral
compatibility or line/branch test percentage is established by these counts.

The original inventory in COMPATIBILITY.md remains a baseline. Its status
columns must not be used to count current missing/verified controllers or
triggers. This ledger intentionally groups obligations; a future closed status
needs a parameter/variant-specific claim and evidence link.
Empty runtime methods are source-audit findings. Native comparison must still
distinguish missing Go behavior from operations disabled in the beta itself.

## Current allocation and stop condition

The user has now authorized all 27 remaining HitDef keys after 24b3ebd0,
with a local checkpoint for each coherent batch and continued iteration until
complete or blocked. Batches: guard.dist (1), literal enums/flags (9), juggle/
chains (3), minimum/maximum/snap constraints (3), palette effects (3), contact
shake (4), fall shake (4). Numeric routing remains a separate metric from the
nine literal schemas. Each batch requires frozen native controls, runtime
consumers or a demonstrated beta limitation, focused checks and documentation.
Full scoped races follow the enum/combat and final integration boundaries.
Prior renderer/round/mode/asset gaps remain open. All seven batches M4bn through M4bt are checkpointed; final scoped races, native replays and production render checks pass. The 27-key allocation is complete. See HITDEF-REMAINING.md for the live key ledger.
No remote push is authorized.

Prior allocation (completed):

The previous two checkpoints were 8779869a and 24b3ebd0. M4bm's full run
identified one idle-override regression, corrected and verified by affected
Match/full Character/native/app checks; see DOWN-BOUNCE.md and HANDOFF.md.

Prior allocation (completed):

The user resumed on 2026-10-18 with a stock KFM sweep-loop report and authorized
two checkpoints. M4bj establishes native trip entry and fixes the recurring
trip-state heuristic, including stock sweep recovery. Native/focused/core/app
checks pass and its local checkpoint is complete (3db8b057). M4bk compiles fall
x/y velocity and yaccel with bounded native consumers. Twenty programs pin
276 observations, three native rejections and 80 source hashes. Focused races,
three fresh native reproductions and three apps/five captures pass. The
combined full scoped race suite passes (Match 693.752s), covering both
checkpoints. M4bk is complete; pause for user testing and the next allocation.
Other families and existing strict alpha/spark failures remain open.

Prior allocation (completed):

The user pushed M4bh (ce6b7a50) and resumed the next bounded checkpoint.
M4bi implements fall/air.fall/fall.recover/fall.recovertime expressions and
recovery eligibility/clock consumers. Thirty native programs pin 372 numeric
observations, five load rejections and 120 input hashes. Focused races, three
fresh native reproductions, three actual apps/four captures and the full
scoped race suite pass (Match 640.740s). M4bi is the completed local checkpoint;
pause for user testing and the next allocation. Legacy recovery state routing
and physics remain open; eligibility coverage does not close those paths.
Juggle, bounce physics, round clocks and renderer families stay outside this
allocation. Existing strict alpha/spark failures remain open.
