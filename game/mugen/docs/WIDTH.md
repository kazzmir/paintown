# Width and horizontal edge queries

M4ai replaces the Width stub with typed expression compilation and separate
frame-local edge/player offsets. Thirty-one authored native programs pin 336
numbers, three load rejections and 93 CNS/AIR/stage input hashes. The programs
run against disposable copies of the supplied 1.1 beta 1; they do not establish
complete Width, body-push, camera or entity compatibility.

## Measured offsets and timing

The beta adds Width's edge values to the stage's screen margins. On a fixed
320-wide stage with margins 15/15, value=30,10 or edge=30,10 changes the right
limit from 145 to 115 and the left limit from -145 to -135 for facing right.
Facing left exchanges the offsets' physical sides; the right limit becomes
135. Negative edge offsets remain negative: value=-30,-10 permits X=175.
Offsets apply after physics while queries see their new values immediately.

Player offsets affect body pushing independently. Two facing actors at -10/10
with active hurtboxes end at -16/16 by default. Adding player=40,10 to the first
actor moves the final positions to -36/36; value=40,10 does the same. Edge-only
values retain -16/16. Player=-10,-5 reduces the final separation to 22, at
-11/11. The tested poses use ground state types and Physics=N, with no active
hit or velocity. These results do not establish complete movement or airborne
push behavior.

Offsets reset before the actor's next state pass. A Time=0-only value=30,10
and velocity 10, starting from X=300, produces positions 300,115,125 at
Time=0,1,2. The next pass's FrontEdgeBodyDist is 30 rather than zero because
the temporary edge offset is already gone. A later edge-only controller also
clears an earlier controller's player offsets: value=40,10 followed by edge=5,6
returns the push result to -16/16. Each controller replaces the complete pair
of offset vectors; source parameters are not mutated.

Width does not change P2BodyDist's base CNS widths. In the expanded push case,
72 units of axis separation minus the two 16-unit front widths gives body
distance 40. The reduced-width case reports -10 despite no remaining push
correction. A rear-opponent control also repairs a separate error: P2BodyDist
selects the opponent's nearer base edge geometrically, while measuring from
the caller's front. At X=145 with an opponent at 70, both queries return -76,
not a calculation using the opponent's farther front edge. Facing, base sizes
and both directions are explicitly captured.

## Edge queries

LeftEdge/RightEdge are absolute stage coordinates at the current camera edges.
FrontEdge/BackEdge select those edges by facing. With a camera at 80 on the
320-wide stage they return -80/240, swapped for facing left. Pos X stays
screen-centered; these absolute edges require the camera term in comparisons.

FrontEdgeDist/BackEdgeDist already subtract the stage's corresponding screen
margin in this beta, despite the manual's description in terms of the axis
and screen edge. At script X=30 they return 115/175 with margins 15/15, or
107/183 with asymmetric margins 7/23. FrontEdgeBodyDist/BackEdgeBodyDist further
subtract Width's temporary edge offsets. At the same X and edge=30,10 they
return 85/165 while the axis-distance queries remain 115/175. Turning exchanges
the physical margins. All eight edge queries return floats in script units.

The 1280-wide Width control on the 320-wide stage produces the equivalent
physical limit, X=460 in actor units. A separate mixed-profile Enemy redirect
checks all eight query values in the caller's units. Borrowed-state Width
storage conversion uses the existing state-owner adapter, but still needs its
own native comparison. Full moving-camera and zoom edge precision is open.

## Grammar and evidence

At least one of value, edge or player is required; the native executable
rejects an empty Width controller despite the manual listing no required
parameters. value is shorthand for both offset pairs only when edge and player
are absent. If either is supplied, value is ignored, even when it contains an
otherwise-rejected float literal. The explicit missing pair defaults to zero.

A single component defaults the back offset to zero. Literal 30.0 is rejected
by an active integer field, as is a third tuple component. Computed fractions
truncate and bottom becomes zero. Negative values are retained. Full tuple,
version and invalid-value grammar is not claimed by these selected controls.

TestNativeWidth uses the production stage/Match path and compares every captured
fvar at float32 precision. Source hashes are checked even for rejected programs,
which must fail with file/state/controller context. Pure tests check shared
source/host isolation, compilation before effects, and evaluating shorthand
components once before copying them into both vectors. Native expression-side-
effect evaluation order is still a separate comparison obligation.

Full scoped races pass in temp/m4ai-full-race.log; all 31 programs participate.
Pure engine/evaluator races pass in temp/m4ai-pure-race.log. The initial 26
programs also pass focused races in temp/m4ai-width-first-race.log.

Four fresh native reset/negative-offset/replacement/mixed-redirect replays
reproduce their pinned observations and input hashes. Real 1280x720 windows
at temp/app-runs/width-push-go/ (2/4/7 seconds) and width-stock-throw-go/
(2/3/4/7) complete without runtime/missing-resource errors. All seven capture
hash/dimension/nonblack checks pass. The push scene shows the expanded spacing;
the inspected stock recovery retains two fighters and no permanent dust clones.
The minimal probe background and full filter/framebuffer equivalence remain
renderer obligations; these captures are smoke checks.

## Remaining work

Native helper/player-helper and foreign-state Width behavior, pause/hitpause
reset timing, complete airborne/body-push/contact/edge interactions, zero-axis
and multi-actor ordering, turn and movement precision, zoom/vertical queries,
full stage defaults and every malformed tuple/version combination remain open.
No full controller, trigger, match or 1.1 beta compatibility gate is complete.
