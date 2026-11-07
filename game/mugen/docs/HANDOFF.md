# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Present the complete change list and obtain approval BEFORE each local commit.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Last reviewed commit 2bf8df056 implements raw game-unit camera startup and direct
modern/legacy stage projection, including BG-only legacy camera truncation.

Latest batch fits camera regions using temporary Width edge bars plus stage
tension, clipping that region to fixed stage edges before choosing zoom/center.
A clipped edge stays anchored if zoomout cannot fit the whole region. Permanent
containment margins are excluded. ScreenBound=0 intentionally allows offstage
roots/helpers; do not add a global clamp to those actor positions.

A bounded beta flow with bounds+/-100/zoomout.5 confirms disabled tracking,
tracking actors beyond one stage edge and asymmetric Width edge20,30 fitting.
The existing connected camera scenario retains exact observations and continues
through mixed 320p/1280p coordinates, Pause and legacy-stage round reset, checking
that camera fitting never changes the offscreen root positions. All four
changed-source segments pass with race detection; state-flow stays checked
without rerunning. Native helpers, symlink overlays, captures and logs removed.

Prior cold-start, tension/zoom/bounds, cropped HD modern/legacy BG/fighter/effect
projection, foreground/mask and KO/reset observations remain covered. Historical
matrices are not required; no raw fixture bank was added.

Local mugen.yaml remains ignored/untracked by 8eda46f84, with contents preserved.
The user will notify the contributor to back up YAML before pulling that removal.
Standalone music-file playback CLI is deferred by user request; in-stage file
music works, and the README lists optional Linux tracker/MIDI dependencies.
Both upstream histories retain Go 1.26/Ebitengine 2.10.6/purego 0.11.3. Root
module files stay read-only; original go.sum backup remains at
`temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked`.

Ledger remains 32/34 checklist items, not native conformance or effort. Remaining:
intermediate camera easing/push ordering, other crop/raster/color/shadow/alpha
edges, KO echo, AI policy, strict asset compatibility, physical CD and absent
licensed fonts. Linux target; other platforms deferred.
