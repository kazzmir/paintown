# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Local checkpoint commits are authorized without individual approval. Recap about
every five commits; ask only for needed input. Never track user configuration.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Current palette/AfterImage batch (validated):

- Palette state now distinguishes mapping changes from shader effect changes.
  Actor/effect textures track MappingRevision; PalFX/HitDef color effects keep
  their ordinary revision and clocks without forcing a base-image upload.
  Legacy initial-outfit portrait policy also invalidates mapping pixels.
- Fighter/HUD cache entries now share immutable mapped pixels. The connected
  outfit/reset scenario renders local PalFX beside an unaffected HUD sprite,
  then expires the effect without changing the mapping revision.
- AfterImage poses now save their AIR element tick, preserving interpolated
  offsets/scales/angles after the live animation changes. One beta flow has
  five retained marker runs at124/146/168/190/212; the connected stage scenario
  continues through Pause, drain/resume and a real round reset.
- AfterImage now snapshots mappings only and reuses them across local effect
  changes. A real RemapPal creates an independent historical outfit; pose
  offsets use fixed storage instead of allocating a slice per recorded frame.
  Existing recording coverage spans effect changes, expiry/resume, Pause and
  remapping without changing historical colors.
- Existing palette/cache regressions and all four changed-source segments pass
  with race detection; state-flow stays checked. Linux rebuilt and matches the
  five beta pose runs after an animation change. Owned clients exited; all
  current native/app captures, overlays, logs and helper scratch were removed.
  Local YAML and root module hashes are unchanged. This is bounded acceptance,
  not whole-renderer parity.
- Prior ca131d22c provides native RGBA mask/upload/filtering behavior, lazy shared
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
shadow/alpha, KO echo, native AI policy, strict asset compatibility, physical CD
and absent licensed fonts. Linux is the target; other platforms are deferred.
Historical matrices/raw renderer gates are not mandatory acceptance.

Local mugen.yaml remains ignored/untracked by 8eda46f84, with contents preserved.
The user will notify the contributor to back up YAML before pulling that removal.
Standalone music-file playback CLI remains deferred; in-stage music works.
Both upstream histories retain Go 1.26/Ebitengine 2.10.6/purego 0.11.3; root module
files stay read-only. Original go.sum backup remains at
`temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked`.
