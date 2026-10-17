# Current handoff

Updated2026-10-17. Continuous implementation/local commits authorized. Do not
ask to continue, stop at a checkpoint, push, or claim the whole port complete.
All authored edits stay under game/mugen/. Read ../AGENTS.md.

## Latest checkpoint: M4ba damage/power

M4ba is verified and ready for its local checkpoint; see git log for its hash.
Reversal83eb5bd9 (M4az). Read HIT-POWER.md and HITDEF-SCHEMA.md. Sixty-six original
programs compare804 observations, three literal-float rejections and264 source
hashes. All66 integrated programs pass. Full scoped races PASS in
m4ba-full-race.log: Match538.364s, Character12.143s. Pipeline session88607 exited0;
no GUI client remains. Pure races and original/root module integrity checks pass.

Eight app runs/eleven captures pass input/config/binary/capture/runtime audit;
all seven clothing masks match exactly (six11,181 pixels, helper16,713).
Binary554c7ba209dd5fe6c8c08ccec8cbf2526429101ff2d697f3ec179679338f6a4e.
Four fresh native programs/six captures reproduce observations. Viewed native/Go
high-rule counters show3/3; stock7s shows two fighters without lingering effects.
Final captures end go-m4ba-final. No stock art/captures/binaries are committed.

Compiler order now follows the full27-component tags plus M4aw vector order:
id,hitonce,damage,getpower,givepower,numhits,hitsound,guardsound,priority,
p1stateno,p2stateno,p2getp1state,p1sprpriority,p2sprpriority,forcestand,forcenofall,
sparkno,guard.sparkno,sparkxy,p1facing,p1getp2facing,p2facing,then vectors.
Defaults retain raw source ASTs and reevaluate them; 2.9 deals2 but grants2/1,
tagged defaults execute Damage five times. Rules are optional so explicit0 is
meaningful. Production config passes factors into round actors/helpers. Helpers
read/write root pools. Transfer precedes entry even for overrideI without life
loss. AttackMul/NumHits do not scale power. Signed damage uses int(product+.5):
raw-10 heals9, guard-3 heals2, scaled-1.75 heals1, with bounded life.

M4az83eb5bd9's RGB consumer filtering/placement remains verified for its scope;
six alpha precision crops remain OPEN. Another observed asset gap is native
magenta uncovered stage fill for BGDef.debugbg=1 versus Go black; parser ignores
that key. Keep these documented and unwaived. Full engine/assets/modes remain
unfinished; config omission/extreme overflow/foreign/projectile/team/pause/
multiple-hit power combinations and remaining literal HitDef fields stay open.

## Next M4bb: lethal hits and fall damage

Continue immediately without permission. kill/guard.kill/fall.kill have no
fields/consumers; HitFallDamage just calls AddLife on fall.damage and ignores
fall eligibility/kill policy. GetHitVar(fall.kill) is documented by native but
missing here. SetHitFallDamage is an unused stub (not a proven controller path).
Read local sctrls.html HitDef/HitFallDamage and trigger.html GetHitVar.

Prepared temp/author-hit-kill.py is UNRUN. It would write38 original hitkill
programs for hit/guard flags, coercion/default/sign/bottom/redirect/type cases,
nonlethal entry-death, fall flags/damage/repetition/scaling/defence. It clones
power fixtures but snapshots GameTime145 to observe lethal contact before round
reset; intended capture4s. It appends simple180/5150 states to avoid missing win/
dead state paths. Use --clipboard-delay0 for these KO probes to avoid waiting
for Time30 after winner state changes. Begin with pilots hit-default/hit-zero/
fall-default and verify capture timing before recording the rest. Preserve the
existing66 power sources/JSONs. Add tagged kill/fall.damage order controls and
MoveTypeI/H/air contexts before implementing their consumers. These are plans,
not native findings. No M4bb source or native JSONs exist yet.

A low-impact diagnostic background fix is also independently identified:
stage/loader.go BGDef parses only spr; FightState fills stageCanvas black. A
future measured DebugBG field/fill can close magenta-vs-black exposed above.
Do not change the original reference distribution or relax raster/alpha gates.

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
GOPROXY=off game/mugen/mugen --check -race -timeout=20m
Redirect output to temp. Keep commentary updates during long tests. Scoped builds
use GOCACHE=.../game/mugen/temp/go-build, GOTMPDIR=.../game/mugen/temp/tmp,
GOFLAGS='-modfile=/home/juvinious/projects/paintown/game/mugen/mugen.mod -mod=readonly'.
Root module baseline: temp/root-module.before-font.sha256. Last free space~82G.

Historical notes are in archive/HANDOFF-2026-10-17-long.md; its active instructions
are obsolete. Prefer this handoff and current checkpoint documentation.
