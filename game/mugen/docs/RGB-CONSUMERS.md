# RGB HUD and fight-effect consumers

M4az follows the actor sampler checkpoint d572399f. Full MUGEN 1.1 beta1
compatibility remains unfinished. The alpha precision gaps below remain open.

## Implementation and measured placement

Fight HUD sprites and fight effects now use the shared clamped linear sampler
when their decoded source is true-color. Indexed consumers retain their existing
path. The source classification precedes palette rendering. Linear.DrawImage
forwards the caller's geometry, tint, Blend and CompositeMode to the triangle
shader; the existing Draw wrapper retains its actor/viewport behavior. Render
errors propagate through the fight/HUD update with action or sprite identifiers.
The shared shader and interpolation arithmetic are unchanged from M4ay.

RGB cue placement truncates signed common-game X/Y before adding stage origin
and camera translation. A .125 or .1875 local offset at scale4 disappears;
.25 and1.75 become1 and7 common units and remain visible. The common960 control
turns1.75 local into5.25 common and selects5. A fractional owner-position control
also discards the fraction, distinguishing the whole cue coordinate from just
its controller offset. Raw fractional placement, nearest-even rounding, local-unit
truncation and rounding after stage-origin translation do not fit these controls.
These controls exercise own SuperPause cues; arbitrary camera/zoom, Explod,
flipped/effect-scale/resource and indexed combinations need additional evidence.

## Original corpus and exact gates

Sixteen original rgbconsumer programs record192 numeric observations and80
CNS/AIR/CMD/stage/SFF hashes with no native load rejection. The generator
consumer_archive() in tests/generate_raster_markers.py creates a32x32 gray RGB24
actor backdrop and an8x4 RGB24 ramp, including gray180/181 variants. The strict
crop620,530..680,565 has2100 original pixels. Only these generated archives and
previous original marker/pattern archives can be recorded by
raster_render_probe.py. No reference artwork or native window captures are
committed. Binary, config, prepared inputs, numeric observations and captured
window hashes are checked before every crop comparison. Go cannot record a
native golden. No per-pixel precision allowance has been added.

The three pre-existing video-fit HUD/world ramp profiles now match both bounds
and RGB core hashes: default, stage0 and window-stretch. Their pre-M4ay and M4ay
nearest-filter baseline failed256 of288 pixels per core, max channel22. That
historical failure remains recorded in m4ay-hud-baseline-checks.json and
SPRITE-SAMPLING.md; the new consumer path closes this specific filtering gap.

Opaque, additive and subtractive original cues match exact crops. The AS192D64
controls at common1280 (gray180/181) and960 retain12 pixels each with a single
channel-level difference. This is a failed strict gate, not accepted parity.
The half and independent-destination controls each fail117 pixels, max1.
Ten of16 whole crops match exactly; all six alpha crops remain failed gates.
The final measurements are temp/m4az-final-consumers.json.

## Rejected experiments

Forcing exactly opaque output alpha did not affect the12-pixel failure.
Complementary source-alpha blending increased errors to48..68 pixels. A hardware
UNORM lookup produced the same larger rounding gaps; its first attempt used
incorrect auxiliary-image coordinates and drew black. Preserving unrounded
bilinear byte fractions still differed (57 pixels for alpha960,144 for half).
All of these candidates were removed. The shader remains byte-for-byte M4ay.
The bundled native rgba.frag distinguishes premultiplied/straight paths and
source-alpha multiplication, but its source alone does not identify every
runtime define or blend setting. It is corroboration, not a proven rounding rule.

## Validation

The clean full scoped race check passes in temp/m4az-full-race.log:
Match489.826s, Character10.368s. All16 integrated consumer programs pass their
192 numeric observations and80 authored hashes. Pure races and root module
integrity checks pass. The first full invocation also found diagnostic Go
snapshots directly under temp/; they were renamed to text and its setup failure
is retained in m4az-full-race-snapshot-setup-failed.log. The clean rerun exited0.

All25 app runs/28 captures pass prepared-input/config/binary/capture/runtime
provenance checks in m4az-final-app-audit.json, using one binary:
7c40366876ea6cd4dee8a3b73eccd3581c8626b8fff4a017ef9751740f8f8394.
Five representative actor crops (340,000 original RGB pixels) still match
byte-for-byte. Three fresh native replays reproduce their numeric/crop goldens,
including two alpha controls. Stock throw recovery was inspected at7s: two
fighters, no lingering dust or effect clones. HUD profiles pass again in
m4az-final-hud.log/json. Final consumer captures use go-m4az-final; earlier
unorm/sourcealpha/unrounded/drawaxis runs remain diagnostics.

Do not label alpha or whole-renderer compatibility complete. Full RGBA,
every driver/backend, all HUD animation attributes, background/rotation/zoom/
foreign-resource and whole-frame gates remain open. Combat power expressions,
transfer/defaults and ownership are the next functional audit.
