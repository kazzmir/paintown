# Current handoff

Use PROGRESS.md as the single ledger and AGENTS.md for scope/execution. Work only
in game/mugen/; local source/text commits, no pushes/binaries/assets/generated
banks. Runtime runs are serial through the private host wrapper; await exit and
guard before editing/starting another run. Reuse canonical assets/shared cache;
remove run scratch after inspection on success/failure.

Latest batch closes StageInfo xscale/yscale and legacy hires defaults: scale BG
sprites/animation/positions/velocity about the stage origin, retain world camera
translation and fighter/HUD size, scale stage-centered mask windows and linked
BG floors. Hires defaults.5 are applied before explicit overrides. Plain zoffset
remains unscaled. One original1.1b1 scene measured xscale2/yscale.5: a16px marker
renders at[220,60,252,68], while P1 ScreenPosY200/GameWidth320 remain unchanged.
The Go connected stage scene agrees. Native debug text covers part of the marker;
this establishes its bounds/queries, not whole-image pixel equivalence. Control
preparation required complete PlayerInfo and a1.1 character declaration for
CameraZoom. All control assets/overlay/preview/helper were removed after review.

All changed-source connected segments pass with race detection; Linux executable
rebuilt. Source fingerprints now include background/coord/stage dependencies for
combat/entities/effects-pause so those changes reopen the relevant scenarios.
State-flow remains checked without rerun. Root module hashes are unchanged.

Previous checkpoints connect moving floors/dummy links, stage autoturn, BG
scalestart/scaledelta/windowdelta/maskwindow, camera zoom and colored reflections.
The connected scene spans those consumers and actual KO→floor/zoom/effect reset.
Standalone Cargo/KFM/KFM720 run inspected at4/8s showed visible fighters/intact HUD
and moving floor; its captures/config overlay/logs were removed. Earlier work
calibrates Turns2-vs-4/single Survival recovery and fixes Survival bout/reserve
carry while retaining human ownership and original roster position.

Remaining: native zoom easing/crop/raster and scaled camera/link/window
interactions; color/shadow/alpha, KO-echo playback and AI-policy precision;
mixed-team life/recovery, Survival dead Simul partner carry, position-link chains
and coupled camera/push ordering. Strict version/invalid-content rejection is
not certified. Physical CD and absent licensed fonts remain limitations. No new
fixture matrices or copied content were retained. Local stages/tracker assets and
unrelated root work remain untouched. Continue from the partial ledger items.
