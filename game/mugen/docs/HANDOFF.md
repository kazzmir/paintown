# Current handoff

Follow AGENTS.md. PROGRESS.md is the single completion ledger. Author only
game/mugen/; local source/text commits are authorized, no pushes/binaries/user
configuration. Recap about every 10–15 commits (user 2026-11-09), not after two.

New supplied pack: temp/ssf2-turbo-hd (native 1.1 Beta 1 P1, 1280x720).
Engine now accepts sub AIR, unavailable roster cells, requested selection
portrait sprites, EOF music loopend=-1, F-prefixed Data spark zero coercion,
padded scalar projshadow and SFF 2.01 palette alpha through links/remaps.
All five connected segments pass with races. Go Ryu mirror combat/audio and
native selection/idle combat inspected; pack files/YAML remain untouched.
Final visual pass still needs numbered HUD background components and the
native first scalar in p2.front.anim=14,0. Complete roster acceptance remains
unverified. One native Ken load failed at SFF; embedded PNGs validate, so do
not label it corrupt or repair content without a specific cause.

Latest continuing batch connects documented fight life/power/stand/time-over,
AI ownership, current-round restart/reload, pacing/vsync, bar visibility,
collision/target/protection diagnostics and requested PNG capture. Restart
snapshots precede state5900; reload retains score/carry/round counters and current
process time, while creating fresh resource/presentation banks. Campaign
templates and archive ownership stay intact.

Options now has session keyboard/gamepad configuration: enable, deadzone,
replace/add/disable, assigned device/layout and standard/raw capture. Preserve
release admission, omitted-key fallbacks, neutral raw-axis baselines and hotplug
rearming. Edits never write mugen.yaml. HD Options/title/bitmap captions now fit
motif dimensions. Direct --mode supports versus/watch/training/coop; --p3/--p4
use normal simul rosters, with P3 as human 2 in co-op.

All five connected segments pass with race detection. Existing full menu/
gameplay, team/co-op and combat/debug episodes were extended, without fixture
banks or per-key tests. Linux rebuilt; Options/input panes, direct mixed-KFM
Training, collision overlay and actual F12 frames inspected. Review scratch is
removed; keep only the ignored temp/bin/mugen-go binary/shared cache. Root module
files and local YAML remain unchanged. See README for controls and Git history
for the checkpoints.

The ledger remains 89 controllers/127 triggers and 32/35 implemented project
rows. Partial: physical gamepad/native joystick numbering, broader assets/fonts/
media and native runtime/render/audio/AI fidelity. Controller model/connection
question remains pending; no physical joystick is attached here. Do not repeat
it or infer hardware acceptance from injected sources. Native dummy/AI/cheat
policy and exact menu parity remain unverified. Ctrl-Alt player enable/disable
and native PCX output remain unimplemented; F12 intentionally writes PNG.
Physical CD, absent licensed/CJK fonts and other platforms remain deferred.

No further SF2 mod-script repairs. Original later-round stalls reproduce in
native; retain original/optional playable packs and their existing tool. A prior
native menu review stayed at the beta welcome overlay despite synthetic F1;
do not repeat that loop or build a matrix to bypass it. Keep the corrected
storyboard fitted top-left layer/window origin: layerall.pos already centers
KFM; BG projection has a separate center.

Preserve unrelated work, including tests/initial_placement_render_probe.py.
Use the readonly mugen.mod/mugen.sum overlay with GOTOOLCHAIN=go1.26.9 through
python3 game/mugen/temp/round-task-env.py. Runtime clients stay serial; await exit
and run the separate owned-client guard before edits/new runs. Never reset
shared servers or expose cookies/unrelated process arguments. Verify root
modules with temp/upstream-post-merge-module.sha256. Root go.sum backup remains
temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked.
