# Global palette effects — M4cs

Follow-up M4ct closes all four original Explod overlap failures with exact
actor/stage comparisons and unchanged goldens; see [EFFECT-ORDERING.md](EFFECT-ORDERING.md).
The M4cs results below remain the historical checkpoint record.

AllPalFX and BGPalFX now have typed schemas, global clocks and production
consumers. This is a bounded controller checkpoint under the continuing item3
allocation. The palette comparisons pass; four owned-Explod overlap crops remain
explicit draw-order counterexamples for the next dependent checkpoint.

## Schemas, ownership and clocks

Both controllers use the measured PalFX schema:time defaults0, color256,
add0,0,0, mul256,256,256, invert0 and sine amplitudes/period0. Positive time counts
global frames; -1 persists; zero and values below-1 cancel. Time1 is visible on
its issuing frame. Unlike actor-local PalFX, these clocks continue through CNS
Pause. Continuous-issue controls establish sine age0 deterministically.

All supplied arguments evaluate even at time0, in time/color/add/mul/sin/invert
order. Tagged controls give123456. Integer literals reject floats; runtime
integer conversions, missing redirects, signed multipliers and out-of-range
color behavior share the typed PalFX compiler. All/BG effects have independent
state:both controller orders agree, and cancelling one preserves the other.
Helpers can issue either global effect regardless of palette sharing.

Eight early sine captures retain their original numeric evidence and images,
but are marked dynamic render diagnostics. A world Pause does not freeze global
sine age, so their wall-clock snapshots cannot establish fixed-age colors. Six
separate continuous-issue controls resolve timing without altering those inputs.

## Measured drawing behavior

The first pass combines palette parameters:add sums, mul/color products and
invert xor. Fighters/projectiles/Explods combine their local transform with
AllPalFX. Stage elements combine BGPalFX with AllPalFX. HUD sprites, portraits
and font glyphs receive AllPalFX. A second AllPalFX pass occurs at viewport
fitting after scene blending and framebuffer clamping.

This is an output-equivalent model inferred from the native controls. It is not
a claim to have recovered every internal native implementation detail. Complex
local/global and All/BG controls discriminate it from sequential color matrices.
For native white247,239,231, AllPalFX invert plus source alpha128 produces
251,247,243, rather than the result of simply applying one sprite transform.

BGPalFX changes stage pixels once. The measured fighter, projectile, Explod,
HUD bar and portrait colors remain unchanged. The beta manual's broader lifebar
wording is therefore not implemented by assumption. Global source consumers
also route foreground stage elements; the wider layer/format matrix stays open.

Debug-background filling uses a separate first byte transform:bounded
truncated desaturation, inversion, saturating add, then multiplication. Negative
color is ignored by this first desaturation step, while ordinary sprite shaders
use it. The final AllPalFX pass still applies. This byte behavior must not clamp
ordinary overbright sprite colors before alpha blending.

The stage fit now uses nearest filtering to preserve native indexed plateaus.
The initial unchanged-code baseline matched every actor/HUD/clear/outer point
but differed at all12 stage samples by1–4 levels because of linear smoothing.
The sampling correction matches the unchanged full native stage region.

## Numerical precision and preserved diagnostics

The first large matrix found gray117->Go116 and133->Go132 in pixels absent from
the small flat-color sample set. The first gray app had1,851 actor-crop and27
stage-crop differences. Explicitly spelling the native shader's weighted sums
removed only two of six reproduced GPU failures.

A native-only supplement collects all consistent flat source/gray pairs in fixed
native regions:98 one-pass and134 two-pass cases. Eight arithmetic models showed
that byte luminance fixes complete gray. Applying it to partial color balance
then exposed108 stage pixels in a combined-color app, despite exact actor crop
and all37 sample points.

The second supplement covers all98 flat stage source colors across33 profiles,
3,234 cases. Five full arithmetic models distinguish the paths:normalized gray
fails6 comparisons; byte-luma fails4 photo comparisons; byte-mix fails6; byte-all
fails154. Only complete gray from byte luminance with normalized interpolation
for partial balance passes all gray/local/global/photo controls. The production
shader uses that measured distinction without epsilon biases or special color
constants. Every failed log, original input, capture and native golden remains
unchanged. Private GPU experiments stay below temp/_tmp.

## Evidence and current verification

83 native programs pin79 accepted snapshots/948 numeric values/four load
rejections and333 unique original/probe hashes.71 static programs pin2,795 RGB
samples. Separate native-only coordinate and crop manifests preserve all original
JSON observations. The flat calibration contains14 actor,12 stage,9 available
HUD, one clear and one outer point. An initial arbitrary12-HUD-color assertion
failed before writing files; using the actual native flat-color count corrected
that setup without consulting Go output.

Core races and83-program Match races pass56.064s. Final Render/Palette/Font/
Background/Stage/System races pass, with Render27.848s. The render corpus checks
10,352 native GPU samples through both samplers:540 alpha,504 local PalFX,
2,376 global/clear,464 gray and6,468 photo comparisons. These are scoped sample
counts, not an overall engine percentage.

The current71-app matrix has2,795 exact sample colors and71 exact full stage
crops (8,207,600 pixels).67 actor crops are exact (7,281,225 pixels). Four
owned-Explod overlap crops each differ126 pixels. At every differing pixel,
native retains the original root hand while Go covers it with the default
Explod; garment masks differ36 pixels. Both palette ownership values and both
global scopes reproduce it. All sampled object colors are correct. These four
crops are UNWAIVED; the public probe and complete strict matrix exit1.

All10 regression apps pass (5 local PalFX,3 HitDef colors/neutral portraits,
2 indexed/RGB raster), and stock win/rematch/win is visually verified with82
balanced owned input events/1,941,379 private PCM frames. Final full races pass
3,857 results across25 tested+11 compiled-only packages, no failures/individual
skips; Match1606.364s, summed1691.569s. All14,164 frozen inputs,333 native hashes
and final resource/config/binary/PNG/audio/module/inventory guards pass. Final
audit:temp/m4cs-final-evidence-audit.json. It explicitly records overall
application RGB conformance FALSE for the4 ordering crops. All runtime sessions
are awaited and separate clients empty; only Markdown changes after final runs. No full
application RGB, font, framebuffer-alpha or platform parity is claimed.

## Reproduce and continue

Focused Go checks use the normal scoped launcher with a working display:

```bash
game/mugen/mugen.sh --check -p 1 -run 'TestNative(GlobalPaletteFX|GlobalPaletteColors|GlobalPhotoColors|GrayOperationPrecision)'
```

Read-only comparison of existing private native/application captures:

```bash
python3 game/mugen/tests/global_palette_render_probe.py \
  --fixture globalfx-all-combined --go-run m4cs4-globalfx-all-combined \
  --name my-global-palette-check
```

The probe cannot record expectations. It verifies prepared assets, native/source/
config/binary/capture hashes, actor/stage RGB crops and all color points.
Manifests are temp/m4cs-{native,extra,boundary,effects}-inputs/metadata.json;
final applications use m4cs4/m4cs5. Failed m4cs1/2/3 experiments remain history.
Byte-identical private Go executables are hard-linked with their paths/hashes
preserved to reduce storage; native files, captures and goldens are untouched.

Next close effect draw ordering before EnvColor layering. Explod palette creation/
remapping, random/angle/shadow/capacity, contact/helper-birth/pause combinations,
afterimages and other item3 consumers remain allocated. Wider renderer/font/
asset/platform conformance belongs to the separately unallocated item4.
