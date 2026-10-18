# Current behavior coverage ledger

Updated 2026-10-18 through M4bl HitFallSet, M4bk fall vectors, M4bj trip recovery, M4bi fall/recovery, M4bh air/down timing, M4bg ground/guard timing and M4bf storyboard
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
| Hit/guard selection | Native hit flags, posture, reciprocal priority, heading and draw priority. HIT-FLAGS.md, COMBAT-PRIORITY.md, CONTACT-FACING.md. | Juggle/chains, reversals, teams and remaining HitDef constraints. |
| Damage/power/KO vectors | Typed expressions, power defaults/rules, lethal caps, fatal guard normalization, fall damage and measured KO velocities. HIT-POWER.md, HIT-LETHALITY.md. | Fall recovery, all GetHitVar selectors and absolute KO/round clocks. |
| HitDef timing | M4be compiles pausetime/guard.pausetime with native source defaults, reversed component evaluation, truncation, bottom/negative handling and malformed-input checks. HIT-PAUSE.md. M4bg adds ground.hittime/ground.slidetime and their four dependent guard timings, including source defaults and measured order. HIT-GROUND-TIMING.md. M4bh compiles air/down timers, default 20/explicit 0 and bounded launch/down/countdown selection. HIT-AIR-DOWN-TIMING.md. | Full recovery/timing interactions, broad foreign/helper/projectile/Pause/SuperPause combinations and absolute KO/round clocks are separate batches. |
| Fall/recovery eligibility | M4bi compiles fall/air.fall/fall.recover/fall.recovertime; native boolean selection, repeated source default, delay default/coercion and order. GetHitVar(fall.time/fall.recovertime) and CanRecover share the receiver clock, including shake and H-state continuation controls. HIT-FALL-RECOVERY.md. | Full recovery/bounce physics, HitFallSet variants, juggle/chains and broad contact/entity/pause combinations. |
| Trip recovery | M4bj removes the recurring trip-state heuristic, chooses native Trip entry at contact, stores air.type and corrects groundtype/airtype values. Thirteen native programs and real mixed-profile down+B sweeps complete lying down, get-up and idle/control. TRIP-RECOVERY.md. | Full enum validation, other recovery-command routing, liedown/mashing clocks and broad entity/contact/pause combinations. |
| Fall velocity/gravity | M4bk compiles fall.xvelocity/fall.yvelocity/yaccel; native omission versus zero, float/bottom/order/default/profile rules, plus actual HitFallVel and scripted gravity consumers. FALL-VECTORS.md. | Full recovery/bounce/HitFallSet rules and broad foreign/helper/projectile/team/contact/pause combinations. |
| HitFallSet | M4bl compiles optional value with native preserve/raw integer/coercion rules and retained clock/query/velocity consumers. The beta ignores documented velocity fields, including invalid expressions. HITFALLSET.md. | Broad foreign/entity/contact/pause combinations and full recovery remain open. |
| Projectiles/reversals | Projectile currently constructs a Character-derived object; ReversalDef stores parameter data (`character/character.go`). | Dedicated lifecycle/cancellation/contact queries and consumed reversal rules with native evidence. |
| Effects/palettes | Typed Explod/RemoveExplod and palette remapping have bounded evidence. EXPLODS.md, RGB-CONSUMERS.md. | ModifyExplod/ExplodBindTime are unregistered. AfterImage, AfterImageTime, EnvShake, EnvColor, Trans, PalFX/AllPalFX/BGPalFX and several system effects still reach empty Character methods. FallEnvShake delegates to empty EnvShake. |
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
**53/81 (65.4%)**, leaving 28 keys literal or absent.
The other metrics are unchanged. See
[../README.md](../README.md) for numerator/denominator sources and
[FALL-VECTORS.md](FALL-VECTORS.md) for the new native-backed scope.
Registration includes partial and empty behavior. No aggregate behavioral
compatibility or line/branch test percentage is established by these counts.

The original inventory in COMPATIBILITY.md remains a baseline. Its status
columns must not be used to count current missing/verified controllers or
triggers. This ledger intentionally groups obligations; a future closed status
needs a parameter/variant-specific claim and evidence link.
Empty runtime methods are source-audit findings. Native comparison must still
distinguish missing Go behavior from operations disabled in the beta itself.

## Current allocation and stop condition

The user has authorized two more checkpoints after M4bk (915f40d7): M4bl
HitFallSet optional value/velocities and bounded consumers, then M4bm
down.bounce schema and down-hit consumers. Acceptance requires frozen native
controls, focused race checks, actual application smoke checks and documented
limits. Run one full scoped race suite at the second integration boundary,
commit both checkpoints locally and pause. No third family or push is authorized. M4bl has passed its native/focused/core/
application gates; see HITFALLSET.md and HANDOFF.md. M4bm is active.

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
