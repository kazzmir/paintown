# Target binding and native push gating

M4ab adds 53 original native programs: 840 numeric observations and ten load
rejections. Typed TargetBind acts on acquired targets using an actor-owned
constraint with a registered anchor, lifetime and coordinate conversion.
The production Character scheduler compares every pinned packet. This covers
the measured root path; other binding controllers and full throws remain open.

## Timing, duration and geometry

The first TargetBind controller does not immediately reposition its target.
The caller still reads the old target position/velocity immediately after the
controller. Before the later target's positive CNS, the caller has completed
its movement/Time increment, and the target is positioned at that updated
anchor plus the binding offset. The target inherits the anchor's world velocity
expressed in the target's facing. The target does not add another movement step
in its bound frame. Own CNS and shared late body push can still change position.

A moving anchor at X=-68 with local velocity 2 requests pos=30,5 while facing
right. It finishes at -66. The target's CNS sees X=-36,Y=5,Vel X=-2 while facing
left. After release it keeps that inherited velocity. The initial constant-
velocity programs could not distinguish finite from infinite bindings because
the released target continued alongside the anchor. They remain diagnostic
captures for the initial time-0/2/3/forever/default/negative runs named
binding-target-CASE-native and binding-target-1-draft. Promoted duration runs
use binding-target-CASE-clock-native; later variant packets are independent.

The improved clock fixtures change anchor velocity at local Times 3/4/5/6 to
5/8/1/4. Completed observations distinguish duration one, two and three:
the released target retains local velocity -2, -5 or -8, respectively. A -1
binding continues tracking the changing anchor and inherits -1 then -4.
The seven-second clock captures use 24 fvars/pages 0 and 12. Their source hashes
match the revised CNS. Omitted duration matches one tick, -1 persists, and -2
falls back to one tick. Additional packets distinguish mixed units, a turned
anchor, ID filtering/no target, omitted position and renewal/cancel/drop.
Position defaults to zero; positive X follows anchor facing. A 1280-wide target
receives a 320-wide caller's offset and world velocity in its own storage units;
direct geometric queries convert back while saved fvars stay scalar.

## Hurtbox gating and shared push

An early inference that H excludes body push was disproved: the initial attacker
actions lacked Clsn2. Independent H/I and hurtbox-present/absent controls now
show both H and I push with hurtboxes, while an actor without Clsn2 does not.
The provisional H exclusion was removed before this checkpoint. Go requires
current hurtboxes on both actors, then uses CNS body widths for horizontal
extents, rather than Clsn2 width. The authored 60-wide hurtboxes still separate
according to the combined 32-unit CNS widths.

Binding does not suppress shared late push. With hurtboxes on both actors, the
bound target's CNS records X=-36 but shared push shifts it to -35 and its anchor
to -67. The following CNS observes those corrected positions. Independent
recovery-with-box and ordinary get-hit controls match too. Complete vertical,
AIR-frame, dynamic Width, helper/team and pause push eligibility remain open.

Binding applies before the target's continuing negative-state pass. The target
can override its position/velocity in CNS: the measured bound frame records
PosSet(100,12) and VelSet(7,0), and retains those writes rather than integrating
another movement step. Expiry retains that velocity. New requests renew the
constraint; time=0 cancels it. TargetDrop also cancels constraints issued by
that attacker. The first Go replay exposed that missing release and an older
Turn bug; both are corrected. Native Turn preserves world momentum by negating
local X velocity together with facing. A new hit cancels the target binding.
Leaving H invalidates the acquired-target generation and releases the constraint
on the next actor update; H-to-H state changes retain it. The current frame's
binding still prevents an extra movement step, and late push remains separate.

## Parameter compilation

Time/ID are compiled integer expressions, and position components are float
expressions. Native rejects direct float literals for Time/ID, including a
whole-valued 1.0, signed literals and parenthesized literals. Float-valued
expressions, fvars and IfElse are accepted and coerced at runtime; floor also
works. Literal rejection is deliberately distinct from runtime coercion.
Warning text/frequency is not established by these numeric captures.

Omitted position is 0,0. A single X or one trailing comma supplies Y=0. An empty
first component, a third numeric/empty/Head component and an empty Time are
native load rejections. Bare numeric bottom becomes zero for time, ID and
position. A redirected geometric expression with nested function commas is
compiled as expressions, not split as plain text. Invalid states/controllers
retain file/state/controller context, including expected load rejections.

## Validation and remaining work

Full scoped races pass with all 53 packets in
temp/m4ab-full-controls-race.log. The initial constant-speed and provisional-H
interpretations remain documented diagnostic drafts, not accepted explanations.
Seven representative fresh native replays match drop, mixed coordinates,
recovery-with-box, float-valued duration, negative fallback and two load-error
boundaries. The real 1280x720 Go match runs the with-box fixture through
seven-second captures at temp/app-runs/targetbind-with-box-go/. Sustained scripted
movement drives the actors to a window edge; this is a process/render smoke,
not native camera/body-edge or framebuffer parity. Root module hashes are
unchanged. No whole controller is promoted to Verified.

BindToTarget, BindToParent and BindToRoot still require their own native timing,
facing, head/mid/foot offsets, helper retirement and pause comparisons. TargetBind
still needs full Pause/SuperPause/hitpause, foreign-state geometry, multiple
targets/teams and entity-retirement/native lifecycle edges. Registry retirement
currently cancels a stale anchor defensively, without a native retirement trace.
M4ac now integrates the measured HitDef custom-state fields and integer
GetHitVar(isbound) publication. The flag reflects the subject's last update,
not a new/cancelled request or the remaining constraint pointer; see CUSTOMHIT.md.
Complete throws remain unfinished. Run all
native/app/Ebiten/audio clients serially on private :97; do not touch the desktop
or original distribution. Preserve root modules and scope edits to game/mugen/.
