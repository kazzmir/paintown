# Current handoff

Follow AGENTS.md. PROGRESS.md is the single completion ledger. Author only
game/mugen/; local source/text commits are authorized. No pushes, binaries,
user configuration, subagents or goals. Preserve unrelated work, especially
tests/initial_placement_render_probe.py. The user requested the next ten local
checkpoints on 2026-11-10; this batch is implemented and accepted.

The ten checkpoints close unused storyboard admission, victory portrait windows,
result font RGB, inset quote wrapping, victory fades, menu/VS font RGB, HUD RGB
and optional tuples, configured result screens, continuing result poses, and
automatic campaign stage assignment. Stock beta fight.def documents neutral RGB
as 256,256,256. Keep bank selection and shared glyph pixels independent of tint.
Campaigns no longer require a manual stage pool when opponents have assigned
stages; missing random-stage pools produce contextual errors. Versus/Watch/
Training retain manual stage selection. Character and chained motif storyboards
resolve when played, including case-folded Windows references.

All five connected segments are checked with races; Linux rebuilt. The existing
campaign episode covers continue, localized victory, ending and no-ending Win
Screen branches, continuing poses with stable score/round/timer, and Survival
results/fade-to-title. Existing co-op coverage uses automatic stage assignment.
Actual HD Arcade selection, assigned Guile stage, round two and the victory
portrait/orange wrapped quote were inspected. This UI review shortened the VS
hold and used debug time-over results; natural KO acceptance remains the earlier
full-life two-round Ryu/Dhalsim run. No new fixture banks or test files; owned
review drivers/overlays/captures removed. Canonical packs/YAML are untouched.

Options remains readable and usable in artwork-only motifs without menu fonts.
Enter adjusts values/opens actions; explicit Back/Escape return to title. F1
opens session keyboard/gamepad configuration. Never write those edits to YAML.
Ctrl-Alt-1..4 activation survives F4/reload while preserving identity/AI ownership.
Native PNG screenshots date to 1.0 RC4; do not reopen the obsolete PCX claim.

HD Remix's executable reports 1.1 Beta 1 P1; it is not byte-identical to the
supplied canonical beta. SHA-256 identities are recorded in README. The original
SF2 executable is byte-identical to the canonical beta. Prior HD-native findings
are pack-runtime observations; verify stock-beta claims with the canonical
executable rather than assuming a matching version banner proves equivalence.
HD Remix targets 1280x720. All 19 characters and 3,118
state programs load. Preserve legacy parenthesized projectile-ID comparisons,
including != and unfiltered Var(...) IDs. Empty effect clocks accept zero-time
placeholders/tails and retire on their first pass; fighter-loop errors stay
strict. Keep authored portraits/blank cells, SFF 2.01 alpha, sparse HUD backgrounds,
sub/AI AIR flags and EOF loopend=-1. Dhalsim references absent palettes 1,21/1,22;
no palette repair is warranted. Other missing-action/AnimElem warnings remain.
Native Ken's unexplained SFF failure under Wine is not a corrupt-file verdict.

Remaining scope: 89 controller/127 trigger routes, 32/35 implemented project rows.
Three partial rows cover broader content/font/media acceptance, native runtime/
render/audio/AI/menu fidelity and physical gamepad/native joystick acceptance.
Concrete next result consumers: wintext/winstext.layerno and Survival roundstowin
pose policy. Broader complete-roster gameplay and exact native timing remain
unverified. The controller question is already pending; no physical device is
attached. Physical CD, absent licensed/CJK fonts, other platforms and standalone
music-file CLI playback remain deferred. Do not invent parameter matrices.

Do not repair further mod scripts. Original SF2 later-round 5900 stalls reproduce
in native; retain original/optional playable packs and the existing tool. Keep
the corrected storyboard layerall.pos origin, borrowed context, round identities
and pre-5900 restart/reload carry snapshots. See the ledger/catalog/Git history.

Use readonly mugen.mod/mugen.sum and GOTOOLCHAIN=go1.26.9 through
python3 game/mugen/temp/round-task-env.py. Runtime clients stay serial; await exit
and run the separate owned-client guard before edits/new runs. Also guard the
HD executable's StreetFighterHDM process name. Never reset shared servers or
expose cookies/unrelated process arguments. Verify root modules with
temp/upstream-post-merge-module.sha256. Retain canonical packs, Wine/toolchain,
shared cache and the ignored temp/bin/mugen-go build; clean owned scratch.
