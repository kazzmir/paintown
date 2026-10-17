# Forced standing and get-hit velocity coordinates

M4au uses 38 original `forcestand-*` programs: 540 native numeric observations,
two load rejections and 152 authored CNS/AIR/CMD/stage hashes. Neutral authored
states isolate posture and velocity values from common recovery scripts;
separate facing/HitVelSet controls also pin actual initial velocity and motion.

## Measured behavior

Forcestand defaults true when evaluated ground Y velocity is nonzero, for either
sign; otherwise it defaults false. Explicit zero overrides that automatic
choice. Signed nonzero values force a crouching receiver to S before ordinary
get-hit state selection (5000 rather than 5010). Inherited-U and explicit-C
destination controls distinguish the contact change from later Statedef entry.
Standing, airborne and downed receivers retain their appropriate state type.

Eligibility uses the original posture: L can hit a crouching receiver and then
force standing; H alone cannot turn a crouching miss into a hit. Guarding keeps
the crouching state and uses 152. HitOverride still applies forced standing,
including an idle destination that suppresses damage. Literal float rejects;
computed 0.5 truncates to zero, and bottom supplies zero.

Ground.velocity expressions compile and evaluate as float32 at HitDef activation.
Computed integer/fractional components, enemy redirects and a single X component
are accepted; missing Y becomes zero. Bottom supplies zero while retaining the
attack. Side effects independently establish that explicit forcestand evaluates
before ground.velocity, regardless of written order; X then Y evaluate in order.
The native counter sequence gives ground X=2/Y=3, rather than X=1/Y=2.

GetHitVar(xvel) is world-oriented, expressed in receiver-local units. Its sign
depends on the attacker's contact heading, not the receiver's facing. With
positive ground X=2, a right-facing attacker produces -2; a left-facing attacker
produces +2. HitVelSet converts that world X into the receiver's facing-relative
Vel X. Independent same-right/same-left/reversed controls pin initial velocity,
late velocity, getter values and positions (-132/+132).

HitVelSet flags select axes; neither 2 nor -1 multiplies velocity. Zero, computed
0.5 and bottom preserve the existing component. Literal float rejects in state
5010. Y flags likewise select the -3 get-hit Y rather than doubling/reversing it.

## Implementation and validation

Optional ForceStand preserves the automatic/explicit-zero distinction. Contact
resolves its default from unscaled ground Y before changing the receiver's
posture. Ground.velocity uses compiled float cues with float32 serialization.
The receiver snapshots attacker heading for the world-oriented X getter.
One typed HitVelSet compiler handles flags and the coordinate conversion; the
old registry adapter delegates to it.

The initial 23-program comparison fails 14 cases in
`temp/m4au-before-posture.log`. The first prototype's 28-program run preserves
two parameter-order and two getter-sign failures in `temp/m4au-before-order.log`.
The expanded native directions/flags are in `temp/m4au-facing-native.log` and
`temp/m4au-hitvel-native.log`; initial/phase captures are in
`temp/m4au-native-first.log` and `temp/m4au-extension-native.log`.
Pure races pass in `temp/m4au-velocity-pure-race.log`. All 38 corrected integrated
cases pass in `temp/m4au-world-fields.log`. The full scoped race suite passes in
`temp/m4au-full-race.log` (Match 320.408 seconds). Six fresh native replays pass:
inherited/explicit entry, parameter order, same-left world/facing velocity,
computed-zero axis and literal-float rejection. Nine real 1280x720 captures
from one identical binary pass source/definition/binary/config/PNG/dimension/
runtime/resource checks. Five stable clothing masks match exactly: 11,181,
11,181, 11,181, 16,563 and 8,379 white pixels. These include horizontal motion
to the clipped edge and vertical motion. Stock throw recovery retains two
fighters without persistent dust clones. Results: `temp/m4au-app-checks.json`
and `temp/m4au-app-checks.log`. No whole-frame renderer claim is made.

Full air/guard/down velocity schema, arbitrary cross-field side-effect ordering,
paused/foreign/helper/multiple contact combinations, all recovery/juggle rules
and complete engine/asset/mode compatibility remain open. Reversal: 8ab6f50d.
