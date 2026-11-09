# Current handoff

Follow AGENTS.md. PROGRESS.md is the single completion ledger. Continue engine
state-machine work; the user will provide another mod for compatibility testing.
Do not repair more mod scripts. Failures that also reproduce in the original
beta are content findings. Keep the existing optional SF2 playable copy intact.

Latest engine checkpoints:

- Declared CMD labels now use their matched press/hold/release/buffer rules.
  Raw fallback cannot reactivate an inactive declared button while it stays
  held. The connected player/helper attack/recovery flow preserves explicit
  held input, charged release and buffer windows, then continues into stock AI.
- Air jumps now enter common state45 on a fresh up edge, with control, height
  and per-flight limits. CNS owns velocity/animation. Grounded S/C restores the
  budget before held guard can return early. The mixed 320p/1280p jump flow now
  spans denied low/held input, allowed directional air jump, exhausted budget,
  landing and a fresh flight, then body contact, Pause and round reset.
- Borrowed-state Explods latch their evaluation coordinate width through
  deferred admission. Geometry converts once into issuer storage units;
  private-sprite scale retains the creating state's profile after SelfState.
  Banks remain the issuer's. The connected entity flow covers contact, missing-
  action gating, creation/Pause, own-state recovery, expiry and fresh identities.
  Rebuilt Linux marker position and dimensions agree with the beta scene.
- Guard-distance behavior in the related native observation matched the current
  implementation. Preserve it rather than applying uniform controller scaling.

All five connected segments are checked with race detection. The latest four
changed-source segments passed; state-flow stayed checked. Linux is rebuilt.
Native and Linux keyboard flows each accepted two air jumps across two flights
and returned to idle. The latest held-button flows also agree: one attack while
held, then a second after release/repress. Current diagnostic scratch/captures/
overlays were removed.

The ledger remains 32/34 implemented items, with 89 controller and 127 trigger
routes. These are scoped functional checkmarks, not arbitrary native conformance
or an effort estimate. Broad font/media acceptance, native AI policy and
unverified compatibility/fidelity combinations remain partial. Physical CD audio,
absent licensed fonts and other platforms are deferred. Do not regenerate old
matrices or turn unverified permutations into independent implementation tasks.

Preserve earlier camera return, indexed/shadow/RGBA calibration, OnTop/HUD/global
palette order, consumed-vs-read-ahead audio cursors and bounded SFF/font loading.
Keep crossover turns before destination entry, current-frame direction rematch,
stable held guard and the Explod velocity alias. Details/checkmarks are in the
ledger and Git history. Paintown C++ turning is not a normative reference.

Author only game/mugen/. Local source/text commits are authorized; no pushes,
binaries, generated banks or user configuration in Git. Preserve ignored
mugen.yaml and unrelated/untracked work, including initial_placement_render_probe.py.
Retain canonical temp/mugen-1.1b1, original SF2 assets, temp/sf2_mugen_playable,
the shared build cache and one ignored temp/bin/mugen-go executable.

Use the readonly mugen.mod/mugen.sum overlay. This host uses
python3 game/mugen/temp/round-task-env.py with GOTOOLCHAIN=go1.26.9 and the
authorized elevated prefix. Native/app/Go runs stay serial: await exit, then a
separate owned-client guard before edits/new runs. Never reset shared servers.
Root module files and user YAML remain unchanged. Root go.sum backup:
temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked.
