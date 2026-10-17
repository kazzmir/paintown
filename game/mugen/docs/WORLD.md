# Root update phases

M4aa corrects a shared-phase assumption while preparing TargetBind. Three
independent native two-character programs pin 48 numeric observations. They
distinguish direct opponent position/time visibility, attacker ordering and
both sides of simultaneous damage. This is root-player evidence; complete
helper/projectile/neutral/team scheduling remains open.

## Measured root sequence

Previous-tick HitDefs still produce contacts before root CNS passes. Contact
metadata and get-hit state selection are visible to negative and positive CNS
before the affected player's pending life reduction. Timed protection and
attacker-first order retain their existing measured behavior.

After ordering roots, each root completes its negative/positive CNS passes,
pending damage, physics and clocks before the next root runs CNS. A root that
starts at X=-70 and sets Vel X=2 is visible to the second root at X=-68 and
Time=1 during that second root's Time=0 body. The old Go all-CNS-then-physics
sequence returned X=-70 and Time=0, and failed the new native replay.

Attacker priority applies to the entire root update. In the second-root-attacker
fixture, the first root's Time=1 CNS sees the second root at Time=2. Equal
priorities keep root order. This extends the M4k CNS ordering evidence; it does
not establish every simultaneous priority/reversal/team rule.

Pending damage is applied after the affected root's CNS, rather than after all
roots' CNS. A simultaneous 25-damage trade records:

| Observation | Own life | Opponent life |
| --- | ---: | ---: |
| First root's get-hit Time=0 CNS | 1000 | 1000 |
| Second root's get-hit Time=0 CNS | 1000 | 975 |
| Each root's get-hit Time=1 CNS | 975 | 975 |

The old single-sided M4g packets could not distinguish these arrangements. They
still pass, but their interpretation as one shared post-CNS damage phase is
superseded. M4y pending-damage negative-state observations remain correct for
the affected actor's own CNS.

## Shared late body push

Body push remains after all root CNS/movement. Moving push into each root's
finish exposed a later root to a displacement before its own CNS and broke the
existing native push/guard-cross/viewport packets. Separating late push restores
all those observations, including near-axis, fractional and mixed-width cases.
The existing one-on-one geometry is retained; full multi-entity/dynamic Width
and bound/paused/helper push rules still need comparison.

KO sound suppression is collected after all root updates so a later root's
global nokosnd assertion still participates. Complete KO/camera/effect/layer
ordering is not established by these numeric probes. Existing sound, pause,
SuperPause, effects, target, ownership and viewport regression packets pass.

## Validation and next work

The production Character scheduler replays all three hash-pinned packets through
the shared native-world fixture helper. Full scoped races pass in
temp/m4aa-push-draft-race.log. The earlier per-root-push draft is a failed
diagnostic, not a passing checkpoint. The first root-physics draft also read the
opponent's intro Time=130; the promoted program samples after both enter their
authored states, isolating root phases from unfinished intro/match timing.

Fresh replays of all three promoted packets match. The real mixed KFM/KFM720
fight runs in a 1280x720 owned window through seven-second captures at
temp/app-runs/root-phases-stock-go/. Root module checksums remain unchanged;
no original reference assets or desktop settings were changed. The window
inspection does not establish full fight or framebuffer parity.

TargetBind's first native trace queues a constraint rather than moving its
target immediately. By the target's CNS, the anchor has already completed
movement and its bound target has the anchor's world velocity. Persistent
duration, facing, release, binding kinds, mixed units and interactions remain
work in progress; no binding controller is declared implemented by M4aa.

Continue binding and HitDef custom-state parameters, then helper keyctrl/
negative eligibility, helper creation/deletion/global phases and projectiles.
Nested legacy helper Update still owns an incomplete timeline/phase adapter.
Full state-machine/engine compatibility remains unfinished.

M4ab now implements measured TargetBind and adds hurtbox gating to this late
body-push phase. Independent controls show H and I both push when both actors
have Clsn2; absent Clsn2 excludes push. CNS widths provide horizontal extents,
and bound actors can still be displaced by shared late push. See BINDING.md;
full vertical/frame/helper/width/pause eligibility remains open.
