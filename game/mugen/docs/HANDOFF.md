# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Local checkpoint commits are authorized without individual approval. Recap about
every five commits; ask only for needed input. Never track user configuration.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Latest approved local commit 64fd8adf5 covers moving-owner helper binding across
asymmetric Pause allowances, body contact, retirement and reset. Prior PosFreeze,
camera and vertical/airborne/body-push observations remain covered.

Current checkpoint fixes TargetBind velocity publication during attacker
hitpause. Targets still follow the anchor's position, but receive zero velocity
until it resumes. Copying stored velocity prematurely moved victims after a
finite bind expired. The connected entity scenario now includes an asymmetric
throw, expiry without drift, own-state recovery/control and round reset, using
mixed character coordinates. Beta observations are recorded in the scenario.
All four changed-source segments pass with race detection; state-flow stays
checked without rerunning. The scenario's placeholder/recovery setup was corrected
before completion. Linux rebuilt; native helper, capture, symlink overlay and
logs removed. Root module hashes and local YAML content remain unchanged.
This checkpoint was approved; subsequent local checkpoints may proceed without
individual approval under the user's renewed five-commit recap workflow.

Ledger remains 32/34 checklist items, not native conformance or effort. Remaining:
camera easing, other pause/hitpause and binding combinations, crop/raster/
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
