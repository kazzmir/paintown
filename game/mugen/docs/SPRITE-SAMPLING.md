# Fractional sprite sampling

M4ay matches the measured actor sampling and viewport interpolation. Reversal
is 931467ac (M4ax). Full 1.1 beta 1 engine,
asset and mode compatibility remains unfinished.

## Measured projection and sampling

Actor axes remain fractional. AIR X offset multiplies Facing; AIR Y offset keeps
its direction. Facing XOR AIR H selects horizontal reversal; AIR V selects
vertical reversal. The established one-common-pixel correction on each effective
flipped axis remains after local/size scaling. The earlier universal-floor
interpretation in SPRITE-ALIGNMENT.md was an overfit to solid integer/half-pixel
RGB markers; the new fractional and patterned controls reject it.

RGB sprites use clamped linear sampling. Indexed sprites use nearest palette
sampling at integer common-game pixel centers, represented by a half-common-pixel
quad translation. Both use unsnapped triangles: Ebiten 2.9.8 rectangle APIs
quantize vertices to 0, 5/16, 11/16 and 1 subpixel phases. Shader interpolation
uses sprite-local custom coordinates; atlas-space fractions lose precision and
caused 662 one-channel errors in the 1.1-scaled RGB helper.

The linear sampler uses eight-bit interpolation weights and byte-rounded
horizontal/vertical interpolation. Floating interpolation left up to two channel
levels of error. Fixed integer arithmetic matches the reference. A ties-to-even
experiment increased errors and was rejected. World/HUD viewport fitting uses
the same sampler. RGB edge sampling clamps to the first/last source texel.

Indexed address selection rounds source coordinates to sixteenths before
indexing. Forward axes select floor; reversed axes select ceil minus one. This
models the measured 1/32-texel selection bias and exact mirrored-boundary rule.
The irregular 400-unit endpoint distinguishes four fractional bits from every
other tested precision (1..12), independently of actor/edge/width rounding.
Neither integer-position nor 1.1-helper controls alone established that rule.
This is an observational model for the tested backend, not proof of the beta's
internal numeric representation or behavior on every graphics driver.

Indexed quads include a transparent one-source-texel guard on every side.
Biased edge samples otherwise disappear at the logical sprite rectangle even
though native draws them. The combined HV400 control exposed one missing edge;
a 32x32 control rejected literal power-of-two-only padding, and an opposite-edge
control establishes guard coverage on the other side. Shader reads outside the
logical image remain transparent and cannot read neighboring atlas images.

Embedded native shader strings independently identify the indexed path as a 2D
index texture followed by a 1D palette lookup, separate from true-color drawing.
The executable also contains sprite-offset calibration labels. These strings
corroborate separate paths, but do not establish the address-bias arithmetic.
The executable and reference distribution are unmodified.

## Corpus and provenance

There are 37 original raster state programs: 444 numeric observations, 180
CNS/AIR/CMD/stage/bank hashes and no final load rejections. Thirty-two original
raster crops cover solid markers, per-texel masked patterns, RGB24, RLE8, LZ5,
positive/negative/half-pixel positions, offsets, flips, 1.1/.5 helper scale,
common game width 640, character width 400, combined HV and power-of-two edges.
Five real-art numeric programs compare equivalent stationary/moving positions.
Their artwork is never stored as a committed golden.

The original 47x106 banks have axis (18,105); the power-of-two bank is32x32 with
axis (16,32). tests/generate_raster_markers.py deterministically writes RGB24,
RLE8 or literal-only LZ5 from authored patterns, including masked fine pixels.
The first LZ5 author used256 palette colors and the native process exited during
rendering; the corrected32-color bank loads and renders. That failed temp run
is diagnostic only and is not a final expectation or a general palette rule.

The original corner-push failures reproduced at equivalent stationary PosSet
positions, including the irregular endpoint. Altering physics was unnecessary.
All five original strict white masks and the additionally measured irregular400
mask now match native exactly in the go-point4 captures. Tests for backing edge
coverage were added afterward; the final pipeline rechecks every advertised gate.

The permanent raster_render_probe.py accepts only byte-identical banks generated
by the original marker/pattern authors. The crop at400,450..800,620 contains only
authored pixels and the black lower stage. It pins actual prepared CNS/CMD/AIR/
bank/stage, native executable, render config, app binary and capture hashes.
Go output cannot record native goldens. Every crop byte must match; there is no
precision allowance. Native video setup clears FirstRun and changes LF to CRLF
on shutdown. The verifier checks the original config hash by reversing only
those exact rewrites and separately pins the actual post-run hash.

No stock sprite art, original executable, native framebuffer crops, disposable
copies, binaries or caches are committed. The compressed crops contain only
original marker/pattern/black pixels. Root Go module metadata remains unchanged.

## Validation in progress

The frozen source passes all 32 original crops (2,176,000 RGB pixels) and all 28
unchanged flipmark crops byte-for-byte. No precision allowance was used. All six
strict clothing masks match, including irregular400. Six fresh native replays
pass, including five strict original crops. Stock throw recovery shows two
fighters without lingering dust/effect clones.

The67 app runs produced70 captures from one identical binary:
b0f0dd90433fcc6958594341d9dd37e886f8b394f80e5b579eb5d1d8d0f33d7b.
All binary/config/capture hashes and runtime/resource checks pass in
m4ay-final-app-audit.json; detailed phase logs use m4ay-all-raster-final,
m4ay-legacy-final, m4ay-corner-final-checks and m4ay-final-replays.

Full scoped races pass in m4ay-full-race.log: Match 461.719 seconds and
Character 10.938 seconds. All 37 integrated numeric programs pass. Root module
checks pass, and no code/data outside game/mugen/ changed.

The additional default HUD/world ramp check FAILS its color hashes while all
rectangles match. Both288-pixel cores differ at256 pixels, maximum channel
error22. The pre-M4ay binary and current binary produce the identical core SHA
3b6d1fda45f45a849ae8a8e0e26db45d0b985d174f23082d1c04f590bde7cf6a,
versus native 332a367881b47bfe3c93e477b7557aaf57d19530b1b50f31acc87f04cd4262c8.
See m4ay-hud-baseline-checks.json. Those HUD/effect files were unchanged by M4ay:
first-stage RGB images still use nearest filtering there, while native ramps
are linear. This is an established pre-existing consumer gap, not a passed
check. The probe stopped at default; stage0/stretch are prepared but unrun.

Record this bounded actor checkpoint locally, then immediately implement and
verify the RGB HUD/effect consumers as M4az. Do not relax the ramp hashes or
claim all assets/consumers complete. Power probes remain deferred until that
render gap is settled. There is no user review/permission dependency.

The earlier48 original crops passed with the corrected RGB sampler; later fine
controls exposed indexed rules absent from those coarse scenes. Prototype names
include raw, ceil, filter, triangles, quantized, candidate, precision, centers,
integer-lerp, even-lerp, local-uv, reverse-index, point4, coverage, padded and
guard. They retain failed hypotheses and cannot overwrite native expectations.
The final captures end go-m4ay-final. Earlier go-final/go-verified captures are
intermediate indexed evidence and must not be counted as final validation.
Pure evaluator/CNS/engine/SFF/palette/coordinate races pass m4ay-pure-race.log.

Full RGBA/alpha, other drivers/renderers, every scale/zoom/rotation/interpolation/
foreign-resource combination, background rendering, all effects and whole-frame
equivalence remain open. Continue the RGB consumer follow-up, then the prepared combat power probes;
temp/author-hit-power.py has not been run.
