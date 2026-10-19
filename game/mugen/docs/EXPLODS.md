# Explod resources, lifetime and owned rendering

M4ae adds 27 native programs: 636 numeric observations, two load rejections
and nine original RGB marker placements. The typed Explod path replaces the legacy literal parser
that converted F(expression) to own action 0 and left it indefinitely visible.
That defect made stock landing dust appear as permanent standing KFM sprites.
The repaired draft stock throw capture at
`temp/app-runs/stock-throw-effects-draft-go/at-7s.png` has two fighters and no
permanent extra poses. The draft capture was inspected; final window validation is recorded with the
checkpoint. Eight fresh native lifetime/grammar/RGB replays match.

## State and rendering ownership

The character owns each effect's ID, animation clock, active flag and lifetime.
The shared effect scene retains that same instance for rendering; it does not
advance its clock a second time. Removing an object makes it immediately
inactive for counts/drawing, even if the scene has not pruned its pointer yet.
Scene retirement also retires active and queued owned instances. Ordinary hit
sparks retain their existing scene-driven one-pass policy.

Character updates publish owned effect expiry after all root CNS/physics/push
passes. The native cross-root program sees the previous active object from a
later root before that shared expiry. A new effect keeps its initial frame;
zero removetime retires at the end of its creation tick. For a five-tick action,
CNS counts it through local Time=5 and sees zero at Time=6. With removetime=1,
CNS sees it at Times 0/1 and zero at Time=2. Indefinite finite actions loop;
indefinite AIR frames remain active. Removed records do not reappear at flush.

## Native lifecycle and parameter evidence

Eighteen programs compare 528 distinct per-program observations and two load
rejections. They distinguish default, zero, positive, -1, -2 and values below -2 for lifetime,
finite versus infinite AIR, common F(expression) versus own actions, missing
actions, defaults/negative IDs, counts, cross-root expiry and immediate removal.
The common case defines an own action 60 that is infinite; native F60 instead
expires using the real common bank, distinguishing resource lookup from a
literal fallback.

Omitted/negative IDs become zero. Omitted NumExplod ID and negative query IDs
count all; negative RemoveExplod IDs remove all. Selected removal is visible
immediately to following controllers. These are per-character records; complete
helper/root-family and entity retirement behavior remains open.

Direct float literals for anim/removetime are native load rejections. Despite
the manual labeling anim required, the beta accepts omission and creates no
object. Missing own/common actions also create none. Go retains contextual
warnings for unavailable actions. Exact warning text/frequency is not compared.
All parameters compile before any expression assignments/effects execute;
shared controller source remains immutable across hosts.

## Geometry and image evidence

The existing original RGB ramp marker is used with blank fighters and a static
320x240 stage/camera calibration. Own and common archives are separately bound
in Go. Nine native captures compare own/common none-stage placement (20,-30),
P1 (-50,-30), P2 (50,-30), screen (100,100), both front-edge directions,
back-edge facing left, and an explicit-stage front-edge case. The shared
calibration stores anchors relative to the 320x240 stage center/ground; screen
anchors are converted before comparison. These probes measure position/facing/resource
selection, not full framebuffer equivalence.

The first front-edge probe combined postype=front, explicit space=stage and
positive Y=100; its marker was off-screen. That raw capture is diagnostic.
The promoted implicit front/back cases use screen Y relative to the top.
An explicit-stage control with visible Y=-30 retains that stage Y while using
the screen edge for X: its measured stage anchor is (60,-30). This differs
from the implicit front case (60,-100). Go preserves the explicit-space flag
and converts the horizontal edge origin accordingly. These are static camera
controls, not complete camera/zoom or every space/postype combination.

All 18 lifecycle programs and nine RGB-position/source-bank cases pass through
the production simulation. Full scoped races pass with all markers in
temp/m4ae-full-markers-race.log. Pure compiler/effect tests cover host isolation,
pre-effect validation, one shared clock, immediate retirement and scene cleanup.

The renderer uses common/own archive selection and active AIR elements rather
than the old own first-frame draw. Negative stock dust priority is drawn below
actors; ordinary effects and ontop use separate passes around foregrounds.
Complete relative actor/effect sorting, camera/zoom and all aspect/filter cases
remain open. Other geometry/control fields currently have implementation-level
coverage only, not complete native verification.

## Remaining controller surface

Explicit bindID currently reports an unsupported-binding error; complete bind
controllers, foreign/helper/multiple-target geometry and pause/hitpause budgets
need native traces. Full ModifyExplod/ExplodBindTime, palette ownership/remap,
PalFX, shadow, random displacement, angle/3D transforms and transparency overrides
are not covered by this checkpoint. Full Explod, engine and asset compatibility
remain unfinished. See THROWS.md, EFFECTS.md and COMPATIBILITY.md.


M4af adds two fixed nonzero-camera marker controls for Pos X + CameraPos X,
using own/common banks. Both project to native (30,-10) with camera (80,-20),
and the production scene and real windows retain that position. Pos X/PosSet
now convert between screen-centered scripts and stage-based actor storage.
See COORDINATES.md. These controls establish the coordinate contract; dynamic
camera follow/easing/zoom and full framebuffer filtering remain open. Do not
accept complete stock visual/engine parity from a clean recovery frame alone.


All final normal/forward/drop 1280x720 windows were inspected through 2/3/4/7
seconds at temp/app-runs/stock-throw-{base,forward,drop}-effects-go/. They recover
with two fighters and no permanent dust clones, without missing-effect warnings
or runtime errors. The ordinary throw was also repeated in
stock-throw-base-effects-recheck-go/. A black preview was a display artifact:
the stored original/repeat recovery PNGs have identical hashes and substantial
nonblack pixels. app_smoke.py now rejects entirely black quick-match captures.
These remain window/asset smoke checks, not full camera/filter/layer/audio parity.
