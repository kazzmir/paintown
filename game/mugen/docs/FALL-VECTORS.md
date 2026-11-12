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

Three fresh native reproductions cover the 1280-wide X-only default, reversed
field order and tuple rejection.

Three real apps/five captures cover explicit zero, recovered stock sweep and the
ordinary mixed-profile fight. The stock 7s image was visually inspected; these are
smoke checks, not complete raster parity. No native expectations or tolerances
changed.

Both authorized checkpoints are complete; pause for user testing.

Twenty-eight keys remain literal or absent. This metric does not measure
whole-engine completion.

## Remaining limits

These controls cover ordinary own-state roots, standard/mixed coordinate
profiles and bounded scripted velocity consumers. They do not close full
recovery-command routing, down.bounce, liedown/mashing clocks, HitFallSet reset
variants, juggle/chains or broad foreign/helper/projectile/team/multiple-contact/
Pause/SuperPause combinations. Absolute KO/round timing and wider assets/modes
remain open. Six alpha and two spark raster differences remain unwaived.
Stop after M4bk's validated local checkpoint for user testing.
