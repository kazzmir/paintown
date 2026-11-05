# README progress history through M4cw

This preserves the previous chronological README narrative. Counts refer to
their named checkpoints; use the current README and topic ledgers for status.

Snapshot: **2026-11-05, through M4cw implementation**. Item 3 is active:
ExplodBindTime, measured ModifyExplod mutation/drawing and parent/root/target
actor binding have production consumers. Explicit Explod entity binding, native
motion/clock behavior and caller-relative retargeting now work too. The latest full race suite (M4cv)
passes **4,033** test/subtest results across **36 packages**. M4cm focused races
pass194 native programs, including73 new controls/792 readings/seven rejections;
three actor-window masks match. M4cn adds62 controls/708 readings/three rejections,
49 calibrated positions and17 exact effect-window masks. See
[effect attachment](docs/EXPLOD-ATTACHMENT.md), [target binding](docs/TARGET-SELF-BINDING.md),
[parent/root binding](docs/SELF-BINDING.md), [effect binding](docs/EXPLOD-BINDING.md),
[mutation](docs/MODIFY-EXPLOD.md) and the [audit](docs/CONTROLLER-BINDINGS.md).
All89 controller names register;72 typed routes,5 stubs and partial consumers
remain. The two indexed and six RGB alpha counterexamples now pass unchanged goldens;
see [alpha precision](docs/ALPHA-NORMALIZATION.md). Rotation sampling and wider
documented gates stay open.
Actor AIR transparency and typed Trans now reach the shared native blend path;
see [actor transparency](docs/TRANS.md). Angle/scale/Offset now drive drawing,
including persistent angle state and native frame-local values. Their62 native
programs and58 actual apps provide46 exact RGB crops/54 exact white masks;
12 raster counterexamples remain unwaived. See [angles/offsets](docs/ANGLES-OFFSET.md).
Standalone PalFX now has typed schemas, helper ownership, expiry/sine clocks
and unclamped indexed/RGB drawing. Its53 native programs pin576 values/five
rejections/812 color observations; all48 actual PalFX crops match exactly
(5,216,400 RGB pixels). Both GPU samplers pass504 native color checks;224 opaque
CPU preview colors match too. See [standalone palette effects](docs/PALETTE-FX.md)
for validation and explicit global/interaction limits.
AllPalFX/BGPalFX now have global clocks and actor/effect/stage/HUD/font consumers.
Their83 native programs pin948 values/four rejections.71 application scenes pass
2,795 palette samples and all71 stage crops;67 actor crops match exactly, with
four explicit Explod priority-overlap counterexamples at M4cs. M4ct now closes
all four against unchanged goldens and adds16 exact151,800-pixel ordering crops;
see [effect ordering](docs/EFFECT-ORDERING.md).
The render tests pass10,352 native GPU color samples. See
[global palette effects](docs/GLOBAL-PALETTE-FX.md). Final full integration passes3,857 results across36 packages, with no failures
or individual skips and14,164 frozen inputs verified. Remaining item3 work
continues with a contact/fall frequency audit, AfterImage and other consumers.
EnvColor now has native RGB packing, global clocks and measured over/under,
foreground/HUD and palette drawing. Its76 native programs pin816 values/eight
rejections; all68 actual applications match31,715,200 crop pixels and2,516 color
samples. Ten regressions and stock win/rematch/win with private audio pass.
See [environment color](docs/ENV-COLOR.md) for beta quirks and remaining variants.
EnvShake now has typed parameters, native defaults/evaluation/replacement clocks
and fixed EnvColor/AllPalFX viewport-gap composition. Its72 native controls pin
792 values/six rejections. The first render matrix passes38/45 scenes; seven
larger-coordinate profiles retain explicit framebuffer/HUD precision failures.
HUD AIR transparency now reaches the shared blend consumer. Revision2 retains38/45 exact crops; the
full environment race suite passes4,033results/36packages. See
[environment shake](docs/ENV-SHAKE.md). These are bounded observations, not an
overall completion percentage.
Contact/fall frequency normalization now preserves tiny nonzero magnitudes and
uses the magnitude of negative inputs. Twelve additional native controls pin
144 observations; all145 selected native programs pass with the fix. See
[contact/fall frequency](docs/SHAKE-FREQUENCY.md). The full4,033-result suite
remains the M4cv integration boundary and precedes this focused parser fix.
The final M4cs integration passes25 tested and11 compiled-only packages,
with no failures or individual skips and all14,164 frozen inputs verified. No whole-engine completion percentage is justified.

Through M4ch, fresh native, actual
render, stock win/rematch and provenance checks also pass. Items 1–2 are complete. Random/VarRandom now share the native match stream. Projectiles have
typed setup, motion/lifetime/pause clocks, multiple hits, cancellation, capacity,
root ownership, foreign-state coordinate profiles and event queries. ReversalDef
and MoveReversed have native-tested combat consumers and seven exact application
render comparisons. See [projectiles](docs/PROJECTILES.md),
[contacts](docs/PROJECTILE-CONTACTS.md), [coordinates](docs/PROJECTILE-COORDINATES.md),
[queries](docs/PROJECTILE-QUERIES.md), [random](docs/RANDOM.md) and
[reversals](docs/REVERSALS.md) for evidence and variant limits.

Received hit/fall metadata and the liedown timer/input mashing are implemented.
Recovery follows character-authored CNS conditions. All34 beta-accepted documented
GetHitVar selectors have routes; two manual-only names reject like the reference.
See [metadata](docs/GETHIT-METADATA.md) and
[liedown/recovery](docs/LIEDOWN-RECOVERY.md).

Earlier changes cover damage/power transfer,
lethal hit/guard normalization, fall damage, KO velocities, stage diagnostic
fill, native sound/spark-selection/pause comparisons and compiled HitDef
hit/guard pause expressions with measured default/evaluation order. Storyboard
canvas scaling is corrected, and ground hit/slide plus dependent guard timing
expressions now follow native defaults and evaluation order. Air/down hit times
now compile as expressions, with corrected launch/down timer selection. Fall
flags and recovery delay now compile, and CanRecover follows the measured
fall clock; see [recovery evidence and limits](docs/HIT-FALL-RECOVERY.md).
The stock sweep loop is fixed by choosing Trip at contact and allowing CNS
recovery to finish; see [trip recovery](docs/TRIP-RECOVERY.md).
Fall X/Y velocity and hit gravity now compile as expressions, with verified
defaults, profile conversion and consumers; see [fall vectors](docs/FALL-VECTORS.md).
HitFallSet now follows native value/default rules, including the beta ignoring
its documented velocity fields; see [HitFallSet](docs/HITFALLSET.md).
Downed hits now respect `down.bounce`, including stock bounce/get-up timing;
see [downed-hit bounce](docs/DOWN-BOUNCE.md).
Guard distance now uses native integer expressions and preserves omitted/negative
range updates; see [guard distance](docs/GUARD-DISTANCE.md).
Literal masks/enums, contact animation metadata, None contacts and trip defence
now follow measured beta behavior; see [literal schemas](docs/HIT-LITERALS.md).
Chain selectors and per-move/projectile juggle costs now have measured consumers;
see [chains and juggle](docs/HIT-CHAINS-JUGGLE.md).
Distance limits and snap now have native timing, offset and duration consumers;
see [distance/snap constraints](docs/HIT-CONSTRAINTS.md).
Hit palette effects now change fighter colors with native arithmetic and
lifetimes while preserving HUD portraits; see [palette effects](docs/HIT-PALETTE.md).
Contact shake and stored fall shake now use the shared frame clock and render
with native rounding and profile conversion; the HUD stays fixed. See
[contact shake](docs/HIT-SHAKE.md) and [fall shake](docs/FALL-SHAKE.md).
The user-authorized 27-key allocation is complete; its M4bt full scoped races
pass. M4bu adds native KO slowdown, timer/pause boundaries and separate
control/pose/reset clocks with focused/native/application verification. See
[round lifecycle](docs/ROUND-LIFECYCLE.md). M4bv adds wins/draws, match completion,
win-pose holds, late damage/double KO and persistent reset, with 26 controls /
1,560 native integers. See [round results and remaining gates](docs/ROUND-RESULTS.md).
Legacy HitVel X/Y now returns native game-space floats, including heading,
guard/air/down, redirects and fractional profiles. The three preserved projectile
counterexamples pass unchanged; see [legacy velocity](docs/LEGACY-HITVEL.md).
Production GameTime now continues through menus, results and rematches.
TicksPerSecond and one-on-one MatchNo are bound, and configured game speed
uses the native rate formula. See [engine clock evidence](docs/ENGINE-CLOCK.md).
Complete input-driven
matches/rematches and final full scoped races now pass (2,774 test/subtest
results through M4cb). See [complete match validation](docs/COMPLETE-MATCHES.md).
