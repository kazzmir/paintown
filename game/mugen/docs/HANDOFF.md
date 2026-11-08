# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Local checkpoint commits are authorized without individual approval. Recap about
every five commits; ask only for needed input. Never track user configuration.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Current media batch (end-state validation pending):

- BGM decodes bounded 1,024-frame blocks instead of requesting one sample per
  decoder call. Playback position counts consumed samples; NoMusic, loop edges,
  menu continuity and replacement retain their existing cursor contract.
- SFX caches pan/gain/frequency factors on Play, SndPan and live volume changes;
  per-sample truncation stays unchanged. Owner stops compact the voice slice.
  The music lifecycle now spans live pan/mute/unmute/retirement during NoMusic.
- WAV metadata parsing now seeks over sample/optional chunks instead of reading
  the entire payload. SND and streaming consumers share validation and sample
  conversion, including PCM/IEEE_FLOAT extensible headers. Unknown GUIDs and
  inconsistent valid-bit/container/frame metadata remain explicit errors.
- The existing connected music lifecycle now crosses two blocks and a short
  final block. Run affected segments once after the complete media batch.
- Prior51f7ae0b3 provides bounded AIR collision allocation and a camera motion
  sequence. Its return precision gap stays open: beta80.698 vs Go80.848 at time120.

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
