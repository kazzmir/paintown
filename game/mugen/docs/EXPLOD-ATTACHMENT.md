# Explod entity binding, motion and clocks — M4cn

Explod creation now consumes bindID instead of raising an unsupported-binding
error. The native corpus has62 programs:59 accepted/708 scalar readings and three
load rejections.249 unique hashes pin248 original CNS/CMD/AIR/stage files and
the probe.49 root-owned positions are independently calibrated by exact whole-
pose translation of the native image, beyond merely counting objects. Fractional
sampling and alternate owners are not inferred from white-pixel counts.

## Native creation and selection

ID-1 binds to the caller, including player2 and a helper. It does not choose
World.Roots[0]. The second-player ModifyExplod control confirms the same shared
lookup, correcting the earlier M4ck inference drawn from player1-only evidence.
ID-2 and lower negative IDs preserve an existing legacy anchor. An unresolved
nonnegative ID warns and also retains that anchor. A new unanchored effect stays
unanchored. Valid player/helper IDs select those entities.

Explicit binding offsets keep their world orientation when the anchor turns;
the drawing facing is independent. Legacy postype first supplies its old facing-
oriented offset and motion direction. An explicit valid ID then retargets that
same offset without reorienting it. Time defaults to1;0 leaves the initial
position unbound, positive values count the issuing update, and negative values
persist. Destroying an anchor stops following it while retaining the last pose.

Screen-space creation warns and ignores explicit binding. Legacy postype can
already have installed its own anchor, which remains. In the measured combined
p2/screen case the beta puts the actor's screen pose into the offset, then later
adds its stage pose during binding. This peculiar mixed-space behavior is
reproduced rather than replacing it with the documented modern equivalent.

## Evaluation and animation lookup

For the currently compiled fields, native order is anim, ID, facing, vfacing,
pos X/Y, vel X/Y, accel X/Y, scale X/Y, bindtime, bindID, ontop, removetime,
supermovetime, pausemovetime, sprpriority, removeongethit. Tagged assignments
pin the ordering. All fields still undergo load validation before execution.
Direct float integer literals reject; runtime values truncate and bottom becomes
zero in the tested binding selectors.

A negative or omitted anim falls back to action0. It therefore creates an object
when action0 exists. The earlier M4ae missing-animation fixture omitted action0,
so its zero count did not establish a universal no-object default. When the
selected action is unavailable, later geometry/binding expressions are skipped.
Production creation resolves the animation before invoking that lazy packet.
Palette/remap expressions have their own earlier native phase; their full
compiler/consumer is the next creation slice, not accepted by this checkpoint.

## Motion and pause clocks

A live constraint overwrites motion displacement while acceleration continues.
After it expires, movement resumes with that accumulated velocity. Explicit
facing flips drawing; movement follows legacy postype's direction. Positive and
negative drawing-facing free effects move in the same world direction, while a
p2 effect retains p2-oriented motion even if its drawing facing is flipped back.

Owned effect movement allowances are absolute age limits. A small positive
pausemovetime or supermovetime can be exhausted before a later pause begins.
-1 continues indefinitely in the measured control. Binding pose, duration,
acceleration and animation freeze together when the effect cannot advance,
including when the owner still has permission to move. Native's internal age
includes creation; Go retains the initial pose separately at age0. The measured
comparison is reproduced without changing any native golden or tolerance.

This update applies the age-limit policy to owned Explods. Other effect factories,
legacy supermove/-2 aliases, reloads and wider pause/hitpause combinations remain
separate controls for subsequent effect closure. Ordinary scene-driven cues keep
their prior policy until that native matrix is established.

## Evidence, acceptance and remaining work

Original inputs and images are immutable. Three genuine load rejections, actual
controller-kind/state/count assertions and59 native white masks accompany the
scalar comparisons. Some screen controls are intentionally offscreen. Only exact
whole-pose matches with a known profile support the49 promoted positions; a
fractional pose can have the same pixel count as a mirrored pose and is not
treated as proof of facing.

Engine/Effect races pass1.425s/1.010s. Focused Match races pass126.003s across236
new and prior attachment/binding/mutation/alpha programs. All1,480 frozen inputs
verify. Character/System/CNS races pass11.146s/9.246s/1.012s.17 final actual-window
white masks are exact. One initial profile app used the wrong opponent DEF;
that diagnostic is preserved, and m4cn2 repeats it with the correct native
1280-coordinate opponent. Final checks pin both selected definitions, not just
the root CNS/AIR. No runtime code or native observations changed for the repeat.
Corrected profile and stock win/rematch/win were visually inspected.82 balanced
owned events and2,002,050 private PCM frames pass. All249 native/1,480 focused
source and application/config/binary/PNG/audio/module/inventory hashes/guards
pass. All sessions awaited/no clients. See HANDOFF.md and the ignored final audit.
The latest full integration is M4cl.

Remaining creation fields include palette ownership/remapping, random offsets
and their stream side effects, angles/transparency, shadows, capacity, relative
ordering and wider resource/foreign/helper/clock combinations. The family stays
partial in CONTROLLER-BINDINGS.json. Shared alpha and final rotation/sampling
limits from MODIFY-EXPLOD.md remain explicit. Item3 continues; item4 is unallocated.

The standard staged whitespace check reports trailing blank lines in the frozen
explodattach-helper-retire and explodattach-turn-enemy CNS originals. Those bytes
remain intact. With blank-at-EOF excluded, checks pass; all328 staged files are
UTF-8 source/text under game/mugen/, with no binaries.
