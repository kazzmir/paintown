# Custom state and animation ownership

M4z adds eleven native programs, 200 distinct per-program numeric observations
(216 capture entries, including overlapping pages). It implements the measured
TargetState → ChangeState → SelfState path and
independent ChangeAnim/ChangeAnim2 ownership. Original two-character CNS/AIR/CMD
fixtures execute in the supplied 1.1 beta, then replay through the production
Character scheduler. The reference executable and every authored input are
hash-pinned. This is a bounded ownership checkpoint, not complete throw support.

## State definitions and entry timing

TargetState selects the caller's state definitions for its acquired targets.
The victim immediately reports the new StateNo, Time=0 and previous StateNo.
Entry attributes wait until the victim's positive pass: a destination ctrl=1
is still zero immediately after TargetState and in the victim's -2 state,
then becomes one when the positive state runs. An entry anim uses the victim's
own AIR. A missing own action retains the previous action.
Omitting entry control preserves a previously controlled victim: a separate
fixture records Ctrl=1 immediately before and after the borrowed positive pass.

ChangeState inside a borrowed state continues using that state owner's
definitions. SelfState restores the victim's own definitions, including when
the positive chain continues during the same tick. The executor resolves the
definition and evaluation context again after each transition. State/animation
owners are independent of EntityID, input slot, opponents and acquired targets.

TargetState clamps a negative destination to zero. An undefined positive
destination is accepted by the beta: StateNo changes, Time advances, the old
animation/control remain in the measured case, and there is no positive body.
Go records a contextual warning and follows that measured external-state case.
Ordinary undefined ChangeState/SelfState and other malformed-state fallbacks
are separate, still unmeasured rules; their existing runtime errors remain.

A new normal hit restores the victim's own state definitions. The repeat-hit
fixtures deliberately omit state 5000 from the attacker, so retaining its owner
would fail rather than accidentally selecting identical code. Get-hit metadata,
damage, complete HitDef state parameters and override interactions remain
separate compatibility obligations.

## Negative states and units

While using another player's states, the victim's own -2 definition still runs;
its -3 and -1 definitions do not. The chain fixture resets startup counters and
observes -3/-2/-1 counts 1/2/1 on custom-state entry, then 1/3/1 a tick later.
The attacker does not supply replacement negative definitions. Helper keyctrl
eligibility and all helper/projectile negative scheduling remain open.

The borrowed owner's coordinate width applies to both the borrowed positive
body and the victim's continuing -2 body. A 1280-wide victim at Pos X=280 reports
70 while executing 320-wide states. Const(size.height) still reads the victim's
own constant: its declared 60 converts to 15. Const240p(320) returns 320 in the
borrowed body, then 1280 after SelfState. Scalar variables remain with the
victim and are not rescaled.

PosSet/Add and VelSet/Add convert between script units and victim storage.
The kinematics fixture records local Pos X=70, then 65 after facing-relative
PosAdd(5); local velocity 3 becomes 4.5 after VelAdd(1.5). SelfState exposes the
same physical motion as victim-local velocity 18 and position 242. Physics
continues in victim storage units. Complete geometry boundaries for every other
controller, fractional profiles and redirected Cond involving borrowed players
still need dedicated comparisons.

## Animation ownership

ChangeAnim selects the actor's own AIR; ChangeAnim2 selects the current state
owner's AIR. Both compile value and optional elem expressions. A two-element
borrowed action entered at elem=2 reports AnimElemNo=2, AnimElemTime(2)=0 and
AnimTime=-7, matching the shared AIR clock.

AnimExist queries the active animation source. SelfAnimExist always queries
the actor's own AIR. Borrowing an action makes attacker-only actions visible
to AnimExist while victim-only actions disappear; SelfAnimExist keeps the
opposite result. Ordinary ChangeAnim changes that source back to the actor.

SelfState does not itself replace a continuing borrowed action or its AIR
source. Neither does a normal hit whose destination omits anim. The fixtures
distinguish state restoration from animation replacement explicitly. The
collision element and production player drawing now resolve the active AIR;
sprite lookup/palette stay with the victim. Borrowed AIR offset/box scaling,
full framebuffer filtering, animation transforms and palette/effect ownership
still require render/contact comparisons and are not claimed as native parity.

## Evidence and remaining work

Use reference_probe.py with --p2-fixture NAME for an independently authored
opponent. Its CNS/AIR/optional CMD and extras hashes are pinned separately.
--p2-localcoord changes only the copied opponent definition. Capture never
overwrites independent opponent sources. Existing same-fixture callers retain
their prior behavior. The original distribution is unchanged.

The promoted ownership-* packets use --mugenversion 1.1 --bank fvar
--clipboard-in-cmd --p2-localcoord 1280,960 and seven-second captures. Early
four-second drafts sometimes captured a stale intro clipboard and are diagnostic
only. Stable chain captures and fresh replays must agree before promotion.

Full scoped races and pure engine/evaluator races pass. Fresh entry-phase,
repeat-hit retained-animation, missing-state and elem replays reproduce their
pinned numbers. A real Go game smoke at temp/app-runs/ownership-borrowed-air-go/
draws both actors and the borrowed action through state restoration; this does
not establish pixel/filtering parity. Root module hashes remain unchanged.

Next: TargetBind/BindToTarget/Parent/Root clocks and geometry; HitDef p1stateno,
p2stateno and p2getp1state; complete helper parameters/eligibility, owner deletion,
multi-target/world phases and every remaining controller geometry boundary.
Complete throws, all state-machine behavior and whole controller/trigger rows
remain unverified. See TARGETS.md, ENTITIES.md and COMPATIBILITY.md.
