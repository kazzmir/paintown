# Current handoff

Updated 2026-10-17. Implementation and local checkpoint commits are authorized.
The user wants continued iteration without routine review requests. Do not push.
Full MUGEN 1.1 beta 1 compatibility is unfinished; do not stop at a checkpoint
or claim completion. The stock one-on-one flow is connected, but complete M4/M5/
asset/render/mode gates remain open in COMPATIBILITY.md and PLAN.md.

## Scope and instructions

All project changes stay under game/mugen/. Read ../AGENTS.md. Preserve the
original temp/mugen-1.1b1/ distribution and root Go modules. Never commit its
binaries/assets, caches or captures. Stage explicit authored files plus tracked
MUGEN edits. Local commits are authorized; no push is authorized. No sub-agents
are authorized by the applicable instructions. No skill or goal tool is active.

## M4ax ready for local physics checkpoint commit

Head/reversal is7af38317 (M4aw). M4ax code is UNCOMMITTED but verified for its
measured physics. All51 cornerpush programs pass in m4ax-common-push.log:
1,224 numbers, zero rejections, 204 authored hashes. Initial corrected22 fails17
in m4ax-before-push.log. Pure races pass; full scoped races pass in
m4ax-full-race.log (Match439.097 seconds). Six fresh native replays pass in
m4ax-replays.log. Ten app captures complete in m4ax-apps.log, with source/binary/
config/PNG/dimension/runtime checks passing and healthy stock recovery.

IMPORTANT: m4ax-app-checks.py's mask gate FAILS. Default/large/hitflip-left/
PosFreeze masks differ by567/471/933/567 pixels; proximity-equal is exact0.
Counts and source provenance are in m4ax-app-checks.json/log. Large's isolated
P1 white masks have the same bounds but different fractional sampling (8214 vs
8337 pixels; integer translation does not make them equal). Numeric positions
match native exactly. Do not loosen the checker or alter correct physics.
Documented in CORNER-PUSH.md. Commit this bounded physical checkpoint locally,
then immediately investigate the renderer gap (M4ay); power work is deferred.
No GUI/audio client remains active.

Current implementation: character/corner_push.go plus contact/physics hooks and
shared screenPositionBounds. Queue original attack field by receiver context.
World impulse uses physical attacker facing at contact, float32 common-space
initialization, double storage/position accumulation and float32(.7) promoted to
double for decay. After movement abs(common velocity)<1 stops it. Contact needs
abs distance to either effective bound<5 common pixels, independent of receiver
facing and knockback speed/direction. It survives state/target/turn changes,
adds to own velocity, runs through PosFreeze, freezes during own hitpause and
is suppressed when receiver ScreenBound is disabled. Projectile-owner push is
still excluded/missing; broad helper/foreign/bind/multi-hit/zoom/camera cases open.

Native logs: m4ax-controlled-native22, math-native16, integration-native13. First
m4ax-native-first copies sampled pre-round due to author clock; final22 were
rerecorded native-controlled with StateNo700/Time>=4/Var31=0/TeamSide1 seed.
All final JSON hashes match corrected sources. Old native-final copies are
diagnostic only. Current binary m4ax-common-fields.test expects51.
Read-only disassembly under temp corroborates strict proximity5 (0x4342f0),
physical-facing initialization (0x43ec5d), double velocity times float32 .7
(0x43cc55) and strict cutoff1 (0x43cc8a). No executable was modified.

## Immediate next task: fractional sprite renderer

Keep the four failing masks as evidence. Existing 28 flipmark controls from
M4aq pass the old floored-axis renderer; do not discard their expectations.
character/player.go:146 floors game-space draw axes. actor_order.go projects
positions; final world viewport fit is another transform. KFM real sprites now
expose a gap at fractional CPP final positions, while stationary roots still
match. Cause is not established. Separate geometry/flooring, filtering and
physics-specific effects with original multicolor marker controls and equivalent
stationary PosSet positions. Inspect SPRITE-ALIGNMENT.md and
sprite_render_probe.py. Existing marker .extra/probe.sff is original, permitted
for reuse; never commit stock art or native framebuffer crops.

Potential first control: clone cornerpush-ground-decay-large, remove HitDef,
place P1 statically at observed -37.534153 with priority1, P2at145, same AIR and
profile. Compare native/Go against the existing moved case to isolate motion
from general fractional rendering. Then use original solid/checker markers to
separate internal texture filtering from axis placement. Do not assume removing
math.Floor is correct: flipmark quarter/eighth/fractional-helper and clipping
controls discriminate some of that ordering. Need fresh reference evidence and
actual app checks for any renderer change, then appropriate full regressions.

Prepared but NOT RUN: temp/author-hit-power.py would generate36 hitpower
programs for next combat work. It uses one compact fvar page, root power seeds,
normal/guard/default/single/sign/cap/bottom/type/redirect/order, damage contexts,
overrides and first-contact power timing. Do not generate until renderer work
is settled/committed. Power get/give are literal-parsed with no runtime consumer;
config .7/.6 rules exist but Match does not pass them to actors; helper/root
power sharing is unverified. Continue autonomously; never push.

## Recent completed checkpoints

- 7af38317 M4aw: 63 velocity/default/down-offset programs, 1,020 numbers,
  252 hashes, no final rejects. Full races m4aw-full-race.log (Match395.933s),
  six fresh replays and ten apps pass; five masks exact. HIT-VELOCITY.md.
- 31795b0c M4av: 48 contact-facing/entry programs, 552 numbers, two rejects,
  192 hashes. Full races m4av-full-race.log (Match349.165s), six fresh replays,
  ten apps and six masks pass. CONTACT-FACING.md.
- dc90bb1e M4au: forced standing, dynamic ground velocity, world/facing
  HitVelSet. FORCED-STANDING.md; older history in CHECKPOINTS.md.

Historical working notes (including obsolete next actions) are preserved in
[the archive](archive/HANDOFF-2026-10-17-long.md). Prefer this short current handoff.

## Private environment and validation

Workspace /home/juvinious/projects/paintown, branch go-rewrite-mugen. Sandbox
bwrap is broken; scoped exec calls have used require_escalated with automatic
review. No outstanding approval rejection/blocker exists. Writable roots are
this workspace and /tmp. No network/dependency changes are needed currently.

Private display: DISPLAY=:97, XAUTHORITY=/home/juvinious/projects/paintown/game/
mugen/temp/virtual-display/Xauthority, LIBGL_ALWAYS_SOFTWARE=1. Original task
Xvfb PID2146490. Private Pulse PID1393643: PULSE_SERVER=unix:/home/juvinious/
projects/paintown/game/mugen/temp/audio-reference/native, PULSE_COOKIE points to
that directory's cookie, ALSA_CONFIG_PATH points to its alsa.conf. Never print
the cookie, touch desktop/focus/keyboard or route audio to host hardware.

Native probes, full Ebiten suites and app/audio clients run sequentially. Before
each new client, wait for the prior session to exit, then separately check:
ps -eo pid,stat,comm | rg 'mugen.exe|mugen-go|match.test|character.test'
Idle wineserver/winedevice is okay. Pure env -u DISPLAY tests may run alongside.

Full suite from workspace root:
DISPLAY=:97 XAUTHORITY=/home/juvinious/projects/paintown/game/mugen/temp/virtual-display/Xauthority LIBGL_ALWAYS_SOFTWARE=1 GOPROXY=off game/mugen/mugen --check -race
Redirect logs to temp. Match now takes several minutes; communicate progress.

Scoped builds/pure tests use GOCACHE=.../game/mugen/temp/go-build,
GOTMPDIR=.../game/mugen/temp/tmp,
GOFLAGS='-modfile=/home/juvinious/projects/paintown/game/mugen/mugen.mod -mod=readonly',
GOPROXY=off. go test -c without DISPLAY builds native comparison binaries; run
those in the package directory with the private display. Never overlap clients.

Root-module integrity baseline is temp/root-module.before-font.sha256; its
checks pass. Temp contains disposable copies, authors/runners, binaries, caches,
images, PCM and disassembly; do not stage it. Last free space about89G. Preserve
user data and the reference. Do not broadly stage unrelated untracked work.
