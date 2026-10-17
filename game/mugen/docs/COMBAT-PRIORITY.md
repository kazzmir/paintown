# HitDef priority, counters and interrupted targets

M4ar uses the supplied beta executable and original `hitclash-*` CNS/AIR/CMD/
stage programs. The local `docs/sctrls.html` priority table and `trigger.html`
HitCount/UniqHitCount entries are the starting specification; observations pin
defaults, parser boundaries, tick behavior and target ownership separately.
Reference binaries and stock art remain under ignored `temp/`.

## Measured rules

Reciprocal eligible contacts compare integer priority before class. Higher
priority wins even when its class is Miss or Dodge. Equal Hit/Hit attacks trade;
Hit/Miss admits only Hit. Any equal Dodge pair and Miss/Miss admit neither hit.
Class does not suppress a one-sided attack against an idle victim. A rejected
no-hit tie leaves both definitions armed; leaving attack MoveType later permits
the outstanding attack to contact. The current resolution uses reciprocal
contact eligibility; distinct box/protection combinations remain an open gate.

Priority defaults to 4, Hit. A number alone keeps Hit. Raw class prefixes accept
H/M/D, including Dodging; quoted names, unknown names and a trailing empty class
reject. Negative priorities become zero; explicit zero remains zero. Eight
beats seven and nine beats eight, so the documented 1–7 range is not an upper
runtime clamp. Computed floats truncate; literal floats reject. Bottom becomes
zero without disabling the HitDef. An enemy redirect in the numeric component
retains its own comma before the final class separator.

An unguarded contact credits HitCount once per attacker/contact frame and
UniqHitCount once per victim, each weighted by numhits (default one). Two helpers
hit together therefore credit 1/2; staggered root/helper contacts credit 2/2.
Guard credits neither. Explicit zero and negative numhits are retained; -1
credits -1. Literal float numhits rejects, computed float truncates, bottom
credits zero. Damage and target acquisition still occur when numhits is zero.
Both counters reset with the state's hitcountpersist rule. These are separate
from the HUD combo counter and HitAdd.

Taking an unguarded hit clears the receiver's own target list, including when
HitOverride redirects to an idle state and suppresses damage. Guarding preserves
the list. Manually entering a hit state or using StateTypeSet MoveType H also
preserves it. Releasing an interrupted anchor's valid bound H victim selects the
victim's own external state 5050; ordinary unbound targets keep their state.
The release shares the measured retirement entry timing. Explicit TargetDrop
and TargetBind cancellation retain their separate existing behavior. Contact
processing order explains the asymmetric surviving target list after a trade;
changing H state alone must not clear every target.

Root receivers take contact before helper receivers, independently of helper
team. Equal root/root trades leave the second root's target; either root/helper
trade leaves the root's newly acquired helper target. The reverse-team control
distinguishes receiver class from player order. A hitonce attack that loses or
ties its root clash can still hit the opposing helper; after consumption the
remaining root may make its formerly tied attack on a later tick. A winning
root contact consumes hitonce before a helper is selected.

## Implementation

`engine/hitdef.go` compiles priority and numhits expressions alongside existing
typed HitDef fields. The final enum separator is parsed without discarding
redirect commas. `character/clash.go` filters reciprocal contacts before hitonce
selection and guard consumption. The production tick credits counters before
get-hit state entry, so normal entry/reset behavior remains observable.
Selected contacts retain root receivers before helpers for contact mutation.
`prepareContact` interrupts the receiver's targets only for unguarded contacts;
`releaseHitBinding` is shared with helper retirement. No alternate combat engine
is introduced.

## Validation

The final corpus contains 63 programs, 996 numeric observations, five native
load rejections and 252 authored input hashes. The first 52 programs pass race
comparison in `temp/m4ar-first-clashes.log`; parser/CNS/evaluator races pass in
`temp/m4ar-first-pure-race.log`. The expanded pre-order comparison has three
failing programs with target-list differences; no priority winner, damage or
counter mismatch. `temp/m4ar-expanded-before-order.log` preserves that evidence.
All 63 programs pass race comparison after receiver ordering in
`temp/m4ar-ordered-clashes.log`. The full scoped race suite passes in
`temp/m4ar-full-race.log` (Match 231.848 seconds). Six fresh native replays pass:
redirect, signed numhits, bound interruption, reverse helper trade, hitonce after
a lost root clash and literal-float rejection.

Sixteen real 1280x720 captures from one identical Go binary pass binary/config/
prepared-source/PNG/dimension/nonblack/runtime/resource checks. Stock throw
recovery displays two fighters without persistent dust clones. Higher-priority,
no-hit and helper-trade white clothing masks match exactly (11,181/11,181/19,515
pixels). Bound release has 12,501 native versus 12,528 Go white pixels and 6,099
mask differences, with a different foreground facing in the overlap. This is
recorded in `temp/m4ar-app-checks.json`; it does not invalidate the separate
state/target observations. HitDef p1sprpriority/p2sprpriority are unimplemented
and are the next marker-based investigation. Do not claim full sprite placement
parity for that scene or guess an offset from the clothing mask.
An initial helper diagnostic was mistakenly put in root-only state -3; its
all-zero helper page is diagnostic only and was replaced by positive-state
publication. Root results independently distinguish priority outcomes.

| Group | Programs | Numbers | Rejections |
| --- | ---: | ---: | ---: |
| Initial class/priority/one-sided/release controls | 25 | 276 | 2 |
| Additional numeric/enum/redirect boundaries | 12 | 120 | 2 |
| Hit/guard/manual-state target interruption | 4 | 96 | 0 |
| Simultaneous/staggered/guard/weighted counters | 4 | 96 | 0 |
| Signed/typed numhits, override and bound interruption | 7 | 144 | 1 |
| Helper/reverse-team and hitonce clashes | 9 | 216 | 0 |
| State-entry counter persistence | 2 | 48 | 0 |

Replay a program with a fresh run name using `tests/reference_probe.py`,
`--fixture hitclash-NAME --fixture-stage --bank fvar --mugenversion 1.1
--clipboard-in-cmd --pages ... --seconds 7`. Use each JSON's capture page list;
the rejected programs use page 0. The helper/reverse-team programs require both
pages 0 and 12. Normal helper snapshots are published from their positive
states; root-only negative-state snapshots cannot substitute for that evidence.
`TestNativeHitClashes` replays all committed observations through the production
match loop and checks all four authored source hashes per program.

Full priority/protection/custom-state/projectile/pause/team-mode combinations,
all remaining HitDef schema fields, combo HUD behavior and complete engine/
asset/mode compatibility remain open. A passing name/parser inventory alone
does not satisfy those gates.
