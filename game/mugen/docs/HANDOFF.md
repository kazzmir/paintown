# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Scope only game/mugen/;
local source/text commits, no pushes/binaries/assets/generated banks. Runtime
runs stay serial through the private wrapper; await exit, guard, then edit/run.
Reuse canonical assets/shared cache; remove disposable output after inspection.

Latest batch fixes documented background compatibility (supplied docs/bgs.html):
positionlink follows the immediately preceding element, recursively composes
start/motion/delta and no longer copies parent velocity a second time. Linked
moving floors publish the same position to characters and reset with the stage.
Animated parallax shares AIR interpolation, masks, BG Anim changes and clocks.
Width uses a centered SFF axis; legacy xscale shears without stretching; parallax
ignores vertical tiling/tilespacing. Infinite tiles follow the visible camera;
ordinary animation spacing measures between origins. Maskwindow follows stage
scaling/zoom. Trans overrides and independent AIR alpha use the shared renderer;
pre-1.0 trans=none retains AIR behavior. Disabled anims freeze/hide; invisible
anims continue advancing.

BG controller parsing is shared by stage/motif/storyboard prefixes. Groups are
ordered by preceding definition, not matching section labels; stages no longer
load duplicate groups. Positive group looptime, including1, wraps its clock.

The existing connected stage scenario now spans that complete lifecycle through
round reset, using a tiny text DEF and in-memory markers. Affected segments pass
with race detection. Linux rebuilt; one owned-window stock KFM/KFM720 stage
capture inspected. Capture/config overlay/log and segment scratch removed;
canonical assets/root modules preserved. No native matrices or asset copies.

Previous checkpoints cover mixed-team life/round budgets, Survival recovery and
pre5900 carry/counters/RNG, Turns order, co-op ownership and Watch/Arcade menus.
The user tested/pushed those checkpoints before this batch.

The ledger remains32/34 implemented rows (94% of checklist items, not native
conformance or effort). Remaining: exact raster/zoom/crop/color/shadow/alpha,
KO echo, native AI policy, camera/push ordering, strict content compatibility,
physical CD playback and absent licensed fonts. Background geometry follows the
beta documentation; native pixel-exact parallax/camera matching is unproven.
Linux remains the target; other platforms deferred. Preserve user content and
unrelated root/untracked work, including initial_placement_render_probe.py.
