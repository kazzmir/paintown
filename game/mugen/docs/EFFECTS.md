# Contact effect scene and native evidence

M4p adds a display-independent effect queue/scene, typed HitDef spark resources
and production hit/guard drawing. The scene owns animation clocks and retires
finite actions after one pass. Explicit infinite AIR frames remain active.
New events display their initial frame; existing clocks advance before the next
contact pass. Match round reset/exit clears queued/active resources. These are
foundations for contact effects, not complete Explod/GameMakeAnim/render parity.

Character contact emits common game-space positions. Spark X is relative to the
defender body edge toward the attacker; negative X moves into the body. Spark Y
is relative to the attacker. Current body front/back/air widths and width-ratio
conversion are used. Drawing applies attacker-facing flip, AIR sprite axes and
frame offsets, and source pixel scale. Own effects use the attacker's AIR/SFF/
palette state; common effects use fightfx AIR/SFF and the fight scale mapped
from motif to game units. Helpers/projectiles inherit the queue/resources, but
complete world/custom-state/ownership/retirement behavior is still open.

The production renderer caches immutable sprite pixels with palette revisions
and prunes inactive keys. AIR now retains A, A1, S and ASnDm draw metadata; effects
consume additive, subtractive and explicit weights. Unknown/malformed draw-type
syntax gets action/line errors. Normal and additive own markers were inspected
through actual owned Go windows. Complete AIR blending/interpolation/angle/scale,
RGBA edge fidelity and other renderer consumers still need native comparisons.
The added metadata does not make Character/HUD/AIR rendering fully compatible.

## Reference scenes

Eight original authored programs use the existing original formats.sff RGB
ramp. Blank fighters isolate one persistent hit/guard marker. Seven geometry/
resource scenes cover own hit/guard, offsets, common selection/defaults, explicit
-1 and a missing common action that must not fall back to the own action. Five
markers are visible; two are absent after confirmed contacts. The eighth program
measures [Data] spark resource-prefix coercion numerically: S9992/S9993 are
accepted by the beta but Const returns zero, and they do not select own effects.
The loader now preserves that measured behavior instead of rejecting them.

`tests/spark_probe.py` owns native/window capture through reference_probe.py and
locates the authored RGB ramp core. Source/AIR/SFF/executable/window/core hashes,
numeric packets, geometry and disposable common-resource recipe hashes are
pinned. Calibration is deliberately limited to these static 320x240 KFM scenes,
zero camera, zoffset 200 and flipped marker. It is not general screenshot parity.
Native common-default and guard replays reproduce both core hashes and geometry.

Measured frame anchors in 240p space are (-50,-50) for hit, (-53,-48) for guard
with AIR offset (3,2), and (-70,-70) for the alternate hit offset. The native
1280x720 window shows the eight-by-four marker at scale three. Bilinear filtering
leaves a 23x12 solid RGB core, whose orientation confirms the facing flip. Go
Character/scene tests compare the measured frame anchors and resource owner;
they do not assert identical framebuffer framing/filtering.

M4q corrects the measured M4d framing discrepancy: world and HUD fit separately,
and stage-aspect game dimensions come from native numeric probes. The actual
1280x720 Go capture temp/app-runs/viewport-spark-hd/ shows the expected margins
and marker projection. Filter-edge pixels still differ; background rasterization,
camera/zoom and full-frame image parity remain open. See COORDINATES.md.

## Validation and limits

Full scoped races pass, plus display-independent engine/effect/AIR races. Tests
cover independent clocks, finite expiry, explicit infinite lifetime, clear,
resource/position errors, typed expression host isolation, eight native programs
and real KFM common spark emission during logical walk/punch/hit/recovery.

Owned-window production captures are temp/app-runs/spark-own-production and
spark-add-production. The original normal marker is visible; adding AIR draw
type A visibly adds its RGB to the stage. Inspection metadata and pixel samples
are in temp/app-runs/spark-gpu-inspection.json. They prove the real GPU route
consumes effect/blend data, not native full-frame/blend fidelity. Display/audio
stay routed to the existing private Xvfb/Pulse setup; no desktop changes.

Open: exact birth/expiry/hitpause/Pause/SuperPause boundaries, moving camera/zoom,
mixed-profile/native scaling edges, dynamic Width, behind/equal-axis contacts,
priority/layers, effect palette/PalFX/custom-state ownership, full Explod family
and configuration/capacity fallbacks. Scene limit defaults to 256 if not supplied;
production passes SysExplodMax. Its limit and the 16384 undrained queue bound are
diagnosed implementation limits, not native fallback claims.
