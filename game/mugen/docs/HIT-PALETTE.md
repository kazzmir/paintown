# HitDef palette effects (M4br)

Target: immutable local M.U.G.E.N 1.1 beta 1. This checkpoint covers palfx.time,
palfx.mul and palfx.add, including an actual palette/rendering consumer. Numeric
routing becomes 64/81; nine literal schemas bring typed coverage to 73/81
(90.1%). Eight contact/fall shake keys remain. These are scoped schema and
consumer counts, not whole-engine completion.

## Schema, defaults and expression order

All components are integer expressions. Mul/add require exactly three values;
a partial tuple rejects. Float literals and unknown expressions reject before
state effects; runtime floats truncate and bottom becomes zero. Time defaults
to zero, add to 0,0,0 and mul to 256,256,256. Source maps remain immutable.
The measured order is after yaccel: time, add R/G/B, then mul R/G/B. Reversed
source declarations produce the same tagged sequence.

## Native color and lifetime rules

The beta computes (source + add) * mul / 256, then clamps the result to a byte.
Addition is not clamped before multiplication. Half values round upward.
For the native garment RGB247,239,231:

| Parameters | Native result |
| --- | --- |
| mul128,256,64 | 124,239,58 |
| add20,-40,5 | 255,199,236 |
| add20,-40,5 and mul128,128,128 | 134,100,118 |
| add-300,0,0 and mul-256,256,256 | 53,239,231 |

Signed multipliers are accepted and used, despite the manual's nonnegative
restriction. The negative-product control distinguishes this from clamping
multipliers or sums before the final operation.

Positive durations enable a temporary effect; -1 enables an indefinite effect.
Zero and values below -1 leave an existing hit effect untouched. Separate
second-contact programs verify default zero, explicit zero and -2 preservation,
and replacing the old effect with a new valid matrix. Guard and None contacts
do not install it. A finite-one program verifies eventual restoration; exact
native first/last visible-frame and broad pause combinations remain open.

## Rendering and clocks

Effects belong to mutable palette state over immutable sprite resources.
Rendering applies palette remapping first, then the hit matrix. Indexed images
retain their indices and transparent key; RGB images preserve alpha. HUD
portraits use palette mappings without the temporary hit matrix, with separate
filtered/unfiltered cache entries. Revision
changes invalidate GPU sprite caches when an effect begins, is replaced or
expires. Shared palettes advance once per world frame before new contacts,
with a separate lifetime for cloned palette state. A two-root regression
verifies that sharing does not double the clock.

These HitDef fields use the new consumer directly. The standalone PalFX,
AllPalFX and BGPalFX controller methods still need their own complete schemas
and behavior, including sinadd, invert/color and global/background handling.
This checkpoint does not promote those controllers from their earlier stubs.

## Evidence and limits

24 hitpalette-* programs pin 324 numeric observations, three native load
rejections and 96 pre-native CNS/AIR/CMD/stage hashes. Manifests under ignored
temp/: m4br-prepared-inputs.json, m4br-prepared-lifetime.json and
m4br-prepared-negative.json. No source inputs or expected numeric values were
changed after native capture.

The effect probe records twelve interior garment color samples for each of
19 accepted static programs, with source/profile/binary/PNG provenance. These
are scalar coordinates/colors, not reference artwork. Go cannot record native
expectations. The native measurements were taken independently from the target
executable. Production application comparisons check exactly the frozen colors,
including the lifetime, guard/None and signed-product cases. Each also checks
that its HUD portrait remains identical to a neutral Go baseline, matching the
native zero-delta result without claiming full portrait placement parity. No
tolerance is introduced. See tests/hit_effect_probe.py and HANDOFF.md for final results.

Native pixel coverage is bounded to opaque indexed KFM sprites. CPU alpha/RGB
support does not establish native filtered/partial-alpha conformance. Broad
helper/ownpal/foreign/projectile/multiple-contact/pause/retirement and complete
palette-controller behavior remain open. Existing strict RGB alpha/spark
failures, full recovery/round/mode/asset completion gates remain unwaived.

Numeric Match races pass16.085s, core/palette races pass, and final Character
races pass11.454s. Final20 production apps/21 captures pass provenance; all19
color/portrait comparisons pass. Visual review discovered an initial portrait
tint regression after body-only checks passed. The corrected cache/render path
and new neutral-portrait comparison resolve it; earlier artifacts remain
diagnostic. Representative palette/stock captures were visually inspected.
