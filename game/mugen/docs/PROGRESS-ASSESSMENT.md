# Progress assessment at the M4bd pause

Assessment date: 2026-10-17. The user requested a checkpoint, an assessment and a
pause before further token spending. Implementation is paused after M4bd until
the user resumes it. The approved target remains full M.U.G.E.N 1.1 beta 1
compatibility, with all changes confined to `game/mugen/`.

The port now runs a connected stock one-on-one game and has substantial native
evidence for language, state scheduling, combat, ownership, drawing and sound.
It is still a partial compatibility engine. Full state-machine coverage, all
asset behavior, all modes and final reference conformance remain unfinished.
The recent work has concentrated on deep combat/rendering details; the long
M4 checkpoint sequence does not imply that M5 through M8 are nearly finished.

## What is working and what the evidence establishes

| Area | Current demonstrated progress | Completion limit |
| --- | --- | --- |
| Build/reference foundation | Scoped dependency overlay, repeatable launcher/checks, isolated native Wine probes, pinned sources and capture provenance; root module unchanged. | Character/font/Match dependencies still initialize Ebitengine and require a display for many CPU-oriented tests. |
| Language/state runtime | Typed int32/float32/bottom evaluator and native-backed expression/ordering, transitions, negative states, pause, ownership and coordinate controls. | No current all-parameter/all-trigger completion ledger proves that every documented variant is covered. |
| Production stock flow | Title → character/stage selection → versus → real match → result → rematch/exit. Shared Match tests cover stock movement, attacks, hit/recovery, KO and time-over lifecycle. | Lifecycle tests sometimes set life at phase boundaries; they do not prove all combat or native round timing. Basic single-player opponent logic is not native-equivalent AI. |
| Combat/entities | Native controls cover target IDs, custom states/throws, helpers, binding/retirement, contact selection/priority, guard/hit contexts, facing, velocity, corner push, damage/power, lethality and fall damage. | Full HitDef schemas, juggle/recovery, projectiles/reversals, teams and interaction combinations remain open. |
| Assets/presentation | Stock v1/v2/v2.01 sprite loading, bitmap/outline font support, stage/HUD/effect drawing and actual sound output; many owned crops and PCM measurements match native. | Broad camera/effect/palette/background/format behavior is still incomplete. BGM is unimplemented. Font fallback is not stock Windows glyph/metric parity. Strict raster failures remain. |
| Verification | Reusable native numeric/pixel/audio fixtures, deterministic replays, scoped/full race checks, production capture provenance and local reversal commits. | Reference coverage is focused, not exhaustive. Full modes, long-running multi-entity/multi-round behavior and platform conformance still need gates. |

The original [ASSESSMENT.md](ASSESSMENT.md) and the controller/trigger tables in
[COMPATIBILITY.md](COMPATIBILITY.md) are the **2026-10-16 baseline**, not current
counts of unfinished features. Progress is recorded in the checkpoint history
and topic documents. Counting registered names, fixtures or commits would give
a misleading whole-engine percentage. There is no defensible completion
percentage or total remaining token estimate from the present evidence.

## Latest revisions and checkpoint result

- M4bb (`9ef4c313`) corrected lethal damage caps, fatal guard normalization,
  one-shot raw fall damage, KO velocity adjustments and ordinary CNS duplicate
  parameter precedence. Ninety native programs compare 1,080 observations and
  four expected type rejections. The full scoped race suite passes, including
  Match in 618.564s. See [HIT-LETHALITY.md](HIT-LETHALITY.md).
- M4bc (`54d8c86e`) corrected stage diagnostic background fill and its integer
  conversion rule. Eleven native programs, 88,000 exact color-region pixels
  and 33 viewport samples pass. See [STAGE-CLEAR.md](STAGE-CLEAR.md).
- M4bd adds 12 native cue/pause programs, 144 observations and 40 source hashes.
  Sound, spark selection and pause semantics pass; full Character race checks
  pass in 11.149s. Production PCM matches the selected native amplitudes.
  Two new strict spark raster checks fail despite correct action/anchor
  selection. This checkpoint records the failures and makes no production code
  change. See [FATAL-GUARD-CUES.md](FATAL-GUARD-CUES.md).

All changes and local commits stay under MUGEN. No remote push was made.
Reference art/binaries and temporary captures remain uncommitted.

## Work required for the original completion target

| Approved milestone | Assessment now | Remaining acceptance work |
| --- | --- | --- |
| M0: reproducible foundation | Established for this environment. | Continue preserving provenance; finish separation from display dependencies where it materially reduces verification cost. |
| M1: language/front end | Substantial implementation and focused native coverage. | Reconcile every controller/trigger variant, parameter type/default, error and version rule to tests; close gaps rather than promoting whole names from a single successful case. |
| M2: scheduling/input/identity | Substantial state, world, pause, helper and ownership evidence. | Complete input/gamepad and command boundary coverage, remaining state/default rules and broad foreign/helper/pause interactions. |
| M3: complete assets | Stock decoding/loading foundation works. | Close the format/resource/default/malformed-data matrix, animation variants and font/platform policy; CPU/GPU separation is incomplete. |
| M4: playable stock match | Connected one-on-one path works. | Native KO slowdown and absolute round/end timing, round persistence/reset/draw rules and wider stock integration still need closure. |
| M5: full combat/entities | Heavily developed, incomplete. | Finish HitDef schema/runtime consumers, all GetHitVar fields, recovery/juggle/chain rules, projectiles, reversals, teams and multi-contact combinations. |
| M6: effects/audio/rendering/all language entries | Partial, with extensive narrow evidence. | Remaining controller/trigger variants, palette/afterimage/angle/zoom/camera/background/effect behavior, BGM and other audio/configuration details; resolve strict render failures. |
| M7: complete game modes/UI | Largely open beyond one-on-one flow and some storyboard wiring. | Arcade progression/endings, team/simul/turns/co-op, survival/training/watch, AI/difficulty, win/continue/game-over UI and configuration lifecycle. Existing win/continue/game-over screen types still contain black-screen stubs. |
| M8: final conformance | Harness active; final gate not reached. | Exhaustive inventory closure, native comparison sweep, malformed-input/fuzz work, sustained reset/resource checks, full production scenarios and final interactive review. |

Specific known gaps that affect the next allocations:

1. **HitDef breadth:** the current documented audit has 81 keys, of which 36
   have expression compiler routing; 45 remain literal or absent. Those 45
   include enums/attributes as well as numeric fields. This is a schema count,
   not 36 fully finished features or 44% engine completion. Missing groups
   include fall/recovery/down-bounce, air juggle/chains, distance/snap
   constraints and palette/shake fields. Existing literal timing/fall fields
   also need coercion/default/order tests and runtime consumers. HitFallSet's
   optional velocity/value behavior and remaining GetHitVar selectors need
   review. See [HITDEF-SCHEMA.md](HITDEF-SCHEMA.md).
2. **Match clocks:** current Match phases provide lifecycle behavior, but native
   KO slowdown and precise over/win timing remain open. Per-contact hitpause
   passing at M4bd does not close them.
3. **Entity/combat combinations:** complete projectile lifecycle/cancellation,
   reversals, juggle/recovery, friendly/team targeting and multi-entity,
   foreign-state, helper and pause combinations still need implementation or
   native confirmation. Existing bounded controls do not cover their cross
   product.
4. **Rendering:** six M4az RGB alpha cases still fail by up to one channel
   value; M4bd additionally exposes two spark raster failures (all 276 native
   core pixels differ, maximum channel delta 8, plus a one-column core-width
   difference). None is waived. Broader vertical camera/zoom/rotation,
   background, effect and complete-frame behavior remains open.
5. **Sound/fonts/assets:** BGM playback is absent. The stock Arial/MS Gothic
   payloads are absent, so the portable outline-font fallback cannot promise
   their metrics, glyphs, CJK coverage or Windows rasterization. Providing the
   requested fonts or explicitly accepting a platform limitation will be a
   later review decision. Full asset support cannot be claimed from stock
   loading alone. See [BUILD.md](BUILD.md) and [AUDIO.md](AUDIO.md).
6. **Modes:** the current menu exposes mode names whose full rules/progression
   are not implemented. This is a substantial remaining workstream, not polish
   after combat.

## Suggested allocations after review

For token control, replace open-ended iteration with an explicitly bounded
feature batch and stop after its checkpoint. Before estimating a large batch,
refresh a concise current controller/trigger/parameter ledger from existing
implementation, tests and topic documents. The baseline inventory is too old
to support an honest numerical burn-down. That refresh should produce statuses
and evidence links, not a new exhaustive native probing campaign.

| Suggested order | Bounded workstream | Useful stopping point | Relative uncertainty |
| --- | --- | --- | --- |
| 1 | Current inventory refresh, then one selected HitDef field family | Updated ledger plus implemented schema/default/runtime behavior and native controls for that family; list the next family without starting it. | Moderate; coercion and evaluation order can expose shared defects. |
| 2 | KO/round lifecycle | Native clock traces and production stock KO/time-over/reset scenarios agree for a defined set of round settings. | High; pause, slowdown and CNS clocks interact. |
| 3 | Projectile/reversal/juggle/team combat, split into separate batches | One subsystem's lifecycle and trigger/ownership behavior is complete with multi-entity fixtures. | High; broad new runtime behavior remains. |
| 4 | Modes and BGM, each as separate deliveries | One named mode's start-to-finish scenario, or verified music loading/looping/transitions, works through production. | High in aggregate; these can be budgeted as visible user-facing slices. |
| 5 | Renderer/asset conformance and final M8 sweep | Existing strict failures close without changed goldens; remaining advertised inventory rows and production scenarios have evidence. | High; exact raster/platform behavior can be costly. |

I recommend prioritizing functional breadth and lifecycle before spending a
large allocation on one-value alpha precision. The newly exposed spark raster
failure is larger and should remain visible in the renderer ledger; its cause
is not established. The original full-parity target still requires all of
these gates unless the user explicitly changes scope.

Use focused regression checks while developing a family, fresh native replays
for its accepted controls, and the full scoped race suite at meaningful
integration boundaries. A full Match race run already takes about ten minutes;
repeating broad suites and many equivalent captures for every small change
adds cost without closing the large outstanding areas. Keep finite original
fixtures, source hashes and failed comparisons, while avoiding redundant prose
and repeated investigation of settled rules. No next implementation batch has
been started after M4bd.
