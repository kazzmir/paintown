# Physics and landing evidence

M4t compares 18 original programs against the supplied beta: 17 accepted cases
pin 336 numeric observations, and one case pins a shared-budget runaway error.
Each CNS/AIR/executable hash is retained. The native harness prepends the stock
KFM constants; Go uses the same constants, local width 320 and initial X=-70.
Blank sprites and early snapshots avoid contact/body overlap. These are focused
physics/state observations, not complete world or graphical parity.

## Movement, forces and landing

Native integrates position using the existing velocity, then applies Physics A
acceleration or S/C friction. With Y=-1 and VY=1, the first movement reaches Y=0;
VY then becomes float32-observed 1.44000006. The engine enters 52 immediately,
whose Time=0 CNS sees X displacement 2, Y=0 and the retained VY. With Physics N
in 52, its resumed physics moves again in the same tick. Next tick's Time=1
snapshot sees X displacement 4 and Y=1.44000006. There is no extra negative-state
pass. The final state's time and animation clock advance once.

The engine does not clamp landing Y or zero VY. A PosFreeze on the launch tick
leaves Y=-1 while acceleration changes VY. The next movement reaches roughly
0.44; 52 sees that overshoot and VY=1.88. The following snapshot sees Y about
2.32. MoveType H takes the same Physics A landing route. If 52 immediately uses
ChangeState to 800, 800 runs Time=0 in the same fixture tick and sees the same
position/velocity before its resumed physics.

Standing friction is .85 and crouching friction .82 in this stock setup. Both
move X by 2 before changing VX: native observes 1.70000005 or 1.63999999. At the
next snapshot the displacement is 3.69999695 or 3.63999939. Pos X returns float32
before subtraction from the stored origin; starting the Go fixture at zero
would hide this rounding. Full internal floating-point edge behavior is open.

State 105 is exempt from automatic Physics A landing. A custom 105 at Y=0/VY=1
still occupies 105 next tick, at Y=1/VY=1.44000006. Its authored/common CNS can
choose backhop landing 106. A custom 50 with Physics N does not auto-land.

## CNS ownership and continuation budget

Custom states 40, 50, 51, 52, 100 and 105 with Physics N and a finite looping AIR
action retain their state at Time=1 and Time=5. Go's previous forced jump-down,
landing-return and run-release exits overwrote that script ownership. Those
exits are removed; the stock common CNS supplies jump and landing transitions.

An explicit StatePassBudget counts CNS transitions across positive chains and
engine resumes within a tick. Negative-state transitions, engine landing entries
and terminal passes do not consume that counter. The displayed native loop bound
is 2500 with the post-increment behavior documented in REFERENCE.md. A nearly
full CNS chain can land and continue to terminal 705; adding one more transition
fails at pending 706. The headless test checks this budget without pretending to
simulate physics; production Character checks all captured values.

Two programs repeatedly enter 52 through Physics A, reset velocity there and
switch physics to N after 3 or 2510 entries. Both finish in the same fixture
tick, with Time=0 and PrevStateNo=52. Thus a limit on engine resumes derived from
the CNS transition count would incorrectly reject valid native behavior.

## Validation and remaining work

Full scoped races, headless engine races, and fresh frozen-landing/shared-budget
overflow replays pass. The actual production Match test injects DirUp into both
KFM and KFM720, requires airborne motion and landing 52, and checks controlled
idle with Y/VY zero afterward. Existing match combat/resource/clock tests pass.
Owned production-window captures at temp/app-runs/physics-stock-match/ were
inspected at 6 and 12 seconds: both mixed-resolution fighters, stage and HUD
render at the requested 1280x720, with the match timer advancing. Binary/config/
capture hashes are retained there. That smoke has idle inputs; jump coverage
comes from the production Match input test, not those images.

Remaining: full root/entity phase scheduling, body/corner/width and binding
interactions, hitshake/Pause/SuperPause, friction thresholds, all numeric edges,
gravity/controller combinations, other special state-number rules, custom-state
ownership and native moving-camera cases. Engine resumptions that run forever
without CNS transitions are not bounded by the CNS guard; finite repeated-entry
coverage does not establish a separate native guard for that case.
