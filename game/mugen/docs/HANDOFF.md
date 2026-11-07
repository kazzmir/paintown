# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Present the complete change list and obtain approval BEFORE each local commit.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Last reviewed commit 4b7e9dfca implements offscreen camera fitting with temporary
Width edge bars and stage clipping of the fitting region, not actor positions.

Latest batch fixes body push: require vertical Clsn2 overlap and allow S/C/A
participants, using existing ground/air widths and temporary Width player bars.
The late body-push phase remains intact. A shared boundary pass before CNS makes
redirected Pos queries observe other actors' clipped prior-frame displacement;
per-entity clipping still handles cameras changed by earlier actors.

One bounded beta flow pins interior, wall, vertical separation, Width, air/ground
and air/air positions. Values live in the extended jump/landing scenario and
feature catalog. The scenario uses mixed 320p/1280p actors, preserves the native
wall half-step, and continues through Pause and round reset. Redirected Pos X
observations use caller coordinates. All four changed-source segments pass with
race detection; state-flow stays checked without rerunning. Native helper,
programs, symlink overlays, captures and logs were removed.
Linux rebuilt successfully; root module hashes remain unchanged.

A separate projectile/direct/helper beta flow confirms projectile contacts do
NOT recoil the owner, even with explicit -12 cornerpush. Direct/helper attackers
move 130 to about 93.294; receiver life confirms all contacts. Keep the adapter's
projectile exclusion; its misleading TODO and historical note are corrected.
No owner-directed projectile impulse was implemented.

Prior camera startup, tension/zoom/bounds/Width/offscreen, cropped HD modern/
legacy BG/fighter/effect, foreground/mask and reset observations remain covered.
Local mugen.yaml remains ignored/untracked by 8eda46f84, with contents preserved.
The user will notify the contributor to back up YAML before pulling that removal.
Standalone music-file playback CLI remains deferred; in-stage music works.
Both upstream histories retain Go 1.26/Ebitengine 2.10.6/purego 0.11.3; root module
files stay read-only. Original go.sum backup remains at
`temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked`.

Ledger remains 32/34 checklist items, not native conformance or effort. Remaining:
camera easing, push/freeze/binding combinations, crop/raster/color/shadow/alpha,
KO echo, AI policy, strict asset compatibility, physical CD and absent licensed
fonts. Linux target; other platforms deferred. No historical matrices.
