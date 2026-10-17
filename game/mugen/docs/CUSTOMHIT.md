# HitDef custom states and throw integration

M4ac compares 38 original native programs, 1244 distinct per-program numeric
observations and three expected load rejections. This connects HitDef's
p1stateno, p2stateno and p2getp1state to the
state-owner and external-entry paths. Original two-character programs compare
against the supplied 1.1 beta, then replay through the production Character
scheduler. This is a bounded contact/ownership checkpoint; full HitDef, throw,
helper, team and world compatibility remain unfinished.

## Custom transitions

A successful, unguarded contact selects the victim's standard get-hit number
first and establishes H/no-control. A following custom state therefore records
PrevStateNo=5000 in the standing cases. The custom destination supersedes the
standard state's entry attributes; its animation change does not run. The
attacker's destination records its previous attack state.
The custom number and Time=0 are visible before either actor's negative pass,
but the custom entry attributes wait until its positive pass.

The attacker's -3 sees state 710, previous 700, old animation 9991, Ctrl=0 and
MoveContact=1. The positive destination then sees its entry animation 9100,
Ctrl=1 and the default reset MoveContact=0. The victim's continuing -2 retains
its prior
animation and sees Ctrl=0; its custom positive body sees Ctrl=1. A distinct
9200 animation and its element clock separate preservation from a reset to the
standard get-hit animation 9990.
Life is still 1000 in that first body, then 900 after pending damage is applied.

Omitting p2getp1state borrows the attacker's state definitions when p2stateno
selects a nonnegative destination. Explicit zero selects the victim's definitions. A negative P2 destination
does not transition, but its default borrowing flag still excludes matching
overrides; even p2stateno=-1 has that observed flag default. The two programs
use distinct markers 111/222, constants and coordinate profiles: the 1280-wide
victim at X=280 reports X=70 and height=15 in a 320-wide borrowed state, versus
X=280 and height=60 in its own state. Own -2 continues; foreign -3/-1 are excluded.
State ownership does not by itself replace the continuing animation source.
ChangeAnim2 remains the explicit way stock throw states select the attacker's AIR.

P1-only/P2-only, ignored flags without a P2 destination, negative destinations
and missing external destinations have separate programs. Negative destinations
leave ordinary state selection intact. Missing positive
external destinations change StateNo/Time and advance without a positive body.
They retain animation/control but reset default contact counters after the
negative pass: the attacker sees MoveContact=1 in -3 at Time=0, then zero at
Time=1. Go warns with player/state context. Integer fields reject direct float
literals at loading; expressions compile and coerce at runtime. A computed .5
flag truncates to zero and selects the victim's own state definitions.
Ordinary missing ChangeState/SelfState and foreign callers remain separate rules.

## HitOverride eligibility

A matching active HitOverride excludes custom hits with a nonnegative p1stateno
or with p2getp1state enabled. Explicit negative destinations do not transition;
p1stateno=-2 also does not exclude an otherwise ordinary overridden hit.
Exclusion occurs before contact: no MoveContact,
damage, custom transition or acquired target. An explicit p2getp1state=1 without
p2stateno still participates in this exclusion, even though it does not borrow
states on an otherwise successful normal hit. A nonmatching override does not
exclude the hit.

With p2getp1state=0 and no attacker custom transition, the ordinary override
path applies. The receiver enters its override state and takes pending damage.
HitOverride does not acquire a target even when its destination has MoveType=H.
This extends the earlier I-override observation and corrects the old H-only
acquisition condition. Guard, simultaneous trade and multi-entity corner cases
still require their own controls.

## GetHitVar(isbound)

The trigger is an integer. Its value publishes the result of the subject's most
recent update. A freshly issued TargetBind leaves a caller's immediate query
at zero; the target's own pass sees one, and an earlier root sees that published
one on its next pass. A duration-one constraint then expires physically while
that last published one remains visible until the subject updates again.
Two-tick and indefinite constraints distinguish this from a request-presence
or remaining-time test. While bound, 5/(isbound+1) returns integer 2.

Cancellation and TargetDrop do not immediately clear the last published flag;
the next subject update does. Recovery also leaves that last value visible
until the next update invalidates its binding generation. Pause, retirement,
foreign controllers and multi-target publication still need native controls.

## Infinite AIR clocks exposed by these states

A final -1 frame gives AnimTime=elapsed+1, including when finite frames precede
it. Same-action state entry and ChangeAnim both restart the clock. This replaces
the old documentation-based constant -1 AnimTime assumption.

The beta also accepts a nonfinal -1 despite the AIR manual restricting it to
the last element. For durations 2,-1,4, element start offsets are 0,2,1 while
the total resets at -1 and then adds 4, producing 3. The negative-length frame
has no display interval. Display elements repeat 1,1,3; AnimTime follows
-3,-2,-1,0,-2,-1,0. A ten-tick program independently checks current element and
element-2/3 elapsed times. Go retains this measured finite schedule rather than
holding the middle element forever. Other malformed schedules, initial-element
selection/offset queries, interpolation and rendering remain separate gates.

The first distinct-animation control had no Clsn2 on action 9200 and therefore
made no contact. That initial customhit-entry-native run is diagnostic only.
The corrected customhit-entry-contact-native packet has an explicit hurtbox,
pinned independent AIR hash and the observed 9200 clock. Other promoted inputs
and the supplied distribution remain unchanged.

Full scoped races pass in temp/m4ac-full-final-race.log; seven fresh native
replays match. The production 1280x720 custom-entry smoke renders both actors
through seven seconds, without runtime errors. Its authored victim uses
320-resolution art in a 1280 profile and is correspondingly small. This window
smoke does not establish framebuffer or stock throw parity. Root modules are
unchanged. See BINDING.md, OWNERSHIP.md and CHECKPOINTS.md.
The stock KFM/KFM720 regression window also runs at 1280x720 through seven
seconds at temp/app-runs/customhit-stock-regression-go/, with both full-sized
actors and no runtime errors. This remains a smoke check, not pixel parity.

Full stock throws still need facing/priority/Width and interrupted-binding checks;
full state-machine and asset compatibility are not declared complete.


M4ad subsequently runs the actual stock throw/common states and AIR, adds an
initial-grab stamp and fixes saved contact poses and omitted fall-X handling.
See THROWS.md. The normal/forward/drop paths now have native composition packets;
facing/hit flags/priority/Width and attacker-hit interruption remain open.
