# Shared floor shadows — M4da

The supplied beta renders one shadow for the current root/helper pose and for
projectiles/Explods with their shadow flag enabled. Historical AfterImage poses
do not cast additional shadows. The shared pass is after stage background,
before actors, foreground, and an under EnvColor quad.

## Native evidence

98 original programs:88 visual/clock controls and10 Explod creation schema
controls.95 accepted,3 rejected;1140 captured numeric values. The first88 have
354 frozen source/probe hashes; the additional10 ledger pins43 recording inputs.
Every native-only row and all seven review sheets were inspected before promotion.
Original sources, recorders, captures, configurations and expectations are immutable.
Configuration finalization is exactly FirstRun1->0 and LF->CRLF for all98.

Defaults are intensity128 and yscale0.4, including absent/empty Shadow sections.
Color is deprecated and ignored. Intensity clamps0..256. Fade endpoints use raw
common GAME coordinates: at CNS Y=-50, fade[-400,0] gives half opacity at game1280
and threequarters at game640. Equal/reversed endpoints disable fade.

Local PalFX leaves the shadow unchanged. Global PalFX acts through final scene
composition. AIR additive/subtractive modes suppress shadows; AIR A1 and Trans
destination128 halve opacity. Source-alpha factors do not determine shadow opacity.
Local noshadow/invisible suppress the issuing actor; issuer flags do not hide
its subordinate projectiles/helpers/Explods. Globalnoshadow asserted by any active
root/helper suppresses the whole pass, and resets every game tick during Pause.
Frozen actor flag maps cannot implement that global lifetime.

Geometry follows current AIR axes/flips/offsets, AngleDraw/scale, stage yscale,
xshear, character shadowoffset and camera/floor coordinates. Indexed center and
reversed-axis biases are applied in the projected surface. Source addressing is
determined before the stage shear; its dominant screen direction may change
without changing texel order. Stage shear includes a measured half-game-pixel
indexed origin correction. Helper shadowoffset moves the projected floor by
(1+yscale)*offset. The three priority-overlap native crops have identical hashes.

Explod creation now compiles shadow as an integer expression. Bare float literals,
malformed expressions and unknown triggers reject with source context; expression
results truncate. The native expression-order control gives1234 for pos,
sprpriority,shadow,removeongethit. Missing animations skip shadow evaluation.
ModifyExplod's existing shadow mutation reaches the same scene consumer.

## Preserved counterexamples and current limits

Initial eight-app baseline had seven missing-shadow failures and one exact
noshadow control. Original88 numeric replays passed because shadows are visual.
The new10-case schema baseline failed five subcases: three absent validations,
expression order and a skipped shadow side effect. All98 native replays pass
under focused races after the fix.

Matrix1 completed all88 real apps,31 exact/57 UNWAIVED. Four Explod shadows were
missing1818 pixels each. Shear controls differed161/363 pixels, max54/89. Focused2
resolves those visible/schema gaps; its eight crops retain exact strict failures:
mostly90 one-byte fringe differences, shear112/104 one-byte differences, and the
scaled actor468 pixels/max2. Those are NOT waived or labelled exact. The shared
RGBA viewport interpolation remains unchanged from M4cz. Private signed/normalized/
odd-rounding experiments did not fit every native color and were not adopted.

## Completed integration

All 98 native programs pass in the full scoped race suite: **4,506 test/subtest
results across 36 packages** (25 tested, 11 compiled-only). Match took 2,021.992s.
No individual test skips, failures or race reports occurred.

All 95 accepted applications opened and captured successfully. All result rows
and five paired native/Go sheets were inspected. **35/95 crops are exact**
(36.8% of this named image corpus), covering 17,325,600 RGB pixels. The strict
matrix exits 1 for **60 UNWAIVED differences**: 59 have maximum channel error 1;
the scaled actor differs at 468 pixels with maximum error 2. Ordinary shadows
typically differ at 90 fringe pixels. Shear controls now differ at 112/104 pixels,
maximum 1. Passing state tests does not waive any pixel failure.

The two old projectile-shadow controls now match exactly, each over 508,800 RGB
pixels. The other 152 historical projectile image cases were not rerun here.
All four old shake masks pass. Stock captures at 21/26/45 seconds show
win → rematch → win; 82 balanced window-owned keyboard events and 1,973,562
private PCM frames pass provenance/audio checks.

The audit verifies 17,312 unique frozen paths, 52,979 hash checks, 1,043 original
native inputs, all 98 native configurations, 41 canonical resource archives and
4,018 native resource copies. All 109 original distribution manifest files are
unchanged. Binary/source/config/capture/audio/module/inventory checks pass.
Private records: temp/m4da-final-evidence-audit.json and
temp/m4da-final-visual-resource-review.json. No executable or capture is committed.

Wider resource/profile, RGBA, perspective, contact/pause and reflection variants
remain explicit compatibility work. Earlier angle/EnvShake differences remain
UNWAIVED. No whole-engine completion percentage follows from this corpus.
