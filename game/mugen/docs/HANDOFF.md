# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Local checkpoint commits are authorized without individual approval. Recap about
every five commits; ask only for needed input. Never track user configuration.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Latest checkpoint:

- AIR count validation prevents negative/oversized allocations. A shared
  1,048,576-box ceiling includes defaults and per-element copies. Packed-character
  fight loading reports the bad file/action/line; correcting the same expansion
  loads successfully. This is port error handling, not native rejection parity.
- One stable world-space camera sequence covers spread, translation, closing,
  stage-bound travel and return/reset inside the existing camera scenario.
  Camera X/zoom matches five beta samples at state times 1,10,21,41,81. Return at
  time 120 remains about 0.15 pixels apart (beta 80.698, Go 80.848). No camera runtime
  change was needed for the five matching points; do not label the tail exact.
- Four affected segments pass with race detection; unchanged state-flow stays
  checked. Linux executable rebuilt. Native assets were symlinked; all current
  helper/captures/overlay/logs removed. Root metadata and ignored YAML unchanged.

Previous 88428c4d9 batch provides ZIP character selection/rosters/quick matches,
selective portraits, CMD [Remap] for human/helper/AI, pack retirement and live
Game Speed. The user has approved continuing without individual commit review.

Ledger stays 32/34 checklist items, not native conformance or effort. Remaining:
the camera return above, other pause/hitpause and bindings, crop/raster/color/
shadow/alpha, KO echo, native AI policy, strict asset compatibility, physical CD
and absent licensed fonts. Linux is the target; other platforms are deferred.
Historical matrices/raw renderer gates are not mandatory acceptance.

Local mugen.yaml remains ignored/untracked by 8eda46f84, with contents preserved.
The user will notify the contributor to back up YAML before pulling that removal.
Standalone music-file playback CLI remains deferred; in-stage music works.
Both upstream histories retain Go 1.26/Ebitengine 2.10.6/purego 0.11.3; root module
files stay read-only. Original go.sum backup remains at
`temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked`.
