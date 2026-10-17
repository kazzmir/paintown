# Current handoff

Updated2026-10-17. Implementation/local checkpoint commits are authorized.
Continue without routine review requests; never push. Full MUGEN1.1 beta1
engine/assets/modes remain unfinished. All authored changes stay in game/mugen/.

## Latest checkpoint: M4az RGB consumers

M4az is verified for its bounded scope; see git log for the local checkpoint
hash. Reversal:d572399f (M4ay). Read RGB-CONSUMERS.md for measured rules, rejected
hypotheses and open gates. HUD/effect true-color sprites use the shared sampler;
Linear.DrawImage preserves blend/tint/geometry options. Shared shader arithmetic
is byte-for-byte M4ay. Indexed consumers are unchanged. RGB cue X/Y truncate in
signed common-game units before stage/camera translation; fractional owner and
common960 controls match.

All16 numeric programs pass:192 observations/80 source hashes, no final rejected
programs. Ten2100-pixel consumer crops match exactly. Three AS192D64 cases each
fail12 pixels max1; half/independent destination cases each fail117 pixels max1.
These six alpha precision gates remain OPEN; native goldens and strict checks
are unchanged. All three video-fit HUD/world profiles now match bounds/core
hashes exactly, closing the pre-existing nearest-filter gap. Never waive hashes.

Five representative actor crops remain byte-exact; three fresh native replays
reproduce goldens, including two alpha controls. All25 app runs/28 captures pass
input/config/binary/capture/runtime provenance; see m4az-final-app-audit.json.
Binary7c40366876ea6cd4dee8a3b73eccd3581c8626b8fff4a017ef9751740f8f8394.
Viewed stock7s: two fighters, no lingering effects. Final consumer captures end
go-m4az-final; intermediate candidates are diagnostic. Full clean scoped races
PASS (session83481 exit0): Match489.826s, Character10.368s in m4az-full-race.log.
Pure races/root module checks pass. The first full invocation's temp-snapshot
mixed-package setup failure is retained separately; its actual package tests
passed, but its overall command failed. Diagnostic Go snapshots now use .go.txt;
AGENTS documents why. No GUI client is active after83481; check ps before next.

## Next M4ba: combat power

Continue immediately; no permission dependency. Runtime Get/GivePower consumers
are absent, Damage/Get/GivePower are literal-parsed, config .7/.6 rules do not
reach Match actors and helper/root pool sharing is unverified. Read
HITDEF-SCHEMA.md and temp/power-next-notes.txt. Do not infer numeric/default/order/
entry timing from the manual alone; record original native programs first.

Prepared44 root programs already exist untracked as hitpower-* CNS/CMD/AIR/stage
files, structurally audited in m4ba-prepared-inputs.json. No native power JSONs
exist yet. author-hit-power.py is the generator; do not rerun after recording.
hitpower-cases.json has decisive defaults/guard/explicit/computed/order/150/type
controls first. temp/run-hitpower.py is ready for sequential native recording.

temp/author-helper-power.py is prepared but UNRUN: eight observe/set/add/cap/
lower/root-add/Statedef-entry/SuperPause controls compare root/helper/parent
power and publication phases. temp/author-power-rules.py is also prepared but
UNRUN: six custom zero/fractional/sign/explicit-factor controls. Generate these
and merge case lists with root44 before native recording if useful (58 total).

tests/reference_probe.py has an UNSTAGED next-checkpoint --rule extension:
two finite power factors, prepared-config hash and strict replay recipe checks.
Pure preparation/invalid CLI tests pass; original reference config hash unchanged.
This tool change and all hitpower inputs belong to M4ba, outside M4az staging.
After measuring, add integrated numeric coverage and implement typed source
expressions/default evaluation order, contact transfer/timing and configured
rules/ownership. Zero factors are meaningful; use explicit presence, not a zero
sentinel. Broader pause/foreign/projectile/team/multiple-contact pools remain
separate gates. Preserve every existing native expectation.

## Scope and environment

Read ../AGENTS.md. All changes stay under game/mugen/. Preserve temp/mugen-1.1b1/
and root Go modules. Stage explicit authored files plus tracked MUGEN edits;
never stage temp, original assets/binaries, caches or captures. No agents, skill
or goal tool is active. Branch go-rewrite-mugen. Sandbox bwrap is broken; scoped
require_escalated calls have passed automatic review. No approval blocker exists.

Private display: DISPLAY=:97,
XAUTHORITY=/home/juvinious/projects/paintown/game/mugen/temp/virtual-display/Xauthority,
LIBGL_ALWAYS_SOFTWARE=1. Xvfb PID2146490. Private Pulse PID1393643:
PULSE_SERVER=unix:/home/juvinious/projects/paintown/game/mugen/temp/audio-reference/native;
PULSE_COOKIE points to that directory's cookie; ALSA_CONFIG_PATH to its alsa.conf.
Never print cookie contents, touch desktop/focus/keyboard or use host audio.

Native probes, actual apps/audio and full Ebiten suites run sequentially. Before
each new client, wait for the prior session then separately check:
ps -eo pid,stat,comm | rg 'mugen.exe|mugen-go|match.test|character.test'
Idle wineserver/winedevice is okay. Pure env -u DISPLAY tests can run alongside.

Full suite from workspace root with private display vars:
GOPROXY=off game/mugen/mugen --check -race
Redirect output to temp. Keep commentary updates during long tests. Scoped builds
use GOCACHE=.../game/mugen/temp/go-build, GOTMPDIR=.../game/mugen/temp/tmp,
GOFLAGS='-modfile=/home/juvinious/projects/paintown/game/mugen/mugen.mod -mod=readonly'.
Root module baseline: temp/root-module.before-font.sha256. Last free space~82G.

Historical notes are in archive/HANDOFF-2026-10-17-long.md; its active instructions
are obsolete. Prefer this handoff and current checkpoint documentation.
