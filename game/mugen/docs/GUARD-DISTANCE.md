# HitDef guard distance

M4bn closes the bounded `guard.dist` schema and range consumer against the
supplied 1.1 beta executable. Fifteen original text programs pin 204 numeric
observations, four load rejections and 60 source hashes.

The field is an int32 expression. Floating literals and multi-component tuples
fail loading; runtime floats truncate toward zero, bottom becomes zero and
unknown triggers fail compilation with source/state/controller/field context.
Expressions run after guard.ctrltime and before guard.pausetime, independently
of declaration order. Omission does not evaluate or inject a default expression.

Omitted and negative results preserve the actor's current guard range. Explicit
zero disables it. State entry restores the character's configured attack range.
The native 40/41 boundary is strict: an opponent exactly 40 units away is outside
range 40. An opponent on the same axis is eligible for a positive range; one
behind the attacker's facing is not. Vertical separation does not change the
query. Mixed coordinates convert the distance once through the existing
GetInGuardDist consumer. These tests keep attack boxes absent to prevent contact
from changing the observing state.

The first draft accidentally allowed contact and is archived, unpromoted, in
ignored temp/m4bn-draft-fixtures. Corrected guardrange-* inputs were frozen in
temp/m4bn-final-prepared-inputs.json before native captures. Literal-float load
rejections changed classification, not input bytes or expected observations.
Do not rerun the author scripts or generate native expectations from Go.

TestNativeGuardDistancePrograms replays the exact fixture count and hashes.
Focused Match races pass 41.153s (15 new plus 43 recent fall/bounce controls);
full Character races pass 12.156s, including prior guard-range/AttackDist cases.
Core engine/evaluator/CNS/combat races pass 1.503s/1.022s/1.013s/1.013s. Two
fresh native reproductions and two apps/three captures pass provenance in
temp/m4bn-evidence-audit.json. Four authored Go hashes are frozen in
temp/m4bn-production-inputs.json. The stock 7s capture was visually inspected.
Binary SHA256: 70191f00bcd89cadb8a963794fb03e24a2c2f9889892983fe242fb57d2d56e5c.
Full scoped races follow the planned enum/combat integration boundary.

Routing is now 55/81 (67.9%); 26 keys remain in the authorized allocation,
including nine literal schemas. This is not overall engine compatibility.
Broader command/input/entity/foreign/pause combinations and other controller
variants remain separate gates. Existing alpha/spark raster differences are
unwaived. No new binaries or reference assets are committed.
