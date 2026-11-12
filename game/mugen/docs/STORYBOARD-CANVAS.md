# Storyboard canvas and output scaling

Current origin rule: layers and scene windows start at the fitted storyboard's
top-left. `layerall.pos` supplies authored placement (stock KFM uses160,0); a
background's centered origin must not be added again. The connected asset
scenario includes stock KFM to catch the reported right-shift regression while
retaining side overflow and uncleared scenes. Current status is in
[PROGRESS.md](PROGRESS.md); the measurements below are historical background.

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

Cases cover omitted 320x240, explicit 640x480 and 1280x720, widescreen/4:3
windows, and KeepAspect 0/1. All 12 marker rectangles/counts and 24 edge samples
match in both actual application captures and the GPU-backed storyboard regression
test.

Run native and Go captures sequentially on the private display described in
BUILD.md. Example:

Native inputs/goldens are frozen; Go runs cannot record them. The stock intro at
640x480 was visually inspected. Root module integrity passes.

This closes the demonstrated canvas-placement bug. It does not establish full
storyboard parity: no-clear persistence, arbitrary out-of-canvas content,
animation blending/rotation/interpolation, background controllers, music,
font/text variants and all fade timing/raster cases remain separate obligations.
Existing six alpha/two spark renderer failures remain unwaived.
