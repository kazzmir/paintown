# Current handoff

Follow AGENTS.md. PROGRESS.md is the single completion ledger. Continue engine
compatibility using supplied beta content; no further mod-script repairs.
Defects reproducing in native are content findings. Native AI policy and
arbitrary content compatibility remain partial, even when routes are present.

Latest continuing batch: ten functional/acceptance commits before this handoff.

- Mixed-coordinate recovery constants, first character story/portrait fields,
  confirmed roster metadata and localized Arcade limits are corrected.
- Idle title demo now runs selection/VS previews, AI combat, end-time/intro
  cycling and input interruption with real music/bar/debug consumers. Title BG
  controllers advance. Native and rebuilt Linux show AI combat without bars
  and return to title on Enter.
- TrueType fontN.height reaches motif/story/HUD rasterization; bitmap cells
  remain unchanged. Legacy CFG fields retain first values before YAML overrides.
- DrawShadows, Debug.StartStage and Misc background simulation/audio policies
  have runtime consumers. Linux quick match without --stage uses stage0-720.

All five segments are checked with races; only changed source was rerun after
coherent batches. Linux is rebuilt. Root modules and local YAML are unchanged;
owned overlays, captures and drivers were removed.

Newly recorded concrete gap: input/manager.go enumerates gamepad IDs but never
consumes them; legacy joystick settings/YAML joy are not connected. Added a
separate partial project row (32/35), rather than hiding this inside unknown
native fidelity. Asked the user for controller models/connection types for the
next Linux adapter/verification pass; answer pending at handoff. No physical
controller support is claimed. Do not infer native raw joystick code mappings
or declare hardware acceptance from keyboard/fake-device tests alone.

Recaps are progress updates, not stop points (user 2026-11-09). Continue larger
batches without per-commit approval, stopping only for actual completion/input.

Preserve the implementation invariants in AGENTS.md, especially:

- Borrowed helper admission converts position once, retains helper-unit size
  overrides and issuer banks/profile/programs through parent SelfState.
- No-op gates differ: native DestroySelf still evaluates parameters on roots.
- Declared commands, held chords/guard, fresh-edge air jumps, crossover facing,
  hit-paused bindings, combat stat scaling and borrowed Explod ownership.
- Resource/palette/font bounds, native raster calibration and audio cursors.

Remaining: gamepad implementation, broader font/media/pack acceptance, native AI policy and unverified
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
