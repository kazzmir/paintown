# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Local checkpoint commits are authorized without individual approval. Recap about
every five commits; ask only for needed input. Never track user configuration.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Current sprite/rendering batch (end-state validation pending):

- PNG32/raw RGBA now expose a lightweight unmasked view: force opacity after
  native byte-truncated RGB premultiplication. One beta scene confirms both
  formats, including opaque black for fully transparent texels. Indexed/opaque
  sprites retain their existing views; linked nodes share the opaque view.
- True-color backgrounds now use the shared linear sampler; indexed sprites
  retain nearest sampling. The connected stage scenario replays native RGBA
  mask/scale samples, then mask changes, visibility and background reset.
- Backgrounds, motifs and storyboards now realize texture/mask variants on
  demand and share uploads for identical immutable pixel views. Hidden/disabled
  layers do not upload frames. Replacing a background archive rebuilds its map.
- Parsed SFF metadata now has indexed group/item lookup. Actors, effects and
  normal/animated/parallax backgrounds use it; caller-built archives retain
  dynamic linear lookup. Missing effect pixels report a contextual warning.
- Latest accepted media checkpoint764918190 retains bounded WAV/SFX playback,
  extensible PCM/float support, Linux KO/rematch/KO and completed race checks.

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
