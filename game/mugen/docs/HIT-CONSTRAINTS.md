# HitDef distance and snap constraints (M4bq)

Target: immutable local M.U.G.E.N 1.1 beta 1. This checkpoint covers mindist,
maxdist and snap: 61 numeric expression keys plus nine literal schemas gives
70/81 typed keys (86.4%). Eleven palette/shake keys remain. This count describes
schemas with bounded native consumers, not whole-engine compatibility.

## Measured schema and order

Bounds accept one or two integer components. Omitted components preserve that
axis. Float literals reject; runtime floats truncate and bottom becomes zero.
Unknown expressions reject before state effects. Omission leaves both axes
unconstrained. Snap accepts an undocumented third integer component: binding
duration. Default/explicit zero queues one displacement, positive durations
bind for that many passes, and -1 holds indefinitely. Third-component floats
and unknown expressions reject, as does a fourth component.

Snap replaces the authored min/max branch. Its X/Y source evaluates twice,
first for minimum and then for maximum; the original bounds' side effects do
not run. X precedes Y. The duration evaluates after p2getp1state and before
p1sprpriority. Bounds evaluate after p1getp2facing and before p2facing. Separate
component, entry and forward/reverse declaration controls establish this order.
A new routing control removes an accidentally duplicated guard.sparkno from
its own input; the original frozen program and its zero observation remain
unchanged, consistent with first-duplicate precedence.

## Runtime consumers

Distances are relative to the contact-time attack axis and heading, converted
once into receiver-local units. Each valid min/max interval clamps its axis;
an inverted interval leaves that axis unchanged. Guards and None contacts do
not apply constraints. Both coordinate-profile directions and reversed facing
are covered. Redirected position queries use caller units; saved FVars retain
the numeric values recorded in the receiver's units.

For duration zero, contact computes a displacement before either actor's CNS.
The receiver observes its old position during entry, and GetHitVar(xoff/yoff)
exposes the pending displacement. The engine adds it after CNS, before movement,
then clears it. A snap from (40,-20) to (30,-10) exposes (-10,+10). If entry CNS
sets (99,-99), the result is (89,-89). A moving attacker proves that the contact
axis is captured before its next movement step, rather than queried later.

Nonzero snap duration uses the binding lifecycle: the receiver observes the
snapped position and isbound=1 during entry. Duration one expires before its
Time=1 pass; duration three still binds then; -1 remains bound at the late
snapshot. Normal target ownership/generation and binding retirement remain
shared with the existing runtime.

## Evidence and limits

39 hitconstraint-* programs pin 588 observations, seven native load rejections
and 156 pre-native CNS/AIR/CMD/stage hashes. Ignored manifests:
m4bq-prepared-inputs.json, m4bq-prepared-extra.json,
m4bq-prepared-routing.json, m4bq-prepared-offset-profile.json and
m4bq-prepared-entry-routing.json. Each new input was frozen before native
execution. Expected values and tolerances were never changed.

Focused races, related binding/facing/down-hit/sweep regressions, full Character,
core/native/app provenance results are recorded in HANDOFF.md. The final scoped
race suite follows M4bt. Native schema/evaluation and actual position/offset/
binding consumers are covered; broad helper/projectile/foreign/override/pause,
multiple-contact, moving timed-bind and retirement combinations remain open.
Existing strict alpha/spark raster failures and full recovery/round/mode/asset
completion gates are unwaived.
