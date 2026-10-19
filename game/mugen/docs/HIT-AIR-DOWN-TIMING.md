# Air, launch and downed hit timing

M4bh compiles `air.hittime` and `down.hittime` and corrects their consumers.
The original receiver substituted 20 for an explicit zero air timer, used the
ground timer for launched/downed hits, and exposed ground slide time in those
reactions. The production Character path now follows the native controls below.

## Measured behavior

Both fields independently default to 20. The down default is measured with an
explicit air timer 17, so it is not an inheritance from air.hittime. Explicit
zero stays 0; negative and bottom become 0. Runtime floats truncate toward zero,
whereas literal floats and multi-value tuples reject at character loading.
Unknown triggers reject before any controller can mutate a state host.

| Contact | Hit timer | GetHitVar(slidetime) |
| --- | --- | --- |
| Ordinary grounded, zero ground Y velocity | ground.hittime | ground.slidetime |
| Grounded, nonzero ground Y velocity | air.hittime | 0 |
| Initially airborne | air.hittime | 0 |
| Lying down, zero down Y velocity | down.hittime | 0 |
| Lying down, nonzero down Y velocity | air.hittime | 0 |

Guard timer selection is unchanged from M4bg. Source integer defaults belong
in compilation/loading; an explicit zero cannot be replaced in the receiver.
Reaction state numbers and the vector-selection posture stay independent of
timer selection. A ground launch can enter 5000 and a down launch 5080 while
using air.hittime. Ground KO controls also select the air timer after KO adjusts
the velocity. This does not establish absolute KO/round clock compatibility.

The bounded fall controls still expose/count down the selected timer when
fall=1; scripted fall/recovery states decide whether HitOver permits recovery.
For an unpaused duration 3, the entry/t1/t3 values are 3/2/0, and HitOver first
appears at local Time 4. With defender shake 2, the values are 3/3/1 and expiry
Time 5. Zero expires at Time 1. Timers stop at -1. These are receiver-local CNS
observations, not wall-clock slowdown measurements.

Full forward/reverse tagged programs place air.hittime after
ground.cornerpush.veloff/before air.velocity, and down.hittime after
air.cornerpush.veloff/before down.velocity, independently of declaration order.
Both sources evaluate once. Shared-host tests confirm no evaluated value or
default leaks into the immutable source map.

## Evidence and verification

Twenty-five original `hittiming-flight-*` text programs pin 336 numeric
observations (overlapping clipboard pages included), three native load
rejections and 100 authored input hashes. The expected values come from the
unchanged 1.1b1 executable. Generated runs, captures, binaries and reference data
stay in ignored temp/. Do not rerun temp/author-m4bh-times.py or derive native
expectations from Go. Source hashes match temp/m4bh-prepared-inputs.json.

The first three downed diagnostic runs used MAF without the required D flag:
life remained 1000 and no contact occurred. They are excluded from acceptance;
their sources/goldens are retained in temp/m4bh-before-down-hitflag/. Corrected
down fixtures have new -contact names and source hashes. The native recorder
also checks life 990 (0 for KO) before accepting a non-order result. The original
reference distribution was never changed.

All 25 new Match replays plus 21 ground/guard, 19 pause and four fatal-guard pause
controls pass under race checks (Match 42.414s). Focused Character reference/
pause/continuation checks pass 1.585s. Engine/evaluator/CNS/combat races pass
1.426s/1.021s/1.014s/1.011s. Three fresh native reproductions (default,
reversed order and float rejection) pass. Three actual apps/four captures pass
source/config/binary/PNG/runtime provenance audit; the stock 7s image was
visually inspected. The full scoped race suite passes: Match 648.043s,
Character 11.516s, System 4.843s, storyboard 1.879s, stock/resources 6.625s.
Logs: temp/m4bh-validation.log and m4bh-full-race.log. Provenance audit:
temp/m4bh-evidence-audit.json.

Example fresh native reproduction using BUILD.md's private environment:

```
python3 game/mugen/tests/reference_probe.py --fixture hittiming-flight-down-clock-contact --name UNIQUE_NAME --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-delay 0 --pages 0 --seconds 6
```

Expression routing is 46/81 documented HitDef keys (56.8%), leaving 35 literal
or absent. This is a schema metric, not engine completion. Full fall recovery,
juggle, projectile/helper/foreign/team/multiple-contact and pause combinations,
absolute KO/round lifecycle, and broader game/asset/render behavior remain open.
The six alpha and two spark raster failures remain unwaived. Stop at M4bh's
validated local checkpoint for the user's testing allocation.
