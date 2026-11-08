# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Local checkpoint commits are authorized without individual approval. Recap about
every five commits; ask only for needed input. Never track user configuration.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Latest media batch:

- b6f684b72: bounded BGM read-ahead; playback cursors count consumed samples so
  NoMusic, loop edges and menu continuity remain exact for the connected flow.
- 32110fb35: cache SFX pan/gain/frequency factors on commands; retain native
  per-sample truncation and live pan/mute/unmute/owner retirement.
- 000617bb2: shared WAV metadata/sample decoding, including extensible PCM/float
  GUIDs and bounds/valid-bit validation, without reading whole music payloads.
- e29aa30b8: streamed/resampled PCM8/16/24/32 and float32/64 WAV music; Linux
  MIDI/tracker buffer reuse. The existing music scenario also spans optional
  RIFF chunks, late fmt, malformed replacement/recovery and float SND expiry.
- Current checkpoint: existing snd/sound regressions and both affected connected
  segments passed with race detection. The other three segments stayed checked.
  Linux rebuilt; owned-window KFM/KFM720 KO/rematch/KO passed with 82 balanced
  keyboard events and 1,980,955 private PCM frames. Authored float WAV remained
  audible after SFX ended. No sound/runtime errors; captures/PCM/overlays/helper
  removed. Root metadata and ignored YAML are unchanged. No user config tracked.

Prior 51f7ae0b3 bounds AIR collision allocation and covers camera motion. The
return precision gap stays open: beta 80.698 vs Go 80.848 at state time 120.

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
