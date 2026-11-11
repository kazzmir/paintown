# Actor transparency — M4cp

The typed Trans controller and actor renderer now share the native blend path
with Explods. Actor AIR transparency previously parsed successfully but was not
consumed by drawEntity. This checkpoint adds that missing production consumer,
including ordinary/helper/projectile actors that use drawEntity.

## Native schema and evaluation

The frozen corpus contains56 programs:41 accepted programs/492 numeric readings,
15 load rejections and630 RGB readings at14 flat palette colors. Four programs
use AIR alone; the others assert the actual Trans kind/count/state.233 unique
hashes pin all authored originals and the reference probe, including two
unpromoted publication diagnostics. Native images and inputs are immutable.

| Mode | Default source,destination | Actor effect |
| --- | --- | --- |
| omitted/default | unused | Preserve the AIR blend or an earlier override |
| none | unused | Ordinary texture-alpha drawing |
| add | 256,256 | Independent additive weights |
| addalpha | 256,0 | Source-only drawing |
| add1 | 256,128 | Additive drawing over half the destination |
| sub | fixed256,256 | Reverse subtraction |

Mode names are case-insensitive literals. Quoted/unknown/numeric names reject.
An omitted mode is accepted. Alpha accepts one or two integer expressions; an
omitted component uses the selected mode's default. Literal floats reject,
runtime floats truncate with warnings, missing redirects become zero and
weights clamp to0..256. Extra tuple components/unknown triggers reject with
file/state/controller context.

All modes validate and evaluate supplied alpha expressions in source then
destination order, even default/none/sub that ignore the resulting weights.
Default preserves a preceding same-frame override; none replaces it. This
behavior differs from skipping the default controller entirely.

## Ownership and lifetime

Overrides belong to an actor, independently of its shared palette. Native
parent/helper controls cover ownpal0 and1. An override expires at the next
active state pass; a frozen world retains the last rendered frame. Native
issuing-frame state transition controls retain transparency in the new state.
AIR values are read from the currently owned animation element when no actor
override is enabled. Collision boxes and simulation positions are unaffected.

The first two transition drafts captured the previous negative-state clipboard
before the new positive state published its observations. They are preserved
as diagnostics (actortrans-extra-transition and -transition-next), not promoted
as observations. The separate -transition-published control explicitly publishes
both clipboard lines after the destination-state snapshot and before freezing.
No original was changed or overwritten to correct that setup.

## Verification boundary

All41 actual applications match native RGB crops exactly:108,675 pixels
each/4,455,675 pixels total. All630 color samples match too. Stock win/rematch/win
is visually inspected, with82 balanced events and1,943,231 private PCM frames.
Sessions were awaited; a separate client check is empty. HANDOFF.md records
continuation details. The existing540-sample GPU corpus verifies the shared alpha
arithmetic independently of the new controller state/evaluation.

These controls cover opaque indexed source sprites and the named AIR/controller
combinations. Partial RGBA sources, every hitpause/foreign-state combination,
full framebuffer/platform sampling and palette-effect composition need their
own evidence. Source routing counts are not an overall completion percentage.
