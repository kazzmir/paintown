# Standalone palette effects — M4cr

The native corpus has53 PalFX programs:48 accepted/576 numeric readings, five
controller load rejections and812 flat RGB observations.213 unique source/probe
hashes pin the originals. Manifests are temp/m4cr-{native,extra,negative}-inputs.
No Go output generated the expected values. PalFX, AllPalFX and BGPalFX are
separate controller obligations; this checkpoint begins with PalFX.

## Native behavior measured so far

Time defaults to0. Zero and values below-1 cancel an existing effect; -1 persists
and positive durations count active owner ticks. Duration1 is visible on its
issuing frame. Sine age begins at0 and stops with a frozen owner. Default add is
0,0,0, mul256,256,256, color256, invert0, sin amplitudes/period0.

All supplied fields evaluate even when time is0. The order is time, color,
add RGB, mul RGB, sin amplitudes/period, invertall. Integers validate at load;
literal floats/unknown triggers reject, runtime floats truncate and missing
redirects become zero. Add/mul require three components. Sinadd accepts three
amplitudes with an optional fourth period; zero/omitted period has no sine term.
Signed multipliers and out-of-range color values are used, despite documented
range restrictions. Negative color extrapolates past gray rather than clamping.

The shader applies color balance (gray weights.3125,.4375,.25), inversion, then
addition/sine and multiplication. Values remain unclamped through source-alpha
blending. With native white247,239,231 and source alpha128/destination0:
add200 produces224,220,216; mul512 produces247,239,231. A palette-byte clamp
before blending would produce the wrong result.

Helpers with ownpal0 evaluate their PalFX expressions but cannot install local
effects. They inherit their parent's effects. Helpers with ownpal1 have separate
palette state and start without an already active parent effect. The original
child controls included a parent clear; separate child-only controls remove
that ordering ambiguity and confirm the ownership rule. All originals remain.

## Implementation / gates

The initial implementation uses typed parameters, palette-owned clocks and
unclamped shared indexed/RGB shader transforms for actors and fight effects.
HitDef palette effects share the drawing path while retaining their distinct
zero/negative-duration installation rules. HUD portraits use mappings without
the actor's temporary effect. New own-palette helpers clear inherited effects.
CPU State.Image also provides a clamped preview, preserving palette indices and
mask alpha. Production draws always use raw mapped images plus the unclamped
shader transform. Normalized float32 arithmetic and nearest-even framebuffer
conversion explain half-value colors which the old integer preview could round
differently. All224 opaque native preview colors and all504 two-sampler GPU
samples pass; the28 alpha readings are checked on GPU where intermediates fit.
Earlier CPU-rounding failures remain in the tool record; native data is unchanged.

Focused53-program Match races pass33.879s. Core Engine/Character/System/Render/
Effect races pass; final CPU preview Palette races pass8.581s. All48 final application RGB crops match exactly:5,216,400 pixels, including812
frozen flat readings. All19 legacy HitDef palette/portrait comparisons pass228 garment color samples
and preserve the neutral portraits. Stock win/rematch/win is visually verified (m4cr-stock-review.png), with82
balanced owned input events and1,978,304 private PCM frames. All213 native input
hashes, final app/regression frozen source hashes, binary/config/PNG/resource/
audio/module/inventory checks pass. Final audit:temp/m4cr-final-evidence-audit.json.
All runtime sessions are awaited; a separate client check is empty. Only source/
text is committed; originals/captures and failed diagnostics remain untouched.
GPU color samples use opaque texture pixels; partial source alpha comes from
Trans. Wider filtered RGBA texture-alpha combinations remain explicitly open.
AllPalFX/BGPalFX and broader palette/contact/afterimage/Explod creation ownership/
pause combinations remain active item3 work. Full renderer/asset/font/platform
conformance remains separately bounded. No overall percentage follows from the
68 typed routes/9 stubs in the reproducible controller inventory.

## Explicit interaction limits

The new native colors establish opaque indexed/RGB inputs plus controller
alpha. Cross-installing full PalFX with HitDef palette effects, helper birth
while a HitDef effect is active, additional pause/foreign-state/Explod ownership
combinations and global composition still require their own native controls.
The retained legacy HitEffect CPU preview is only pinned by the earlier garment
color controls; production HitDef rendering now uses the shared GPU transform.
No renderer-wide RGBA/platform parity is inferred from these controls.
