# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Local checkpoint commits are authorized without individual approval. Recap about
every five commits; ask only for needed input. Never track user configuration.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Current legacy-asset batch (validated on Linux):

- PCX now decodes runs directly into visible rows, discarding padding without
  allocating a second full decoded plane. Cross-row runs and trailerless shared
  palettes are retained; visible indices outside a supplied palette are errors.
- V1 archives now enforce the same total decoded-pixel budget as V2 before
  allocating a frame. Linked images charge their source once; portrait loading
  charges only retained sources. PCX errors include sprite identity.
- FNT V1 settings/section headers accept inline comments; mapping suffixes
  preserve literal quote/semicolon glyphs. Malformed integer/map fields now
  retain file/line context instead of silently becoming zero.
- Existing storyboard/packed-character flow now spans a tiny padded-row V1
  font, bank changes across retained scenes, quoted/hex map characters, bad
  font metadata and bad packed PCX, followed by correction and successful reload.
  Existing SFF/font checks and affected match-flow pass with race detection;
  the other four segments stayed checked. No new test file or asset bank.
- Linux rebuilt and displays both authored font banks with expected colors.
  The bounded beta scene showed its background but no authored glyphs; legacy
  font native equivalence remains unverified. Do not claim native font parity.
  Current native/app captures, overlays, logs and helpers were inspected and
  removed; local YAML and root module hashes are unchanged.
- Prior a51b2146b retains mapping-specific texture revisions, shared fighter/HUD
  pixels and AfterImage interpolation/outfit snapshots. Four affected segments
  and palette/cache regressions passed with race detection; Linux matched five
  beta trail positions. Native/app scratch was removed.
- Prior ca131d22c retains native RGBA mask/upload/filtering behavior, lazy shared
  BG/storyboard textures and sprite indexes. Linux matched40 fixed beta samples.

Prior 764918190 retains bounded WAV/SFX playback, extensible PCM/float support,
Linux KO/rematch/KO and completed decoder/mixer/media race checks.

Prior 51f7ae0b3 bounds AIR collision allocation and covers camera motion. The
return precision gap stays open: beta 80.698 vs Go 80.848 at state time 120.

Previous 88428c4d9 batch provides ZIP character selection/rosters/quick matches,
selective portraits, CMD [Remap] for human/helper/AI, pack retirement and live
Game Speed. The user has approved continuing without individual commit review.

Ledger stays 32/34 checklist items, not native conformance or effort. Remaining:
the camera return above, other pause/hitpause and bindings, crop/raster/color/
shadow/alpha, KO echo, native AI policy, strict asset compatibility, physical CD,
legacy-font equivalence and absent licensed fonts. Linux is the target; other
platforms are deferred.
Historical matrices/raw renderer gates are not mandatory acceptance.

Local mugen.yaml remains ignored/untracked by 8eda46f84, with contents preserved.
The user will notify the contributor to back up YAML before pulling that removal.
Standalone music-file playback CLI remains deferred; in-stage music works.
Both upstream histories retain Go 1.26/Ebitengine 2.10.6/purego 0.11.3; root module
files stay read-only. Original go.sum backup remains at
`temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked`.
