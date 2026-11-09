# Current handoff

Follow AGENTS.md. PROGRESS.md is the single completion ledger. Continue engine
compatibility using supplied beta content; no further mod-script repairs.
Defects reproducing in native are content findings. Native AI policy and
arbitrary content compatibility remain partial, even when routes are present.

Latest fix: corrected the reported right-shifted storyboards. The viewport
change added a BG-style half-width to layers/windows, even though stock KFM
already authors layerall.pos=160,0. Layers/windows now use the fitted top-left;
BG centering, widescreen overflow, clipping and retained canvases are preserved.
Native canonical1280x720 ->640x480 and rebuilt Linux both center stock KFM art
at x320 (edge bounds140..500 versus141..499). Small raster differences remain
partial. Existing asset lifecycle now includes that actual stock intro; affected
match-flow passes with races, other segments stay checked. Linux rebuilt and
inspected; owned scratch removed. Root modules/local YAML remain unchanged.

Previous continuing batch committed coordinate/metadata/schedule fixes, idle
demo cycling, TrueType height overrides and configuration/runtime consumers.
All five segments remain checked. See PROGRESS.md and Git history for scope.

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
