# Helper binding completion, retirement and hit facing

M4am compares 27 authored helperbind programs: 564 numeric observations, one
intentional load rejection and 108 CNS/AIR/CMD/stage hashes. Source and native
recipes are in tests/testdata/reference/helperbind-*. This extends the root
TargetBind evidence in BINDING.md; it does not close the full binding gate.

## Anchor completion and the observable flag

A TargetBind request leaves the caller's immediate position/flag queries
unchanged. Position, velocity and the flag publish when the anchor completes
its movement, before another entity runs. The prepost/post-only controls show
both helper queries at zero, a later root query at one, and the victim's saved
own-CNS flag still at zero before that victim has run. This distinguishes anchor
publication from both immediate controller mutation and target-only publication.

A victim's own GetHitVar(isbound) reflects whether its anchor was attacking at
the tick's ordering boundary. Changing the anchor to I during its current pass
affects the next tick. An idle anchor still moves its bound target when it
completes; the victim's own flag is zero and its physical movement stays
constrained. Its own pass can leave a zero flag visible until the anchor's next
completion publishes one. The earlier M4ac subject-last-update description
could not distinguish these phases with only two actors; this corpus does.

Changing-speed controls separate binding from retained free velocity. In the
idle case the victim's first unbound-looking CNS still sees the prior bound
position; the later helper's completion updates position and velocity again.
The active/idle snapshots pin exactly what a third root and the helper can read
between passes. Four independent H/A/I controls show no general hit-state
priority change: existing attacker-first ordering remains intact.

## Helper retirement and forced state 5050

Removing a live anchor releases its binding and queues an external transition
to 5050 for its H victim. The victim keeps its current position/velocity. A
borrowed but unbound victim remains in state 810 after helper removal, so the
fall is a binding consequence, not blanket retirement of foreign state data.
Both own and borrowed bound states take the fall path.

If the helper is still attacking, its pass precedes the victim: the new state
runs at GameTime 134 in the changing-speed control. If the helper has become
idle, the victim already completed its pass: entry runs next tick, at 135.
Both controls enter at X=61,Y=-5,Vel X=-5, with previous state 810. Pre-retirement
snapshots distinguish that retained constraint result from a second movement
step or a special H update order. A defined 5050's type/anim/ctrl attributes
apply; ctrl=1 and anim=9994 remain observable. GetHitVar(fall) stays zero in the
separate constant-speed control. Undefined 5050 is accepted as an external
missing state with a warning and retained attributes.

State ownership after forced fall still needs an independent opponent-file
control; the current own/borrowed programs share definitions. Full death/KO,
attacker-hit interruption, bound helpers/teams, pause and generation edges are
not inferred from these traces.

## Hit facing exposed by the grab

Default hit facing stays unchanged, including attacks from behind. The old
unconditional turn-toward-contact code gave the wrong local inherited velocity.
The corpus includes normal and throw attributes, reversed victim/attacker
headings and explicit p2facing controls. The typed field rejects literal 1.5.
Values +1/-1 request opposite/same attacker heading in the measured beta; the
change follows the victim's first CNS, and preserves world velocity. A value 2
leaves facing unchanged. These native sign/timing observations take precedence
over a literal reading of the manual's same-direction wording.

The first hit-state CNS records the old facing; its next pass sees the requested
facing and corresponding local velocity. P1 facing/get-P2-facing precedence,
guards/overrides, multi-contact conflicts, changing attacker orientation and the
remaining field grammar/bottom/version cases remain open.

## Validation and remaining work

The preserved implementation fails the original ten programs in
temp/m4am-before-binding.log. The first integrated 27-program comparison has
one flag mismatch in m4am-helper-first-race.log; a helper reading its already
updated idle victim must see zero until its own completion. All 27 pass focused
races in m4am-helper-second-race.log. Pure engine/evaluator races pass in
m4am-pure-race.log. Full regression, window and fresh replay results are recorded
with the checkpoint when complete.

Disposable authors/catalogues: temp/author-helper-bind*.py and
helperbind-*-cases.json. The first rejected drafts used unsupported IsBound;
accepted inputs use GetHitVar(isbound). Those rejects remain only under temp,
not in the passing corpus. The intentional p2facing float rejection identifies
expected_compile_state=800. Stage camera is fixed at 80; push is disabled in
the contact programs so it cannot masquerade as binding movement.

Still required: BindToParent/Root/Target, paused/frozen/foreign/mixed-unit binding,
multiple targets, target CNS overrides between different update orders, binding
cycles, helper/projectile contacts and complete retirement/combat lifecycles.

Seven real 1280x720 app captures pass hashes/dimensions/nonblack/runtime-log
checks: temp/app-runs/helperbind-idle-fall-go/ (3/5/7 seconds) and
helperbind-stock-throw-go/ (2/3/4/7). The stock throw recovers with two fighters
and no permanent dust clones. The helper probe takes the native fall path;
its source intentionally omits standard state 5000 and retains that native
warning. The stationary left sprite's clothing mask matches exactly. The late
clipped right sprite starts at x=1003 in Go versus native 1004, with identical
vertical bounds; white counts are 8316/8229 and the full mask differs at 801
pixels. Late numeric position, mirrored placement and clipping still need an
independent calibration. This is not full framebuffer parity. Native debug
magenta/Go black and minimal background filtering differences remain open.
Measurements: temp/m4am-app-comparison.json.

Full scoped races pass in temp/m4am-full-race.log. Fresh flag-prepost,
fall-idle-speed, borrowed-unbound and facing-reverse-positive replays match all
observations/hashes under m4am-replay-*; facing-float is verified separately as
m4am-replay-facing-float-typed. The first wrapper omitted a required --pages
value for an empty rejected packet; that argument-only failure remains in the
log and the saved recipe now supplies page 0. No native discrepancy remains in
the advertised numeric corpus.
