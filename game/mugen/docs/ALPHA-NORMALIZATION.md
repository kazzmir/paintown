# Native alpha precision — M4co

This checkpoint closes the two M4ck indexed alpha counterexamples and all six
M4az RGB alpha crop failures. It does not establish complete renderer or RGBA
compatibility. Native goldens, input files, images and comparison tolerances are
unchanged.

## Two causes and their consumers

First, native byte colors reproduce a float32 reciprocal product. A texture
unit's normalized read or a later division can fall below that product at a
half-integer color boundary. The indexed shader now recovers the byte before
multiplying by1/255; the RGB sampler uses the same product after its existing
byte-weighted interpolation. Geometry/interpolation weights are unchanged.

Second, complementary and independent source/destination weights use different
blend paths. One-minus-source-alpha and an independent destination factor can
quantize differently at a half weight. Complementary weights retain the former
path. Independent weights use the direct destination factor, with transparent
texel discard rather than multiplication by texture alpha. ApplyAlpha and both
ordinary/projected samplers carry this distinction into production effects.
The supplied native indexed.frag/rgba.frag corroborate the transparent-texel
rule, while the frozen grid distinguishes the actual blend behavior.

A source-only half-alpha trial passed before the full grid exposed this second
cause. That incomplete candidate and the failed GPU log are preserved. The
final fix passes the complete corpus without adding a per-pixel allowance.

## Immutable native color regression

indexed-alpha-colors.json contains270 RGB readings derived only from three
frozen M4ck native grids: ten palette colors, nine weights and three grid forms
(source varying/destination0, source varying/destination128, source128/
destination varying). It pins15 original CNS/AIR/CMD/stage/JSON hashes and the
three native PNG hashes. No Go output was used to generate these values.

The GPU test exercises the actual indexed and RGB samplers plus the shared
production blend selector:540 RGB samples /1,620 component comparisons, all
exact. It runs the public Ebitengine loop for real GPU readback. Render therefore
now has display-dependent tests instead of being a compiled-only package.
A graphical display remains required, including for the full race suite.

## Actual application verification

The final frozen source builds26 application windows:

- Three indexed grids reproduce all270 RGB readings exactly.
- The two previous single-effect failures match their8,379-pixel native masks
  exactly: source-only white124,120,116 and half/half white251,120,243.
- All16 RGB consumer crops match byte-for-byte, including all six previously
  failed alpha crops. Their original2100-pixel crops and hashes are unchanged.
- Five representative actor sampling crops remain byte-for-byte exact.

GPU races pass1.809s; System/Character/Background/Effect races pass
9.239s/10.991s/1.034s/1.010s. App hashes, selected definitions, prepared resources,
configurations, binaries, PNGs and all frozen inputs verify. Final stock win/rematch/win is visually verified, with82 balanced owned input
events and1,987,918 private PCM frames. All frozen source/module/app/inventory
guards pass; sessions were awaited and a separate client check is empty. Final
audit is temp/m4co-final-evidence-audit.json; HANDOFF.md records the checkpoint.
Artifacts begin temp/m4co4; source-only and RGB-half trials use m4co1/m4co2.

Rotation masks from M4ck are still unwaived. All drivers/backends, partial source
alpha, color-effect composition, whole-frame filtering and the wider asset/
controller matrix remain separate gates. Alpha-channel framebuffer equality is
not inferred from RGB captures. The Trans controller and other item3 consumers
still require their own schemas/clocks/native controls. Latest full scoped
integration remains M4cl; focused GPU/application checks cover this change.
