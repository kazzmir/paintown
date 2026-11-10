# Current handoff

## Continuing batch (user 2026-11-10): ten checkpoints

- Victory quote wrapping within its authored anchor: implemented; acceptance pending.

- Authored result and campaign font colors: implemented; acceptance pending.

- Victory portrait window in motif coordinates: implemented; acceptance pending.

- Lazy character and campaign storyboard admission: implemented; acceptance pending.

Validate the affected connected segments once after the implementation batch;
unchecked batch entries are implemented but await that acceptance.


Follow AGENTS.md. PROGRESS.md is the single completion ledger. Author only
game/mugen/; autonomous local source/text commits are authorized, no pushes,
binaries, user configuration, subagents or goals. Recaps about every 10–15
commits are progress updates, not automatic stopping points.

User priority (2026-11-10): close explicit omissions, then content compatibility;
also repair the reported Options page. The identified debug-player omission is
closed, and the PCX screenshot claim was corrected: native uses PNG since 1.0
RC4. Ctrl-Alt-1..4 toggle root activation across F4/reload while preserving
identity/AI ownership. Existing connected lifecycle and native queries agree.

Options is fixed for artwork-only title motifs without menu fonts: a fitted
built-in panel retains authored background/heading, displays live values and
offers an explicit Back action. Enter adjusts a value or opens an action;
F1 opens session keyboard/gamepad configuration. Actual HD edits and return to
title were inspected. Changes never write mugen.yaml.

Latest compatibility checkpoint: ssf2-turbo-hd bundles native 1.1 Beta 1 P1 and
targets 1280x720. All 19 characters and 3,118 state programs load. Parenthesized
projectile IDs retain legacy boolean/time comparisons, including != and native
unfiltered Var(...) IDs. Empty Explod/spark clocks accept zero-duration actions
and retire on their first pass; ordinary fighter-loop errors remain strict.
Connected guard/contact/recovery and pause/effect/expiry/reset coverage passes.
All five segments are checked with races; Linux rebuilt. Ryu/Dhalsim completes
a full-life two-round AI match with natural KOs, score 0-2 at tick 7497; a live
rendered Watch fight also passes the previously fatal effect. Prior Ryu/Ken
rendering, input and stage audio checks remain valid. Complete roster gameplay
and arbitrary native parity remain unverified. Dhalsim's scripts reference
palette 1,22, absent from its SFF along with 1,21; no engine palette repair is
warranted. Other missing-action/AnimElem warnings remain; do not silently repair
pack scripts. Source assets/YAML untouched; owned diagnostic scratch removed.

HD admission also retains authored portraits/blank roster cells, SFF 2.01 palette
alpha, EOF loopend=-1, sub/AI AIR flags, padded projshadow, native loose CNS
preamble labels, sparse HUD backgrounds and first scalar animation IDs.
Native Ken failed at SFF under Wine without a specific cause; its PNGs validate
and Go loads/renders him. Do not infer corrupt content from that observation.

Ledger: 89 controllers/127 triggers; 32/35 implemented project rows. Remaining
partial scope: broader content/font/media acceptance, native runtime/render/
audio/AI/menu fidelity, and physical gamepad/native joystick-number acceptance.
The controller model/connection question is already pending; no physical device
is attached here. Do not repeat it or claim injected input establishes hardware
acceptance. Physical CD, absent licensed/CJK fonts, other platforms and standalone
music-file CLI playback remain deferred. Continue from concrete content failures
or unchecked scope; do not invent parameter matrices to fill unknown permutations.

No further SF2 mod repairs. Original SF2 later-round 5900 stalls reproduce in
native; retain the original/optional playable packs and existing repair tool.
Keep the corrected storyboard fitted top-left layer/window origin: layerall.pos
already centers KFM, while BG projection has its separate center. Restart/reload
snapshots precede 5900; preserve score/carry/current input and resource ownership.

Preserve unrelated work, including tests/initial_placement_render_probe.py.
Use readonly mugen.mod/mugen.sum and GOTOOLCHAIN=go1.26.9 through
python3 game/mugen/temp/round-task-env.py. Runtime clients stay serial; await exit
and run the separate owned-client guard before edits/new runs. Also guard the
HD executable's StreetFighterHDM process name. Never reset shared servers or
expose cookies/unrelated process arguments. Verify root modules with
temp/upstream-post-merge-module.sha256. Keep canonical packs, Wine/toolchain and
shared cache; only temp/bin/mugen-go is the retained ignored build.
