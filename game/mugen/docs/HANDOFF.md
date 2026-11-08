# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Local checkpoint commits are authorized without individual approval. Recap about
every five commits; ask only for needed input. Never track user configuration.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Latest sprite/rendering batch:

- 9cd399b2f: PNG32/raw RGBA unmasked views force opacity after native byte RGB
  premultiplication, including opaque black at fully transparent texels.
- de459a839: true-color backgrounds use linear filtering; indexed backgrounds
  retain nearest sampling. The existing stage flow covers native mask/scale
  colors, mask changes, visibility and reset.
- 50523b4de: background/motif/storyboard textures upload on demand, sharing
  identical immutable pixel views and preserving mask variants. Hidden frames
  upload nothing; replacing a background archive replaces its image map.
- dce0e6277: parsed SFF group/item indexes serve actor/effect/BG lookups.
  Caller-built archives retain dynamic fallback; missing effect pixels warn.
- 085a8a59d: the native pixel check exposed Go's brighter 16-bit RGBA conversion.
  Shared UploadPixels now preserves native byte truncation across actor,
  effect, background, HUD/selection portrait and bitmap-font uploads.
- Current acceptance: four affected segments pass with race detection after
  the correction. Unchanged state-flow stayed checked. Existing SFF/background
  regressions passed. Linux rebuilt and its RGBA scene matches all 40 fixed beta
  RGB samples with unchanged expected values. Both native/Go scenes were viewed;
  no runtime warnings. All current helpers/captures/logs/overlays removed.
  Root metadata and ignored YAML remain unchanged; no user config tracked.

Prior 764918190 retains bounded WAV/SFX playback, extensible PCM/float support,
Linux KO/rematch/KO and completed decoder/mixer/media race checks.

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
