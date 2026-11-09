# Current handoff

Follow AGENTS.md. PROGRESS.md is the single completion ledger. Continue engine
compatibility work using supplied beta content and packs. Do not repair more
mod scripts. Failures that also reproduce in native are content findings.
Keep the original packs and existing optional SF2 playable copy intact.

Latest compatibility checkpoints:

- Ordinary invalid numeric results now use beta zero fallback in the confirmed
  movement, life/control/power and state-transition paths, plus entry ctrl and
  velset. Omitted attributes retain their defaults; later arguments still run.
  Genuine VM errors remain terminal with source context. The existing combat
  scenario spans movement/life/power, entry, SelfState recovery and fatal vector
  diagnostics. Rebuilt Linux matches all twelve native observations.
- InGuardDist includes opposing helpers and live projectiles independently of
  their owner’s move type. Projectile defaults use size.proj.attack.dist in the
  projectile’s coordinate profile; explicit guard.dist wins. Retiring attacks
  stop contributing even while removal artwork remains. Native range flows and
  Linux keyboard blocking agree: 5 chip damage, guard event, no hit.
- Base attack/defence reach contact damage. Negative nonabsolute TargetLifeAdd
  and LifeAdd share base/runtime/SuperPause defence reductions; healing and
  absolute additions stay unscaled. Native TargetLifeAdd ignores issuer attack
  despite its manual. The connected mixed-coordinate flow retains the twelve
  native observations, recovery and fresh-round reset.

Preserve earlier declared CMD press/hold/release windows, fresh-edge air jumps
and per-flight budgets, borrowed-state Explod geometry/private-sprite scale,
crossover facing and stable held guard. Paintown C++ turning is not normative.
Preserve camera/sprite/shadow/alpha calibration, OnTop/HUD/global palette order,
consumed-vs-read-ahead audio cursors and bounded SFF/font loading. Details are in
AGENTS.md, the ledger and Git history.

All five connected segments are checked with race detection after the runtime
batch. Linux is rebuilt; owned-window observations agree with native. Shared
assets were reused; current diagnostic scratch/captures/overlays were removed.
The ledger remains 32/34 implemented items, 89 controller and 127 trigger routes.
These are scoped checkmarks, not arbitrary native conformance or an effort
estimate. Broad font/media acceptance, native AI policy and unverified rules/
fidelity combinations remain partial. Physical CD audio, absent licensed fonts
and other platforms are deferred. Do not regenerate historical matrices.

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
