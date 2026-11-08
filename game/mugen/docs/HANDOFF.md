# Current handoff

Follow AGENTS.md. PROGRESS.md is the single completion ledger, now including
four checked known-difference checkpoints. Local source/text commits are
authorized; no pushes, binaries, generated banks or tracked user configuration.
Author only game/mugen/ and preserve unrelated/untracked work.

The user-requested known-difference batch is complete. Stop for the user before
starting compatibility and deferred work.

- Camera: preserve the unclipped participant midpoint before final stage limits.
  All eleven beta camera/zoom samples match through time 120 (X 80.6977).
- Rendering: fixed 3/8 indexed calibration, unsnapped source addresses and
  delta-first ties-to-even byte filtering close sprite/shadow/RGBA differences.
  OnTop effects blend over HUD before final AllPalFX; projected clipping keeps
  parent coordinates. Rebuilt Linux output matches all four target beta RGB
  regions exactly. Connected finite AIR blends, pause/expiry, HUD coverage,
  letterboxes and global-palette composition pass.
- KO echo: both current native and Go device captures pass the existing frozen
  seven-segment reference, including the formerly missing quiet interval. Three
  Go cue levels match exactly; relative starts differ at most 0.05s, within the
  unchanged 0.35s capture criterion. Historical mismatch no longer reproduces;
  retain the scheduler. Sample-exact device synchronization is not established.
- All five connected segments are checked with race detection; existing render
  color regressions also pass with races. Linux binary rebuilt. All owned
  scratch/captures/PCM/logs/overlays were inspected and removed. Retain the single
  ignored temp/bin/mugen-go executable and shared cache.

Checklist stays 32/34 implemented items, not conformance or effort. Next work is
compatibility: representative external characters/stages, legacy/font behavior,
unverified pause/binding/crop/filtering combinations and native AI policy.
Physical CD audio, absent licensed fonts and other platforms remain deferred.
Do not regenerate historical matrices or reopen completed segments by default.

Retain shared asset/ZIP/portrait lifecycles, legacy PCX/FNT diagnostics and budgets,
RGBA byte uploads, palette mapping revisions and saved AfterImage AIR clocks.
Retain consumed-vs-read-ahead music cursors, shared PCM/float WAV decoding, reused
Linux codec buffers and SFX quantization. Earlier details are in Git history.

Use the private round-task-env.py wrapper with GOTOOLCHAIN=go1.26.9; runtimes stay
serial: await exit, separate owned-client guard, then edits/new runs. Root module
files stay read-only; use mugen.mod/mugen.sum. Preserve ignored local mugen.yaml.
Standalone music-file playback CLI remains deferred. Root go.sum backup remains
at temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked.
