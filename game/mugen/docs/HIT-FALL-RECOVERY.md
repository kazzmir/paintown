# Fall flags and recovery eligibility

M4cb supersedes the historical global recovery-command adapter below: actual
routes belong to authored CNS, and eligible retained fall data can still answer
CanRecover after leaving H. Read [LIEDOWN-RECOVERY.md](LIEDOWN-RECOVERY.md) for
new native controls, liedown/input semantics and remaining interactions.

M4bi compiles `fall`, `air.fall`, `fall.recover` and `fall.recovertime`, exposes
the two missing integer GetHitVar selectors, and binds CanRecover to the
receiver's fall clock. Previously, the flags were literal strings and
CanRecover returned fall.recover without considering the delay. The existing
recovery-command adapter now uses the same eligibility gate as CNS expressions.

## Native behavior

| Field | Omitted | Conversion |
| --- | --- | --- |
| fall | 0 | Integer zero is false; any nonzero integer is true. |
| air.fall | Re-executes the fall source | Same boolean rule. |
| fall.recover | 1 | Same boolean rule. |
| fall.recovertime | 4 | Nonnegative integers retained; negative results use 4. |

Runtime floats truncate toward zero before use. Thus 3.9 gives true/3, while
-0.9 gives false/0. Bottom gives zero. Literal floats reject for all four
fields, and a multi-value recovery-time tuple rejects. Unknown triggers reject
before earlier state controllers can mutate the host. Shared compiled states
retain their original sources, including inherited sources, across hosts.

With Var(10)=7, `fall=(Var(10):=Var(10)+1)` executes twice when air.fall is
omitted, leaving Var(10)=9. Explicit source-default controls separately pin
fall.recover and fall.recovertime side effects. Forward/reverse tagged controls
place fall after forcenofall, then fall.damage, fall.recover and
fall.recovertime; air.fall executes after air.hittime and before air.velocity.
All 46 previously routed fields retain their relative order.

An ordinary grounded contact falls when fall is nonzero. An airborne contact
falls when either fall or air.fall is nonzero: explicit air.fall=0 does not
clear fall=1. The air-only and air-only-ground controls distinguish the two
postures. Existing fall retention, forcenofall and lethal normalization remain
separate contact rules, covered by their earlier bounded controls.

GetHitVar(fall.time) starts at 0 on contact and advances each get-hit fall tick,
independently of State Time. Changing from the reaction state to another H
state at Time 2 preserves the clock: that state's local times 1/3/6 expose
fall times 3/5/8. GetHitVar(fall.recovertime) exposes the configured duration.
CanRecover is true when recovery is enabled and the fall clock has reached
that duration. Zero permits recovery on entry; disabled recovery never permits
it in the measured window. A scripted ChangeState gated by CanRecover reaches
state 730 after four ticks in the default-delay control.

The manual says the recovery delay excludes shaking time. The native controls
instead show fall.time and CanRecover advancing during defender shake: with
pausetime=0,8 and delay 3, CanRecover becomes true after three ticks while
GetHitVar(hitshaketime) is still 5. The stock common1.cns shake states gate
entry into the fall/recovery logic; CanRecover itself does not wait for shake
or HitOver. Implementation follows the measured executable behavior.

## Evidence and validation

Thirty original `hitfall-recovery-*` text programs pin 372 numeric observations
(overlapping order pages included), five native load rejections and 120 authored
input hashes. Expected values are recorded from the immutable 1.1 beta 1
executable, never generated from Go. Sources are frozen against
temp/m4bi-prepared-inputs.json. Do not rerun temp/author-m4bi-fall.py. Native
runs/captures, generated apps and reference assets remain ignored in temp/.

Focused races pass all 30 new programs, 25 air/down, 21 ground/guard and 38
forced-standing programs (Match 71.220s), plus recovery-command, Pause and
SuperPause Character checks (1.386s). Three fresh native reproductions cover
long shake, reversed order and a float rejection. Three actual apps/four
captures pass source/config/binary/PNG/runtime provenance checks; the stock
7s image was visually inspected for intact fighters, stage and HUD. These
application checks establish smoke behavior, not full-frame native parity.
Audit: temp/m4bi-evidence-audit.json. Shared application binary SHA256:
79ea936dec430cfdd4318ca3e7a83543421e56040af6e7e8ac224c7a561bd596.

The first focused run exposed missing typed trigger selectors, an air.fall
order transcription error, and a missing Null controller in a unit fixture.
Those are corrected; the failed log remains temp/m4bi-initial-focused-race.log.
The corrected log is temp/m4bi-focused-race.log. No native expectations or
tolerances changed.

The full scoped serial-package race suite passes in temp/m4bi-full-race.log:
Match 640.740s, Character 11.000s, System 4.652s, storyboard 1.778s and
stock/resources 6.510s. Compiler/evaluator/CNS/combat pass
1.434s/1.020s/1.013s/1.012s. The final pipeline
(temp/run-m4bi-validation.py, log temp/m4bi-validation.log) exited 0. All 12
authored Go source hashes match their acceptance snapshot, and the root module
baseline and original native executable hashes remain unchanged. No test,
native or app client remains on the private display.

Example fresh native reproduction with BUILD.md's private environment:

```
python3 game/mugen/tests/reference_probe.py --fixture hitfall-recovery-shake-inspect --name UNIQUE_NAME --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-delay 0 --pages 0 --seconds 6
```

## Remaining limits

Expression routing is 50/81 documented HitDef keys (61.7%), with 31 still
literal or absent. This is a schema metric, not engine completion. The current
audit is temp/m4bi-hitdef-current-audit.json, freshly derived from the bundled
manual and source order. Its checkpoint/percentage metadata correct the stale
M4be metadata in the prior temp audit; the prior 46-key list was accurate.

This checkpoint establishes ordinary root/own-state contact and a bounded
scripted recovery transition. It does not close full recovery physics, bounce,
HitFallSet reset variants, juggle/chains, KO clocks, or broad projectile/helper/
foreign/team/multiple-contact/Pause/SuperPause combinations. The six alpha and
two spark raster failures remain unwaived. Pause after the validated local
checkpoint for the user's test allocation.

The legacy engine recovery-command transitions still use posture/animation
heuristics. Applying the eligibility gate does not establish native state,
velocity or alive conditions for those transitions. Reviewing that routing
against common1.cns, non-fall GetHitVar(recovertime), and remaining bounce/fall
velocity/gravity fields is the next recovery integration work.
