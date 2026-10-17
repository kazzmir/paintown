# HitDef flags and fall contact eligibility

M4at uses 67 original `hitflag-*` programs: 792 native numeric observations,
one load rejection and 268 authored CNS/AIR/CMD/stage hashes. Fixed overlapping
boxes, disabled push, zero velocities and authored get-hit states isolate flags
from movement. The local beta `sctrls.html` hitflag/forcenofall descriptions are
the starting specification; native boundary and state controls take precedence
over assumptions from the manual or the incomplete C++ implementation.

## Measured behavior

H permits standing targets; L permits crouching; M supplies H and L. A permits
airborne targets. A falling airborne get-hit target needs both A and F: neither
A, F nor MF alone suffices; AF and the default MAF do. D permits downed targets
and selects their own state 5080. Standalone D is accepted despite the manual's
general statement requiring a posture letter. This checkpoint does not claim
complete down velocity/bounce/recovery behavior.

Plus requires target MoveType H; minus excludes H. Both modifiers together admit
neither idle nor H targets, in either written order. These filters precede
priority arbitration: an ineligible higher-priority attack cannot suppress its
eligible reciprocal opponent. Rejected candidates do not consume a hitonce
definition or acquire a target.

Omitted hitflag defaults to MAF. Explicit empty and Z strings are accepted but
hit nothing. Quoted MAF and lowercase maf are accepted. Preserve explicit empty
separately from omission, including zero-valued programmatic HitDefs used by
the direct geometry adapters.

A new hit preserves an existing H-state fall even when the new fall/air.fall
values are zero. ForceNoFall clears that continuation. A hit's own fall or
air.fall setting takes precedence over ForceNoFall. Idle and attacking target
controls distinguish a fresh get-hit sequence from continuation: their new
fall setting is used. The falling flag restriction also applies to H, rather
than arbitrary stale fall metadata in idle/attack states.

ForceNoFall accepts signed nonzero integers. Computed 0.5 truncates to zero;
literal float rejects. Bottom becomes zero without disabling the attack.
Explicit zero and bottom therefore preserve the existing fall; -1 clears it.

## Implementation

`character/hitflag.go` gates the common collision path before protection,
clash selection and contact mutation. `HitFlagSpecified` preserves the empty/
omitted distinction. Root, helper and existing projectile collision adapters
share that path. `prepareContact` selects 5080 for L targets, preserves valid
H fall continuation and applies typed ForceNoFall. It does not invent juggle
budgets or alter sprite projection to make the probes pass.

## Validation

The original 49-program pre-fix run fails 35 cases in
`temp/m4at-before-flags.log`. Native evidence is recorded in
`temp/m4at-native-first.log` and `temp/m4at-extension-native.log`. The eighteen
extensions pin combined modifiers, AF/MF, non-H fall context and ForceNoFall
typing/precedence. All 67 integrated focused cases pass in
`temp/m4at-fields-flags.log`; pure schema/engine/evaluator races pass in
`temp/m4at-final-pure-race.log`.

The full scoped race suite passes in `temp/m4at-full-race.log` (Match 292.855
seconds). Six fresh native replays pass: MF against falling H, A against idle
fall metadata, downed 5080, fractional/negative ForceNoFall and literal-float
rejection. Nine final 1280x720 captures from one identical binary pass prepared
source/definition, binary/config/PNG/dimension/nonblack/runtime/resource checks.
All five stable white clothing masks match exactly: 11,181 pixels each. Stock
throw recovery retains two fighters without persistent dust clones. These
remain actor-mask and smoke comparisons, not full framebuffer parity. Evidence:
`temp/m4at-app-checks.json` and `temp/m4at-app-checks.log`.

Full juggle budgets, guard/fall, custom/paused/foreign/simultaneous flag
combinations, legacy direct HandleHit equivalence and remaining HitDef/schema/
geometry/projectile/team/asset/mode gates remain open. Reversal: 89b5a501 (M4as).
