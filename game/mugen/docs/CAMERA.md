# Horizontal camera following

M4ag replaces averaging the two fighter positions with the reference's measured
horizontal tension, retained goal, easing and root update points. The native
programs use authored 320x240 stages, tension=60, zoom=1 and no hurtboxes/input.
This isolates camera behavior from body push, command handling and zoom rules.
The real Match scheduler executes the same authored CNS/AIR/stage inputs in Go;
it retains stock constants/resources and runs through round control grant.
Twenty programs pin 720 numbers: inside/below/boundary/above, right/left,
spread, phase, long, return, left/right limits and eight motion variants.

## Goal and easing

The camera stays still while both actors are inside the tension region. Its
goal moves only far enough to contain them within that region, or to their
midpoint when their span exceeds it. Stage camera limits clamp the goal.
The goal is retained independently of the currently displayed camera position.
Moving the fighters back inside does not cancel easing toward that valid goal.

For the measured 320-wide view, the region extends 100 units on each side of
the center. X=99 and X=100 cause no movement; X=101 starts a move that stops
exactly at camera X=1. A right actor at X=130 sets goal=30; left X=-130 sets
goal=-30. Actors at -110/+130 set goal=10. Camera limits of -10/+10 bound the
corresponding goals before easing.

Each camera step moves 5% toward a point two 240p units beyond its goal, then
clamps at the goal. The left edge is rounded to float32 between steps, and
scripts expose the center. Rounding a center-only calculation gives different
native numbers near zero. For the right move, successive left-edge steps from
-160 toward the biased point -128 yield camera centers 1.60000610, 3.12001038,
4.56401062. Long traces reach exactly 30 rather than drifting toward 32.
Read-only inspection of the supplied executable's floating-point operations
also supports the 0.95/0.05 blend; no executable/disassembly is tracked.

## Moving actors

An extreme actor moving farther beyond its tension edge imposes a minimum
camera speed in that direction, clamped at the camera goal. Teleports alone
did not expose this rule. With the rightmost actor moving right at two units
per tick, camera readings begin 0,4,10,16,22,28,34,40,46,48,50,52. The two later
steps per issuing frame and three in later frames account for the initial
catch-up; once the camera reaches its moving goal, it follows at two per tick.
Moving only the interior left actor leaves the static right-edge easing packet
unchanged. Moving only the extreme right actor gives the same two-unit packet.

Speeds .5/1/3 distinguish ordinary easing from the speed floor. At .5 the
initial easing dominates; later increments are 1.5 per frame until the goal
catches up. The mixed-profile program uses P2 localcoord 1280x960 and speed
eight in its script units, and produces the same physical two-unit camera trace.
The mirrored leftward program begins 0,-6,-12,-18,-24,-30,-36,-42,-46: the
earlier moving root activates all three steps on its issuing frame.
All motion programs use Physics=N; forces, airborne movement, constraints and
the entire fractional precision surface remain separate obligations.

## Root and shared update points

The camera advances after each completed root CNS/physics update, then once
again in the shared late match phase. In the right-move program P1 moves to 30
before P2 moves to 130, so only two of the issuing frame's steps move the camera.
P1 reads 3.12001038 on its next Time=1 pass; after P1's update, P2 reads
4.56401062 in that same frame. Moving the earlier root left instead produces
three active steps and P1 reads -4.56401062 at Time=1. The phase program also
checks both actors' unchanged absolute stage positions.

World.AfterRootUpdate exposes that existing completion point to Match, without
creating another state executor. Nested helper/projectile adapters do not call
the root observer. Direct state-only worlds have no observer. Camera changes
publish through the shared viewport and cached actor camera fields before the
later root evaluates position/screen/sound coordinates. The goal resets with
the round's stage camera start. Shared late body push remains in its own phase.

## ScreenBound follow-up

M4ah compares root camera eligibility, per-pass reset and replacement, plus the
beta's coupled-axis empty-participant rule. The horizontal camera uses only
eligible roots. A nonempty X set and an empty Y set freeze X as well; constant
and time-dependent controls distinguish this from easing or clock reset errors.
SCREENBOUND.md records the 47-program boundary corpus and exact values. Root
and ordinary helper screen confinement now includes converted stage margins.
The following M4ag notes describe that checkpoint's historical window results;
its missing-margin limitation is superseded by M4ah. Background/filtering and
complete frame equivalence remain open.

## Scope and remaining work

All 20 packets pass through TestNativeCameraFollowing, including their 60
authored input hashes. Full scoped races pass in temp/m4ag-full-final-race.log,
retaining all previous contact/throw/guard/pause/binding/ownership/effect and
fixed-camera regressions. The old averaging implementation's four failing
programs are preserved in temp/m4ag-before-camera.log. The initial ten static/
phase programs passed before motion was added; walking then exposed the missing
speed floor, which the additional speed/one-sided controls distinguish.

Four fresh native phase/return/slow-motion/stage-limit replays match all pinned
values and input hashes. Real 1280x720 windows were inspected under
temp/app-runs/camerafollow-motion-go/ at 2/3/4 seconds and
temp/app-runs/stock-throw-camera-go/ at 2/3/4/7 seconds. All seven captures have
checked hashes/nonblack pixels and no runtime/missing-resource errors. The
stock throw still recovers with two fighters and no permanent landing-dust
clones. The motion scene uses an intentionally minimal single-image stage;
its background ends during scrolling, and P2 can be clipped at the right edge.
Full background/debugbg rendering and native screen boundary margins remain
open. These windows are smoke checks, not full framebuffer parity.

The camerapos-* and cameramark-* fixed-camera evidence remains separate; see
COORDINATES.md. These horizontal comparisons do not establish vertical camera
following, 1.1 tensionhigh/tensionlow, zoom, every stage/profile/default/invalid
value, arbitrary fractional movement precision, full screen clamps/ScreenBound/movecamera,
helpers/projectiles, attack-priority ordering or pause/hitpause interactions.
The renderer's native filter differences also remain open.
