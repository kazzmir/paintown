# Current progress assessment

Updated 2026-10-18 through the verified M4bt final checkpoint.
The approved target remains full M.U.G.E.N 1.1 beta 1 compatibility. The user
subsequently authorized all 27 HitDef keys remaining after 24b3ebd0; that allocation
is the current scope. All authored changes stay under game/mugen/, with local
checkpoint commits, no push and no new binaries/assets committed.

The connected stock one-on-one game is playable. Stock sweeps now complete
trip, lying down, get-up and return to control. The 27-key allocation adds literal
mask/enum validation, guard distance, chain/juggle selection, constraints/snap,
hit palette effects and contact/fall shake, with actual runtime consumers.
All 81 documented HitDef keys have typed schemas: 72 numeric expressions plus
nine literal fields. This is 100% of that key inventory, not 100% behavioral
compatibility or a whole-engine percentage. Broad interactions remain open.

## Measured progress

| Scope | Current count | What it establishes |
| --- | ---: | --- |
| HitDef typed key inventory | 81/81 (100%) | 72 numeric schemas and nine literal schemas, with bounded native defaults/errors/order and consumers. HITDEF-KEYS.json and HITDEF-REMAINING.md. |
| Controller registration | 87/89 (97.8%) | Dispatcher names only; includes partial implementations and stubs. ModifyExplod and ExplodBindTime are unregistered. |
| Trigger recognition | 127/127 (100%) | Vocabulary only; bindings/variants are not all complete. |
| Loaded stock state programs | 216/216 (100%) | Both KFM variants and loaded common states compile; not all execution paths/interactions. |
| Current 27-key native corpus | 248 programs/3,144 numeric observations | Includes 36 target load rejections and 992 frozen CNS/AIR/CMD/stage input hashes across seven batches. 19 palette color/portrait, 17 contact-shake and 16 fall-shake render comparisons provide additional bounded evidence. |

The counts have different denominators and must not be averaged. The original
ASSESSMENT.md and inventory status columns in COMPATIBILITY.md are dated
baseline evidence. COVERAGE.md and topic documents describe current behavior.
No defensible overall engine completion percentage or remaining token estimate
follows from the present inventory.

## What remains for the original target

| Workstream | Current foundation | Remaining acceptance |
| --- | --- | --- |
| Language/state runtime (M1/M2) | Typed expressions, shared executor, ordering/persistence, negative states, ownership, pause, coordinates and stock commands have extensive native controls. | Every remaining controller/trigger schema/binding/variant, version/default/error rules, complete command/gamepad timing and broad foreign/helper/pause combinations. |
| Combat/entities (M5) | All HitDef key schemas; bounded contact, guard, damage/power, fall/velocity/recovery eligibility, juggle/chains, targets/throws, helpers and single-contact projectiles. | Full recovery/input/mashing/liedown behavior; remaining GetHitVar fields; full projectile lifecycle/cancellation/queries, reversals, teams and multiple-contact/entity interactions. |
| Match clocks (M4) | Working intro, combat, KO/time-over, result, round reset/rematch and real stock sweep recovery. | Native KO slowdown and absolute over/win/round timing, draw/persistence/reset combinations and sustained multi-round production scenarios. |
| Effects/rendering/audio (M6) | Sprite/background/HUD/effect drawing, hit palette/shake, sound output and owned numeric/pixel/PCM controls. | Standalone EnvShake, PalFX/AllPalFX/BGPalFX, afterimages/angles/zoom/transparency and other incomplete methods; broad camera/background behavior; BGM loading/looping/transitions. |
| Assets/fonts (M3) | Stock SFF v1/v2/v2.01, AIR/SND, bitmap/outline fonts and relocatable resource loading work. | Full format/default/malformed-resource matrix, animation variants, resource lifecycle and platform conformance. Missing Arial/MS Gothic payloads use a visible Go Regular fallback, without native glyph/metric parity. |
| Modes/UI/AI (M7) | Connected title → selection → versus → match → result → rematch/exit; basic opponent logic. | Arcade progression/endings, teams/simul/turns/co-op, survival/training/watch, native AI/difficulty, continue/game-over/win UI and configuration lifecycle. Several screen types retain black-screen stubs. |
| Final conformance (M8) | Repeatable isolated native captures, scalar fixtures, production smoke/provenance, scoped races and reversal commits. | Exhaustive inventory closure, sustained reset/resource tests, malformed-input/fuzz coverage, full production scenarios and interactive review. |

Six M4az RGB alpha comparisons still fail by at most one channel value. Two
M4bd spark raster comparisons remain open (276 native core pixels versus 288 Go,
maximum channel difference 8). Neither is waived. Hit palette rendering evidence
is bounded to opaque indexed KFM sprites; successful CPU alpha handling does
not close native filtered/partial-alpha parity.

## Checkpoints and next allocation

HITDEF-REMAINING.md records the seven batches and their source/topic evidence.
HIT-SHAKE.md and FALL-SHAKE.md describe the final families; HANDOFF.md records
exact validation results and artifacts. CHECKPOINTS.md records local reversal
history. The final full scoped race suite passes, including Match in 942.741s and
Character in 11.918s. It includes corrections made after the
earlier M4bp integration run; earlier failed or superseded runs remain diagnostic.

The requested 27-key allocation is complete, with final native/application
checks and local checkpoint commits. A useful next functional allocation is native KO/round lifecycle,
followed by complete projectile/reversal/team subsystems as separate deliveries.
Modes and BGM can also be budgeted as visible end-to-end slices. Resolve strict
renderer/platform failures before claiming final conformance. This assessment
does not silently expand the current allocation into all remaining M0–M8 work.

Keep feature batches bounded. Freeze original native controls before comparing
Go, use focused checks while developing, and run full scoped races at meaningful
integration boundaries. Preserve findings and exact limitations at each local
checkpoint; do not push or commit generated binaries/reference art.
