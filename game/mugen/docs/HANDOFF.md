# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Present the complete change list and obtain approval BEFORE each local commit.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Last reviewed commit 1a2c8ec0b implements vertical/airborne body collisions and
shared screen clipping before CNS, preserving the measured late-push wall order.

Latest batch fixes PosFreeze/body-push interaction. PosFreeze blocks ordinary
velocity movement while the actor still participates in late body separation.
Remove both the scheduler's frozen-actor exclusion and the opponent exclusion;
retain real Pause/SuperPause and hitpause scheduling gates.

One bounded beta lifecycle with stored VelX 2 gives positions no-freeze(-6,26),
first frozen(-7,25), second frozen(-5,27), both frozen(-6,26). Velocities 2/2 and
CNS times 350/350 continue. Exact observations live in the extended connected
jump/body scenario and feature catalog. It retains prior interior/wall/vertical/
Width/air phases, uses mixed 320p/1280p actors and checks freeze release, real
Pause and round reset clearing velocity/freeze flags. All four changed-source
segments pass with race detection; state-flow stays checked without rerunning.
Native helper, symlink overlay, capture and logs were inspected and removed.
Linux rebuilt successfully; root module hashes remain unchanged.

Prior camera startup, tension/zoom/bounds/Width/offscreen and cropped HD modern/
legacy BG/fighter/effect/foreground/mask observations remain covered. Projectile
contacts intentionally do not recoil the owner; preserve that adapter exclusion.
Historical matrices and old raw renderer gates are not required acceptance.

Local mugen.yaml remains ignored/untracked by 8eda46f84, with contents preserved.
The user will notify the contributor to back up YAML before pulling that removal.
Standalone music-file playback CLI remains deferred; in-stage music works.
Both upstream histories retain Go 1.26/Ebitengine 2.10.6/purego 0.11.3; root module
files stay read-only. Original go.sum backup remains at
`temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked`.

Ledger remains 32/34 checklist items, not native conformance or effort. Remaining:
camera easing, asymmetric pause/hitpause and binding combinations, crop/raster/
color/shadow/alpha, KO echo, AI policy, strict asset compatibility, physical CD
and absent licensed fonts. Linux target; other platforms deferred.
