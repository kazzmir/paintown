# Current handoff

Follow AGENTS.md. PROGRESS.md is the single completion ledger, now including
four checked known-difference checkpoints. Local source/text commits are
authorized; no pushes, binaries, generated banks or tracked user configuration.
Author only game/mugen/ and preserve unrelated/untracked work.

The prior known-difference batch is complete. The user then supplied sf2_mugen
for the first compatibility assessment; its executable matches the beta reference.

Current SF2 checkpoint:
- Fixed legacy Scene0bgDef/Scene0bg recognition, explicit empty roster slots and
  WAVs counting one omitted final alignment byte. Actual sample truncation stays
  rejected. Existing asset/packed-selection flows retain these cases; affected
  match-flow and SND checks pass with races. Other segments stay checked.
- Original pack intro/title, roster/portraits and music run in Linux. Its config
  selects data/mugen1/system.def. The README has the data-root launch command.
- Full fights still fail at data/mugen1/font/sf2win.fnt: Type=Fixed, Size=22,10,
  image398x10, with variable-style coordinate columns. Sequential fixed glyph T
  starts at418. Next establish native glyph/bounds behavior before relaxing
  font loading; do not simply relabel the font or edit the pack. Balrog/Vega
  gameplay remains unvalidated. Selection layout/cursor/portrait fidelity also
  needs compatibility work. A missing sound/blank.mp3 is a nonfatal pack warning.
- Pack, ZIP, bundled executable and user YAML are preserved. Scratch captures,
  PCM, overlays and input recipes were inspected/removed. No new fixture bank.

Prior closed scope:

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
