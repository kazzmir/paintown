# Current handoff

Updated 2026-10-17. Continuous implementation and local commits are authorized.
Do not ask to continue, stop at a checkpoint, push, or call the whole port
complete. All authored changes stay under game/mugen/. Read ../AGENTS.md.

## Latest verified checkpoint: M4bb

M4bb is verified; reversal/predecessor is 3fa397a9 (M4ba). Read HIT-LETHALITY.md.
The accepted corpus is now 90 hitkill programs: 1,080 numeric observations,
four float-literal rejections and 360 CNS/AIR/CMD/stage hashes. Sources/goldens
are frozen; never rerun their authors. Final source hashes are audited against
the native JSON. Diagnostic killdiag sources moved to
`temp/m4bb-flow-diagnostic-sources/`; none belong in the final corpus.

Validation pipeline57076 completed exit0 in temp/m4bb-post-validation.log.
All phases pass: app audit, fresh native replays and full scoped races. Match
618.564s, Character11.012s, System5.027s in m4bb-full-race.log. Root module checks
also pass. No active GUI client remains.

Validation:
- All90 integrated race replays pass: m4bb-integrated.log, Match61.287s.
- Pure CNS/engine/combat/evaluator races pass: m4bb-precedence-pure.log.
- Thirteen app runs/sixteen captures share binary
  d354737083b645fc513029e69f441268af50869ed14c3dbed067b2d5048185bb.
  All provenance/runtime checks and all12 exact garment masks pass in
  m4bb-final-app-audit.json. The mask selects RGB247,239,231 in
  box160,180..1120,650, excluding grayscale round text. This is an actor check,
  never whole-frame parity. Stock throw7s was visually inspected: two recovered fighters, no lingering effects.
- Native airborne capture initially had a K.O. overlay hiding1,180 garment
  pixels. Retained as *-occluded diagnostics. A fresh unchanged source replay
  `m4bb-replay-ko-air-up-unoccluded` at8s verifies pinned numeric observations
  and matches all13,923 garment pixels exactly. No golden or tolerance changed.
- Root go.mod/go.sum and original mugen.exe checks pass; final check follows
  the full suite too. Native binary SHA remains
  d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e.

Native findings implemented:
- Effective damage caps before entry to remaining life, or life-1 for
  kill/guard.kill0. Victim5/raw10 exposes5(fatal) or4(nonlethal), entryLife5;
  raw-derived power remains7/6. Entry LifeSet0 stays dead.
- Fatal guard retains chip/guard.kill selection but normalizes reaction,
  counters, power and context to ordinary hit. Fall is forced. Normal fatal
  contacts get KO-adjusted velocities before CNS/HitVelSet.
- Ground zeroY scales X by float32.66; nonzero X then gains signed2.5.
  Airborne skips.66. NonpositiveY loses2 with ceiling-6 ground/-3 air;
  positiveY stays. Constants scale by receiver width/320, after source vector
  conversion. Downed contacts retain the supplied vector. Thirty-eight controls
  cover signs, thresholds, falling/nonlethal, S/C/A/L and mixed coordinates.
- Typed kill/guard.kill/fall.kill/fall.damage follow the measured full31-tag
  component order (36 typed keys total). GetHitVar(fall.kill) defaults1 and
  returns int. Getter fall is0 outside H.
- Native HitFallDamage consumes raw signed fall.damage once, without attack/
  defence scaling, even outside fall/H and with fall.kill0. Repeated6 only
  applies once; negative6 heals6. The controller does not itself force a fall.
- CNS ordinary parameters now keep the first duplicate; repeated triggers stay
  separate. Seven airborne/downed probes exposed the former last-wins bug.
  Three explicit duplicate damage/kill/PowerSet controls confirm the fix.
  Metadata/Statedef duplicate rules remain outside these controls.

Earlier failed focused logs are diagnostic evidence: m4bb-first-integrated
(typed fall.kill selector absent), m4bb-ko-integrated (seven first/last parameter
mismatches), and prior empty-state/unreached snapshot pilots. Current sources
include their fixes. Pilot capture errors never count as expected load rejection.
The accepted snapshot protocol is positiveState700 last, Time>=8 one-shotVar59,
clipboard there at delay0, FVar8=100000+Var29 sentinel. KO motion programs freeze
positions after the first three physics steps; phase timing remains independent.

Next: continue with stage diagnostic fill, then remaining combat/schema/KO
and renderer gates. M4bb contains only authored MUGEN source/fixtures/docs;
never stage temp, captures, original art or binaries. The final native/app
artifacts stay ignored under temp. Keep future checkpoints scoped and local.

## Remaining gates and next work

Full engine/assets/modes remain unfinished. Known unwaived gaps include M4az's
six RGB alpha precision crops, absolute KO slowdown/round transitions, complete
literal HitDef schema, recovery/juggle/projectile/reversal/team/foreign/multiple
contacts, and broader modes/assets/rendering. Do not claim whole-engine parity.

An independently confirmed low-impact stage gap is next: BGDef.debugbg1 fills
uncovered native stage pixels magenta; Go ignores the key and fills black.
Local docs/bgs.html explicitly states this behavior, and current authored
contact stages expose it. Stage loader only reads spr in BGDef; FightState fills
stageCanvas black. Measure on/off and viewport boundaries, implement the scoped
field/fill, and compare original captures. Do not relax existing raster gates.

Local fight.def also exposes Round slow.time=60 (alongside over.waittime45,
over.hittime10, over.wintime45, over.time210). Match currently has no KO slowdown.
Measure CNS/GameTime/phase boundaries deterministically before implementing it;
wall-clock screenshots alone do not establish the slowdown factor or consumers.

Previous verified checkpoint: M4ba3fa397a9, reversal83eb5bd9. HIT-POWER.md
records66 native programs,804 observations, three type rejections,264 hashes;
full races passed Match538.364s/Character12.143s. M4az83eb5bd9 closes bounded RGB
sampling/placement and retains six strict alpha failures. Older notes are in
archive/HANDOFF-2026-10-17-long.md; its active-task instructions are obsolete.

## Scope and environment

Branch go-rewrite-mugen. No agents, skills or goal tool active. Sandbox bwrap
startup is broken; scoped require_escalated calls have passed automatic review.
No approval blocker. Do not touch the host display/audio/keyboard/focus.

Private display DISPLAY=:97,
XAUTHORITY=/home/juvinious/projects/paintown/game/mugen/temp/virtual-display/Xauthority,
LIBGL_ALWAYS_SOFTWARE=1. XvfbPID2146490. Private PulsePID1393643:
PULSE_SERVER=unix:/home/juvinious/projects/paintown/game/mugen/temp/audio-reference/native;
PULSE_COOKIE points to that directory's cookie; ALSA_CONFIG_PATH to its alsa.conf.
Never print cookie contents. Native/app/audio/GUI tests run sequentially. Wait
for the prior session, then separately check
`ps -eo pid,stat,comm | rg 'mugen.exe|mugen-go|match.test|character.test'`.
Idle Wine service processes are fine.

Use scoped builds with GOCACHE=.../game/mugen/temp/go-build,
GOTMPDIR=.../game/mugen/temp/tmp,
GOFLAGS='-modfile=/home/juvinious/projects/paintown/game/mugen/mugen.mod -mod=readonly',
GOPROXY=off. Full suite uses private display/audio variables and
`game/mugen/mugen --check -race -timeout=20m`; growing Match corpus needs the
explicit timeout. Never save disposable Go snapshots with a .go extension under
temp; recursive tests discover them even when gitignored. Root baseline is
`temp/root-module.before-font.sha256`. Free space last checked75G.
