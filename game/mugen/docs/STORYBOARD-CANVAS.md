# Storyboard canvas and output scaling

M4bf fixes the small top-left KFM intro reported by the user. The loader ignored
`[Info] localcoord`, and playback drew 320x240 coordinates directly into the
1280x720 motif surface. Storyboards now retain their own canvas, defaulting to
320x240, and backgrounds inherit its dimensions for clipping. Invalid dimensions
report the DEF path, section, attribute and line.

The local 1.1b1 manual, docs/storyboard.html's Info reference, specifies this
default and scaling to fit. Six fresh native controls establish two distinct
transforms: fit storyboard into configured GameWidth/GameHeight, then fit or
stretch that game surface into the physical window according to KeepAspect.
The motif's canvas does not determine storyboard coordinates. Clear colors and
fades cover the game surface, including its inner fitted margins; outer window
letterboxing stays black.

For example, default 320x240 into a 1280x720 game fits at 3x with x=160. In a
640x480 window with KeepAspect=1, the game then scales by .5 with y=60. A marker
whose source bounds are (64,28)-(96,60) appears at (176,102)-(224,150), exactly
matching native. KeepAspect=0 yields (176,56)-(224,120) instead.

## Verification

`tests/storyboard_probe.py` generates original RGB markers into ignored temp/.
Six text-only native goldens under tests/testdata/storyboard-*.json pin source,
sprite, native executable, config, motif and capture hashes plus measured marker
bounds/counts and edge samples. No new binary fixtures/assets are tracked.
Cases cover omitted 320x240, explicit 640x480 and 1280x720, widescreen/4:3 windows,
and KeepAspect 0/1. All 12 marker rectangles/counts and 24 edge samples match in
both actual application captures and the GPU-backed storyboard regression test.

Run native and Go captures sequentially on the private display described in
BUILD.md. Example:

```
python3 game/mugen/tests/storyboard_probe.py --fixture default-wide --name UNIQUE_NATIVE
python3 game/mugen/tests/app_smoke.py --name UNIQUE_GO --reference game/mugen/temp/storyboard-runs/UNIQUE_NATIVE/stock --motif data/mugen1/system.def --width 1280 --height 720 --seconds 3
python3 game/mugen/tests/storyboard_probe.py --fixture default-wide --name UNIQUE_CHECK --go-run UNIQUE_GO
```

Native inputs/goldens are frozen; Go runs cannot record them. The app check
validates captured file hashes, binary/config provenance and generated inputs.
M4bf's six comparisons, stock intro and mixed-stock match captures pass in
temp/m4bf-validation.log. The stock intro at 640x480 was visually inspected.
Scoped race checks pass: parser 1.013s, storyboard 1.787s, System 4.613s,
stock/resources 6.530s. Root module integrity passes. Full Match was not rerun
for this presentation-only change; M4be was the latest full-race result at
that checkpoint. M4bg subsequently passes a full scoped race run covering both
changes; see HIT-GROUND-TIMING.md.

This closes the demonstrated canvas-placement bug. It does not establish full
storyboard parity: no-clear persistence, arbitrary out-of-canvas content,
animation blending/rotation/interpolation, background controllers, music,
font/text variants and all fade timing/raster cases remain separate obligations.
Existing six alpha/two spark renderer failures remain unwaived.
