# HitDef guard distance

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

Literal-float load rejections changed classification, not input bytes or expected
observations. Do not rerun the author scripts or generate native expectations from
Go.

The stock 7s capture was visually inspected.

Routing is now 55/81 (67.9%); 26 keys remain in the authorized allocation,
including nine literal schemas. This is not overall engine compatibility.
Broader command/input/entity/foreign/pause combinations and other controller
variants remain separate gates. Existing alpha/spark raster differences are
unwaived. No new binaries or reference assets are committed.
