# Current handoff

Follow AGENTS.md. PROGRESS.md is the single completion ledger. Continue engine
compatibility using supplied beta content; no further mod-script repairs.
Defects reproducing in native are content findings. Native AI policy and
arbitrary content compatibility remain partial, even when routes are present.

Latest batch (six local checkpoints):

- AI command plans use timed steps. A remapped 300-tick charge now completes
  through CMD, attack/contact/damage/recovery, followed by fresh stock AI combat.
- Variable writes admit IDs/ranges before evaluating values. Invalid/reversed
  destinations skip side effects/errors; valid ranges evaluate once and use
  ordinary-bottom zero fallback. Real admitted VM errors stay fatal.
- Pause/SuperPause use zero for ordinary bottom, preserve omitted defaults and
  later arguments, and retain zero-duration power updates and normal clocks.
- CMD defaults are global. First settings/remaps/attributes win; buffers are
  1–30 ticks. Full characters need a nonempty command set with resource context.
- Statedef/persistent settings keep first values while repeated triggers remain
  separate conditions. Entry/control/motion/power and recovery match native.
- Scalar controls/effects/stats accept ordinary-bottom fallback. Nonzero negative
  scaled LifeAdd/TargetLifeAdd have a one-point minimum; explicit zero stays
  zero and a zero attack still makes zero-damage contact. Both actors recover
  and effect art retires in the connected combat flow.

All five connected segments pass with race detection against current source.
Linux is rebuilt. Bounded native comparisons agree for variables, pause clocks,
command windows (5/30/1), state entry/persistence and scalar/life adjustments; AI command availability is
covered by the connected scenario, not a claim of native AI decision parity.
Current diagnostic scratch/captures were inspected and removed. The progress
page now lists short completion checkmarks; detailed scope/evidence is in its
feature catalog and Git history. No new test banks or per-parameter documents.

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
