# Current handoff

Follow AGENTS.md. PROGRESS.md is the single completion ledger. Continue engine
compatibility work using supplied beta content and packs. Do not repair more
mod scripts. Failures that also reproduce in native are content findings.
Keep the original packs and existing optional SF2 playable copy intact.

Latest compatibility checkpoints:

- Variable register/range destinations admit value evaluation. Invalid IDs,
  invalid ranges and reversed ranges skip the value and continue. Valid ranges
  evaluate once and use ordinary-bottom zero fallback; genuine admitted VM
  errors stay fatal. The connected runtime flow and rebuilt Linux match twelve
  native observations; all five segments pass with races. Scratch removed.

- AI plans use timed input steps, including long charges, without allocating
  one input array per tick. The connected command flow now completes a remapped
  300-tick AI charge, attack/damage/recovery and a fresh stock AI fight. Four
  affected segments pass with races; state-flow stays checked. Linux rebuilt.
  This covers authored command availability, not native AI decision parity.

- Command history reserves the loaded CMD motion, charge and buffer window
  before input is recorded. Keep sizing overflow-safe and allocation lazy;
  player/helper buffers remain independent and fresh matches discard history.
  The connected human/helper/recovery/AI flow covers long and short charges,
  expired/delayed motions and fresh presses. Native/Linux counts agree; four
  affected segments pass with races, state-flow stays checked.

- Helper-bank capacity is checked before any Helper parameter expression.
  CreateHelperEvaluated preserves this gate; direct creation retains its own
  capacity check. Borrowed contexts must forward CreateHelperEvaluated through
  the state adapter: convert positions once after admission, retain helper-unit
  size overrides and issuer banks/profile/programs. Native/Linux mixed-profile
  context observations agree through parent SelfState and helper retirement.
  The entity flow covers full-bank rejection, skipped failures,
  retirement/vacancy reuse, parent confirmation and round reset. All five
  segments pass with races; rebuilt Linux matches twelve native slots.

- Target controllers admit expressions only after target existence and ID
  selection. TargetBind checks ID before time/position. ParentVarSet/Add skip
  their expressions without a live parent. Keep compilation and admitted VM
  failures strict. The same entity lifecycle now covers grab/drop, skipped
  effects/errors, actual helper-parent writes, retirement and recovery. All
  five segments pass with races; rebuilt Linux matches twelve native slots.

- Config.Language now reaches all definition asset consumers. Localized groups
  replace whole sections; missing sections fall back to English. Keep source
  lines, scene order, command/state/BG exemptions and runtime quote banks.
  Standalone AIR stays undecorated; localized Files can choose another bank.
  Motif-local select.def resolves before shared data. The connected asset
  lifecycle and all five segments pass with races; rebuilt Linux matches the
  twelve native observation slots. Font glyph availability remains partial.

- Character DEF and motif Info/Files use first values, including blanks and
  original source lines. Ignored bad duplicates cannot replace sprites, fonts,
  coordinate profiles or legacy palette policy. The existing asset/ZIP/menu/
  fight/reload lifecycle passes; rebuilt Linux agrees with native loading and
  metadata. Other completed segments stayed checked.
- Hold-only commands ignore buffer.time; /x+y holds the entire chord. Held
  sequence endings complete on their down edge and use the ordinary buffer.
  Simultaneous-press chords keep per-button edges. The connected human/helper/
  recovery/AI scenario passes; rebuilt Linux matches twelve native keyboard
  observations. Four changed-source segments pass; state-flow stays checked.
- AIR lookup retains the first duplicate action while validating later blocks.
  Invalid ChangeAnim element selectors restart at element1; ordinary bottom
  animation arguments use zero fallback. The connected combat/runtime flow and
  rebuilt Linux match all twelve native observations, then recover control.
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
