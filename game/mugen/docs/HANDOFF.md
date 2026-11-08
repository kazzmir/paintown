# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Present the complete change list and obtain approval BEFORE each local commit.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Latest approved local commit 375753396 preserves body push during PosFreeze.
Prior camera and vertical/airborne/body-push observations remain covered.

Current checkpoint extends the existing entity lifecycle through moving-owner
BindToRoot helpers with Pause allowances 2/0, two pauses, body contact, helper
retirement and round reset. Existing runtime matches beta: the helper allowance
is spent across pauses, frozen helpers retain their bound pose, and late body
push follows binding. Exact observations are in the scenario and feature catalog.
No runtime change or binary rebuild was needed. Only the changed entity segment
ran, passing with race detection; all five segments remain checked. Native
helper, capture, symlink overlay and logs were inspected and removed. Root module
hashes and local YAML content remain unchanged. This checkpoint awaits review.

Ledger remains 32/34 checklist items, not native conformance or effort. Remaining:
camera easing, other pause/hitpause and target-binding combinations, crop/raster/
color/shadow/alpha, KO echo, AI policy, strict asset compatibility, physical CD
and absent licensed fonts. Linux target; other platforms deferred. Projectile
contacts intentionally do not recoil the owner. Historical matrices and old
raw renderer gates are not required acceptance.

Local mugen.yaml remains ignored/untracked by 8eda46f84, with contents preserved.
The user will notify the contributor to back up YAML before pulling that removal.
Standalone music-file playback CLI remains deferred; in-stage music works.
Both upstream histories retain Go 1.26/Ebitengine 2.10.6/purego 0.11.3; root module
files stay read-only. Original go.sum backup remains at
`temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked`.
