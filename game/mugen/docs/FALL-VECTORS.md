# Fall velocity and hit gravity expressions

M4bk compiles fall.xvelocity, fall.yvelocity and yaccel. Their previous literal
parsing discarded expression results and side effects. The existing Character
GetHitVar/HitFallVel/VelAdd path now receives evaluated float32 parameters.

## Measured behavior

Omitted fall X preserves the receiver's current X velocity. Explicit zero,
including bottom converted to zero, sets it to zero instead. The compiler
keeps the X key absent when omitted, preserving the existing sentinel and
FallXUnchanged behavior. Fall Y defaults to -4.5 and yaccel to 0.35 at width
320; default values scale with the executing coordinate width. At widths
640/1280, the Y defaults are -9/-18 and gravity is 0.7/1.4. Defaults are
evaluated separately for each host, without changing the shared source map.

Explicit values are already in the source's units and do not receive default
scaling. Contact converts them into receiver units once. Both mixed directions
and omitted mixed defaults match native. Floating literals and expressions are
accepted; negative gravity is retained, bottom becomes zero, and scalar tuples
reject. Unknown triggers reject before earlier state effects. The inherited
fall flag/source behavior from M4bi is unchanged.

HitFallVel consumes both evaluated components, preserves X on omission, and
does not change the measured X result when facing is reversed. A repeated
HitFallVel resets Y after a scripted gravity addition. Those tests distinguish
stored values from actual consumers. Forward/reverse tagged controls place
fall X/Y after fall.damage and before fall.recover; yaccel runs after
airguard.cornerpush.veloff, last among the 53 currently routed fields.

## Evidence and validation

Twenty original `fallvector-*` text programs pin 276 observations, three native
tuple rejections and 80 pre-native source hashes. Expected values come only
from the unchanged native 1.1 beta 1 executable. All source hashes match
temp/m4bk-prepared-inputs.json; do not rerun temp/author-m4bk-fall-vectors.py.
Core races pass engine/evaluator/CNS/combat 1.442s/1.020s/1.012s/1.012s.
Focused races replay 20 new, 13 trip and 63 earlier velocity controls plus
both actual sweep directions (Match 73.453s); Character recovery-command/
SuperPause checks pass 1.121s. Three fresh native reproductions cover the
1280-wide X-only default, reversed field order and tuple rejection.

Three real apps/five captures cover explicit zero, recovered stock sweep and
the ordinary mixed-profile fight. All source/config/binary/PNG/runtime checks
pass in temp/m4bk-evidence-audit.json. The stock 7s image was visually inspected;
these are smoke checks, not complete raster parity. Shared binary SHA256:
399865d0af49a3421c9e875ab3c3aa8a759399908b75de28b16a0164d60187ec.
The audit verifies both families (33 programs/432 observations/132 input
hashes), the four current Go source hashes and M4bj's five committed source
hashes against 3db8b057. No native expectations or tolerances changed.

The combined full scoped race suite passes in temp/m4bk-full-race.log:
Match 693.752s, Character 11.396s, System 4.735s, storyboard 1.790s,
stock/resources 6.520s; engine/evaluator/CNS/combat
1.453s/1.023s/1.012s/1.011s. This integration boundary covers both M4bj and
M4bk. temp/run-m4bk-validation.py exited 0 (log temp/m4bk-validation.log).
No native/app/test GUI client remains; the root module baseline and original
native executable hash are unchanged. Both authorized checkpoints are complete;
pause for user testing.

Source audit temp/m4bk-hitdef-current-audit.json derives 53/81 routed keys
(65.4%) from the bundled manual and current compiler order. Twenty-eight keys
remain literal or absent. This metric does not measure whole-engine completion.

## Remaining limits

These controls cover ordinary own-state roots, standard/mixed coordinate
profiles and bounded scripted velocity consumers. They do not close full
recovery-command routing, down.bounce, liedown/mashing clocks, HitFallSet reset
variants, juggle/chains or broad foreign/helper/projectile/team/multiple-contact/
Pause/SuperPause combinations. Absolute KO/round timing and wider assets/modes
remain open. Six alpha and two spark raster differences remain unwaived.
Stop after M4bk's validated local checkpoint for user testing.
