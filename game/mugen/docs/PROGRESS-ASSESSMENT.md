# Current progress assessment

Updated 2026-10-18 through verified M4bw complete-match validation.
The approved target remains full M.U.G.E.N 1.1 beta 1 compatibility. The 27-key HitDef allocation is complete. The user subsequently authorized
M4bu clocks, M4bv results/reset and M4bw complete input-driven matches. All authored changes stay under game/mugen/, with local
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
| KO/time-over clock corpus |29 programs/1,740 observations | Native slowdown, timer/pause boundaries, readiness and pose/reset clocks; ROUND-LIFECYCLE.md. |
| Complete stock scenarios | Eight matches/18 rounds | Full-life logical/keyboard attacks, both winners, alternating wins, time draws and three repeated rematches. A separate rendered 2%-life replay reaches two result screens. Full scoped integration passes. COMPLETE-MATCHES.md. |
| Results/reset corpus | 26 controls/1,560 observations | Native normal-mode results, holds, late damage/DKO, persistence and cleanup; ROUND-RESULTS.md. |
| Completed 27-key native corpus | 248 programs/3,144 numeric observations | Includes 36 target load rejections and 992 frozen CNS/AIR/CMD/stage input hashes across seven batches. 19 palette color/portrait, 17 contact-shake and 16 fall-shake render comparisons provide additional bounded evidence. |

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
| Match clocks (M4) | Working intro, combat, KO/time-over, result, round reset/rematch and real stock sweep recovery. | M4bu establishes bounded KO/time-over/pose clocks. M4bv establishes bounded draws/persistence/reset, and M4bw completes full-life stock matches/rematches. Broader assertion/pause/reset interactions, cumulative engine GameTime handoff and exact presentation timing remain. |
| Effects/rendering/audio (M6) | Sprite/background/HUD/effect drawing, hit palette/shake, sound output and owned numeric/pixel/PCM controls. | Standalone EnvShake, PalFX/AllPalFX/BGPalFX, afterimages/angles/zoom/transparency and other incomplete methods; broad camera/background behavior; BGM loading/looping/transitions. |
| Assets/fonts (M3) | Stock SFF v1/v2/v2.01, AIR/SND, bitmap/outline fonts and relocatable resource loading work. | Full format/default/malformed-resource matrix, animation variants, resource lifecycle and platform conformance. Missing Arial/MS Gothic payloads use a visible Go Regular fallback, without native glyph/metric parity. |
| Modes/UI/AI (M7) | Connected title → selection → versus → match → result → rematch/exit; basic opponent logic. | Arcade progression/endings, teams/simul/turns/co-op, survival/training/watch, native AI/difficulty, continue/game-over/win UI and configuration lifecycle. Several screen types retain black-screen stubs. |
| Final conformance (M8) | Repeatable isolated native captures, scalar fixtures, production smoke/provenance, scoped races and reversal commits. | Exhaustive inventory closure, sustained reset/resource tests, malformed-input/fuzz coverage, full production scenarios and interactive review. |

Engine-wide GameTime origin/handoff across menu and rematch boundaries remains
open: native GameTime includes menus, while current Match timelines are local.
The scalar trace alignment does not prove that engine integration.

Three preserved native controls expose unbound legacy HitVel Y (native -24 versus
Go bottom/0 in the measured profile); they do not establish a physical-knockback
difference and are excluded from the passing result corpus.

Six M4az RGB alpha comparisons still fail by at most one channel value. Two
M4bd spark raster comparisons remain open (276 native core pixels versus 288 Go,
maximum channel difference 8). Neither is waived. Hit palette rendering evidence
is bounded to opaque indexed KFM sprites; successful CPU alpha handling does
not close native filtered/partial-alpha parity.

## Checkpoints and next allocation

HITDEF-REMAINING.md records the seven batches and their source/topic evidence.
HIT-SHAKE.md and FALL-SHAKE.md describe the final families; HANDOFF.md records
exact validation results and artifacts. CHECKPOINTS.md records local reversal
history. The final full scoped race suite belongs to M4bw: 2,707 passing test/subtest
results across 24 tested and 12 compiled application packages, no individual
skips/failures. Match takes 968.597s and the complete command 1,037.599s. Source,
root-module and application/PNG/private-PCM provenance checks pass. Earlier
failed or superseded runs remain diagnostic.

M4bu, M4bv and M4bw are complete within the approved round-lifecycle allocation.
The next small allocation should audit missing trigger bindings (including
legacy HitVel) and wire the native cumulative GameTime origin through engine
menus/rematches before expanding broad interactions.
Then prioritize remaining
recovery/input behavior and projectile/reversal/team subsystems as separate deliveries.
Modes and BGM can also be budgeted as visible end-to-end slices. Resolve strict
renderer/platform failures before claiming final conformance. This assessment
does not silently expand the current allocation into all remaining M0–M8 work.

Keep feature batches bounded. Freeze original native controls before comparing
Go, use focused checks while developing, and run full scoped races at meaningful
integration boundaries. Preserve findings and exact limitations at each local
checkpoint; do not push or commit generated binaries/reference art.
