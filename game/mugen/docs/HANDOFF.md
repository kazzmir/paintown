# Current handoff

Use PROGRESS.md as the single ledger and AGENTS.md for execution/scope. Author
only game/mugen/; local source/text commits, no pushes/binaries/assets/generated
banks. Runtime runs are serial through the private host wrapper. Await exit,
guard clients, then edit or start the next run. Reuse canonical assets/cache;
remove per-run scratch after inspection on success/failure.

Latest stage batch connects dummy BGs and zoffsetlink through position-linked
velocity/sinusoidal motion to fighter/effect/shadow/reflection floors and CNS
viewport queries. Stage autoturn now controls automatic facing (default1).
Normal/animated BGs consume scalestart/scaledelta; windowdelta moves their clip
without moving the sprite, and1.1 maskwindow uses the stage-centered origin and
exclusive bottom/right edges. Tiny/zero-coordinate clips stay meaningful.

The existing connected stage scenario now covers those consumers alongside
startzoom/separation/return, mixed-coordinate constraints, world/BG/screen-effect/
HUD projection, reflections/suppression and actual KO→floor/zoom/effect reset.
All changed-source segments pass with race detection; completed state-flow stays
checked without rerun. Linux standalone rebuilt and Cargo/KFM/KFM720 owned-window
run inspected at4/8s: visible fighters, intact HUD and moving floor. Both PNGs,
config overlay and logs removed; private client guard passed. Root modules intact.

Earlier local batches calibrate Turns2-vs-4 and single-player Survival recovery,
fix one-opponent Survival bouts and retain the human Turns reserve's original
roster position/life/power across opponents. Zoom and stage reflection consumers
are implemented; native easing/blend/raster precision remains unverified.

Remaining asset consumer gap: StageInfo xscale/yscale. Native parsing was located
read-only at42338d..4233a5 (stage fields368/36c); rendering semantics are not yet
established. Keep it explicit in the asset partial. Further precision remains:
color/shadow/alpha, KO-echo playback, AI policy, mixed-team life/recovery, Survival
dead Simul partner carry, position-link chains and coupled camera/push ordering.
Strict invalid-asset rejection is unproven. Physical CD and absent licensed fonts
remain limitations. All local stage/tracker assets stay ignored and untouched.
