# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Local checkpoint commits are authorized without individual approval. Recap about
every five commits; ask only for needed input. Never track user configuration.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Latest batch checkpoints:

- a010d8e2d: ZIP/default/alternate DEF loading through selection, portraits,
  intros/endings, rosters, quick matches and resets; spaced filenames preserved.
  Engine-owned temporary expansion handles errors and termination cleanup.
- e841c13e9: selective SFF v1/v2 portrait loading follows linked pixel sources
  and palette sequencing; gameplay continues to load the complete archive.
- 43bf5139c: CMD [Remap] works for human/key-controlled-helper input and authored
  AI. Beta confirms logical x=a and blank a semantics. Chords, charged releases,
  Start, disabled slots and remapped stock AI attack/special/contact are covered.
- 97fb7c45f: title return releases inactive pack files after stopping packed
  music; selection reloads fresh paths/portraits. Game Speed updates the live
  scheduler and subsequent match clock. No settings file is written.
- Current checkpoint records rebuilt Linux ZIP startup/rendering and signal
  cleanup. One inspected private-window capture showed both KFM profiles; no
  runtime/audio warnings or surviving pack expansions. Canonical banks were
  shared; test ZIPs contained tiny DEF references only. All scratch removed.

Four affected connected segments passed with race detection for CMD changes;
subsequent asset/menu changes reopened only match-flow, which passed. All five
segments are checked for current source. Root module hashes and ignored local
YAML content remain unchanged. Earlier 848b00caf batch retains storyboard/frame/
fade, font-path, PCX and playable KO/rematch coverage.

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
