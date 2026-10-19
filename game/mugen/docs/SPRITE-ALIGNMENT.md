# Actor frame flips, offsets and pixel alignment

M4aq compares 28 native programs: 336 numeric observations, 140 authored input
hashes and 28 original red-marker raster crops. These controls separate game
coordinates, local/size scaling and the final viewport fit.

M4ay corrects the earlier sampling interpretation and now matches all 28 crops
byte-for-byte, including the four former one-channel edge differences. See
[SPRITE-SAMPLING.md](SPRITE-SAMPLING.md) for the added patterned controls,
integer indexed pixel centers, fractional RGB quads and native filtering.

## Native projection rules

AIR X offset multiplies character Facing, including when AIR H cancels the
image flip. AIR Y offset keeps its direction under V. The +5/-3 controls move
fifteen physical pixels horizontally and nine up in the 1280x720 window.
Facing XOR AIR H selects horizontal flipping; AIR V selects vertical flipping.

Each effective flipped axis gets one game-surface pixel after local/size
scaling, before viewport fitting. In the pinned physical window that is three
pixels at game width 320, 1.5 at 640 and .75 at 1280. Cancelled flips have no
shift. Nonuniform 2.5/.5 helper sizing keeps the same game-pixel correction;
left/HV exact-red counts are 11,472/11,233.

These RGB integer/half-pixel markers originally suggested floored draw axes.
M4ay's additional fractional and patterned controls reject that generalization:
RGB quads retain fractional geometry, while indexed sampling uses integer game
pixel centers. The earlier renderer's explicit flooring concealed Ebiten's
rectangle vertex snapping and did not establish a native flooring rule. The
original positions, clipped widths, 1.1 helper extents and AIR offset goldens
remain unchanged and pass with the corrected sampler. Quarter-local-unit X
gives one full game pixel at game width 1280.

The first width controls also resized the physical window because native video
width defaults to GameWidth. Those unintended raster runs are diagnostic only.
Accepted width controls explicitly pin Video Width=1280 and Height=720. StageFit
derives render height from the 320x240 stage aspect. Stage debugbg=0 gives black
behind the original marker, isolating sampling from debug/background differences.

## Raster provenance and precision

The permanent tests/sprite_render_probe.py validates native executable/input/
config/video/capture provenance. Go verification additionally checks binary,
prepared CNS, AIR, marker, stage and render profile. Go output cannot record a
native golden. Compressed native RGB crops contain only the original red/black
marker data and validate against the saved crop SHA-256.

All 28 final-code Go crops match native geometry and exact-color counts. Twenty-
four complete crop rasters are byte-identical. Four have sparse blended-corner
color differences: air-hv and helper-scaled-hv differ at one pixel; left-640 and
right-offset-640 differ at four. Maximum channel error is one. Strict SHA checking
is the default. Explicit --max-edge-channel-error 1 permits at most four blended
pixels; pure red/black pixels and all geometry/counts remain exact. The report
retains the actual differing-edge count. This allowance still rejects the earlier
half-pixel position failure. No renderer color correction is introduced.

## Validation

Full scoped races pass on final code in temp/m4aq-final-full-race.log (Match
190.318 seconds). All 28 state/profile cases pass m4aq-axis-frame-race.log.
The earlier m4aq-full-race.log precedes final axis flooring and is intermediate
evidence only. Pre-fix and first corrected controls are preserved under
m4aq-before-comparison.json and m4aq-first-comparison.json.

Thirty-nine real 1280x720 captures pass provenance/dimension/nonblack/runtime/
resource checks from one identical final binary. Marker results are in
m4aq-axis-raster-summary.json and m4aq-axis-go-checks.log. Reverse-helper
foreground/color/rectangle matches native. The real-art overlap and helper-fall
clothing masks now match exactly: 22,362 and 16,608 white pixels respectively,
zero mask differences versus the previous 408/801. Stock recovery retains two
fighters without permanent dust clones. See m4aq-final-app-checks.json.

Four final independent native replays pass: offscreen-left, helper-fractional-
offset, right-eighth-y and left-640 under m4aq-final-replay-*. Source authors/catalogues and disposable captures remain in temp;
only original source/marker/goldens are committed. No reference artwork/binary
is added, and root Go module hashes remain unchanged.

This does not complete foreign animation/sprite-resource combinations, AIR
interpolation/scale/rotation/blending, every size/zoom/pause/profile combination,
effect/projectile alignment, debug/background/HUD behavior or whole framebuffer/
asset compatibility. The four quantified corner-precision differences remain
visible in the evidence.
