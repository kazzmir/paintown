# Current behavior coverage ledger

Updated 2026-10-17 through M4bf (storyboard canvas), after M4be and upstream
merge 99b4605c. This ledger reconciles current source/tests with the historical
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
| HitDef timing | M4be compiles pausetime/guard.pausetime with native source defaults, reversed component evaluation, truncation, bottom/negative handling and malformed-input checks. HIT-PAUSE.md; prior FATAL-GUARD-CUES.md covers lethal cue selection. | Other hit/slide/control timing fields, broad foreign/helper/projectile/Pause/SuperPause combinations and absolute KO/round clocks are separate batches. |
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
(44.4%). M4be adds the two pause keys: **38/81 (46.9%)**, leaving 43 keys
literal or absent. The other metrics are unchanged. See
[../README.md](../README.md) for numerator/denominator sources and
[HIT-PAUSE.md](HIT-PAUSE.md) for the new native-backed scope.
Registration includes partial and empty behavior. No aggregate behavioral
compatibility or line/branch test percentage is established by these counts.

The original inventory in COMPATIBILITY.md remains a baseline. Its status
columns must not be used to count current missing/verified controllers or
triggers. This ledger intentionally groups obligations; a future closed status
needs a parameter/variant-specific claim and evidence link.
Empty runtime methods are source-audit findings. Native comparison must still
distinguish missing Go behavior from operations disabled in the beta itself.

## Current allocation and stop condition

The user authorized M4bf storyboard scaling, followed by one bounded combat
batch through the next checkpoint. M4bf is validated. M4bg covers compiled
`ground.hittime`/`ground.slidetime`, measured defaults/order/types and focused
consumer controls. Commit the validated result and stop for user testing.
Do not start further timing fields, round clocks or renderer families. Existing
strict alpha/spark failures remain open.
