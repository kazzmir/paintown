# Screen and stage boundaries

M4ah targets the supplied M.U.G.E.N 1.1 beta 1 executable. The `screenbound-*`
programs use authored CNS/AIR/stage files in disposable reference copies. They
isolate horizontal constraints from hurtbox push with a box-free infinite AIR
frame and Physics=N. This checkpoint does not establish complete camera,
helper, Width, binding or pause compatibility.

## Root position constraints

A root normally stays between the stage's PlayerInfo leftbound/rightbound and
the visible screen margins. On a 320-wide stage with a fixed zero camera and
15-unit margins, setting X to 300 records 300 in the issuing CNS pass and 145
on the next pass. Setting -300 produces -145. Asymmetric 7/23 margins give
-153/137. A fixed camera at 80 preserves the same script-space 145 edge; storage
remains absolute, as described in COORDINATES.md.

The constraints apply after the actor's position integration, including when
PosFreeze prevents integration. At X=140 and velocity 10, the next position is
145 while velocity remains 10. The tests also distinguish stage limits of
+/-50 and a 640-wide stage. Stage margins convert through stage and actor
coordinate widths: a 15-unit margin from the 640-wide stage becomes 7.5 in a
320-wide actor, giving an edge of 152.5. A 1280-wide actor on the 320-wide stage
has the corresponding edge at 580.

`ScreenBound` is now a compiled typed controller. Omitted `value` and omitted
`movecamera` default to zero, unlike the normal root defaults when no controller
runs. `value=0` disables both screen and PlayerInfo confinement: a position of
3000 remains outside the stage's nominal +/-1000 bounds. `value=1` enables
confinement. A one-time value=0 at Time=0 produces positions 300,300,145,145 at
Time=0,1,2,4; the default is restored during the actor's next CNS pass. Later
controllers replace earlier settings in that pass.

## Camera participation

The normal root participates on both axes. `movecamera` independently selects
X and Y participation, regardless of the confinement value. A stationary actor
at absolute X=130 and the other at 30 gives camera X readings
0,4.56401062,8.47706604,14.7084808 at Time=0,1,2,4. Disabling the first actor's X
participation keeps the camera at zero; enabling X preserves the original trace,
even with confinement disabled. Omitting movecamera disables both axes for that
controller's tick. A Time=0-only controller delays the ordinary camera trace by
one actor pass.

The beta has an additional coupling: if either axis has no participating actor,
the camera stops updating on both axes. Both actors continuously issuing
movecamera=1,0 therefore freeze horizontal motion once the second actor disables
Y. A transient first-root step remains observable. This is measured behavior,
not an assumption that X and Y tracking are fully independent. CAMERA.md retains
the separate horizontal easing evidence; complete vertical following and zoom
are still open.

## Normal helper controls

A normal helper is unbound by default. Its Time=0/1 positions remain 300 with
no ScreenBound controller or with explicit value=0. Explicit value=1 gives
300 then 145 on the same fixed stage. Helpers now inherit the stage limits
and margins in their inherited coordinate space, and restore their unbound
default at each own state pass. These controls use helpertype=normal, keyctrl=0,
one child, and no boxes. They do not establish player-helper behavior, helper
camera participation, complete typed Helper parameters or global child phases.

## Parameter and stage grammar

The controller's fields use integer expression boundaries. Literal 1.0 is
rejected for value or movecamera. Computed .5 truncates to zero; negative one is
true; bottom is consumed as zero. A single movecamera component, including a
trailing comma, defaults Y to zero. An empty first component or three components
is rejected. All expressions are compiled before effects; a malformed later
component cannot execute an assignment in an earlier component. Compiled source
is immutable and shared hosts retain their own evaluation context.

Both stage [Bound] attributes must be present; missing either or both causes a
native load rejection. Zero and negative values are accepted despite the manual's
positive-value guidance. Fractional margins truncate toward zero: 23.5 behaves
as 23. Present empty or nonnumeric values behave as zero. The port preserves
these measured rules and reports missing attributes with the stage path and
[Bound] context.

## Evidence and validation

Full scoped races pass in temp/m4ah-full-race.log and pure engine/evaluator
races pass in temp/m4ah-pure-race.log. The focused boundary race run passes in
temp/m4ah-screenbound-final-race.log.

The 47 programs pin 924 numeric observations, four controller load rejections,
three stage load rejections and all 141 authored CNS/AIR/stage input hashes.
TestNativeScreenBoundaries loads each authored stage and program through the
production Match path, resets the round, advances through control grant and
90 further ticks, then compares every captured fvar at float32 precision.
The helper programs compare values copied back through Helper(123) redirection.
Rejected programs must produce source-context errors in Go. Pure engine tests
also exercise immutable source, host-local expressions and compilation before
side effects.

Native inputs retain their captured bytes, including final blank CNS lines and
the empty-margin assignment's trailing space. Whitespace-only cleanup would
change their pinned hashes; implementation and documentation pass diff checks.

Six fresh native replays reproduce reset, constant coupled-axis freezing,
normal-helper confinement, float-literal rejection, 640-stage scaling and a
missing-margin stage rejection. Real 1280x720 windows at
`temp/app-runs/screenbound-motion-go/` and `screenbound-stock-throw-go/` run at
2/3/4/7 seconds. All eight PNG hashes/nonblack checks pass, with no runtime or
missing-resource errors. The stock throw recovers with two actors and no
permanent dust clones. The box-free motion scene eventually overlays the two
actors at the right limit; its minimal background still ends during scrolling.
These are window/asset smoke checks, not full native framebuffer parity.

## Width follow-up

M4ai adds native temporary edge offsets and eight edge query bindings, including
stage-margin subtraction and mixed-profile redirection. See WIDTH.md. Its
selected ground push controls supersede the unimplemented Width limitation
recorded here; complete Width/pause/bind/helper/zoom coverage is still open.

## Remaining comparisons

Hitpause/world-pause reset timing, constraints during active binds, dynamic Width,
body-push interactions, malformed combinations beyond the pinned controls, stage
bound and screen edge conflicts, helper camera eligibility, player helpers,
projectiles, zoom and complete vertical camera motion need native comparisons.
The previous moving-window clipping was partly a missing margin; background,
filtering and complete frame equivalence remain separate renderer obligations.
