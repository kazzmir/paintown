# Current handoff

Follow AGENTS.md. PROGRESS.md is the single completion ledger. Continue engine
compatibility using supplied beta content; no further mod-script repairs.
Defects reproducing in native are content findings. Native AI policy and
arbitrary content compatibility remain partial, even when routes are present.

Latest batch (three local checkpoints):

- Legacy and v2 font fields retain first values. Native loads duplicate bitmap
  metadata with later invalid fields; rebuilt Linux loads the same v2 font.
  The storyboard asset lifecycle spans bitmap/TrueType/legacy text, banks,
  primary-resource errors and corrected reloads without copied fixture banks.

- Stage resources/metadata and fight layouts keep first fields. BG/controller
  fields keep first values per instance; repeated BG names stay distinct.
  Native and Linux render the same combined duplicate-setting stage/HUD flow.
  Connected camera/linked-parallax/reset and round/combo/expiry flows pass.

- Motif/storyboard fields retain first declarations, including music/resources,
  scene/layer/sound settings and source-context errors for authored blank BG
  banks. The connected storyboard/pack lifecycle passes with races; native and
  Linux complete the same intro-to-title flow.

All five segments are checked with races; Linux is rebuilt. Prior scratch was
removed. One immediate follow-up remains in the checklist: intro presentation
inside a widescreen motif. The current disposable scene-compat overlay/captures
remain only for that follow-up and must be removed when it finishes.

Preserve the implementation invariants in AGENTS.md, especially:

- Borrowed helper admission converts position once, retains helper-unit size
  overrides and issuer banks/profile/programs through parent SelfState.
- No-op gates differ: native DestroySelf still evaluates parameters on roots.
- Declared commands, held chords/guard, fresh-edge air jumps, crossover facing,
  hit-paused bindings, combat stat scaling and borrowed Explod ownership.
- Resource/palette/font bounds, native raster calibration and audio cursors.

Remaining: broader font/media/pack acceptance, native AI policy and unverified
rules/fidelity combinations. Physical CD, absent licensed/CJK fonts and other
platforms are deferred; Linux is the selected target. Do not regenerate matrices
or call unknown combinations new implementation gaps without evidence.

Author only game/mugen/. Local source/text commits are authorized; no pushes,
binaries, generated banks or user configuration in Git. Keep ignored mugen.yaml
and unrelated work, including tests/initial_placement_render_probe.py, untouched.
Retain canonical temp/mugen-1.1b1, original SF2 pack/ZIP/executable,
temp/sf2_mugen_playable, shared cache and one ignored temp/bin/mugen-go binary.

Use the readonly mugen.mod/mugen.sum overlay with GOTOOLCHAIN=go1.26.9 through
python3 game/mugen/temp/round-task-env.py. Native/app/Go runs stay serial: await
exit, then a separate owned-client guard before edits/new runs. Never reset
shared display/audio/Wine servers or expose cookies/unrelated process args.
Root modules/user YAML remain unchanged. Root go.sum backup:
temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked.
