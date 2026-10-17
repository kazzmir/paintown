# Current handoff

Updated 2026-10-17. **Pause after committing M4bd and the progress assessment.**
The user's latest instruction requests a review of progress and remaining work
before more token spending. It supersedes continuous iteration and any historical
"next" instructions below. Do not start another implementation batch until the
user explicitly resumes. Local commits were authorized; never push. All authored
changes stay under `game/mugen/`. Read [../AGENTS.md](../AGENTS.md) and
[PROGRESS-ASSESSMENT.md](PROGRESS-ASSESSMENT.md).

## M4bd checkpoint: cue/pause evidence, with open raster failures

Predecessor/reversal: `54d8c86e`. M4bd changes only authored fixtures, tests and
docs; there is no production behavior change. Read FATAL-GUARD-CUES.md.
Twelve native programs (four each audio/spark/pause) record 144 numeric
observations and 40 source hashes. Fatal guard matches ordinary fatal hit for
sound, spark selection and pause; safe/protected guard retain guard behavior.
All original sources/goldens are frozen; never rerun their authors.

All new semantic replays pass: four PCM programs (20 windows / 160 stereo
amplitudes), four spark programs (11 total cases) and four Match pause programs.
Focused logs are m4bd-audio-integrated.log and m4bd-cues-integrated.log.
Full Character race regression passes in m4bd-character-race.log (11.149s).
The latest full scoped suite remains M4bb (Match 618.564s); M4bc production/stock
scoped checks also pass. No full Match rerun was needed for this test/data-only
checkpoint.

Validation pipeline93682 exited0: m4bd-validation.log. Fresh native fatal audio,
protected spark and fatal pause reproduce the pinned goldens. Four final apps /
four captures pass prepared-resource/config/binary/PNG/runtime provenance audit.
Two private-device PCM outputs match all 16 selected stereo amplitudes exactly
at the recorded precision. Binary SHA256:
48624d81792daa306e8c9b4af585877014a51e7f581d9f46d85ed7cb659d1b9e.
The source/production audit is temp/m4bd-final-app-audit.json.

**Two production spark raster gates fail and remain unwaived.** Selection and
anchors match, but each native 276-pixel core differs at all 276 pixels, max
channel delta8; Go has an extra left core column (288 pixels). Native hash
5b077cf1947ce2dbe811076af1f3549be4013f1becd5a8148108fc8237a1c238;
Go hash2c55c6b0e790d626fa7c7a6ff42ae51c0fa5969efc45a19b843be1974ef94753.
Cause not investigated. These add to M4az's six alpha precision failures.
Final audio apps end -go-m4bd-verified (5s), spark apps -go-m4bd-cue (3s).
Longer fatal pilots reach missing winner state180 in the minimal fixtures and
are retained only as diagnostics. Shorter captures do not prove their full
lifecycle. Stock lifecycle evidence is separate. Fatal spark was visually read.

No GUI test/native/app client remains. Root module integrity and original native
executable SHA checks pass. Native SHA remains
d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e.
All disposable scripts/captures/reference copies stay ignored under temp.
The private display/audio infrastructure may be reused only after checking it
is still live and that no GUI/audio clients overlap; never affect host focus or
routing. The environment is documented in BUILD.md/AUDIO.md.

## Review and resume decision

PROGRESS-ASSESSMENT.md distinguishes working stock flow from full compatibility,
relates progress to M0-M8, lists known failures and suggests bounded allocations.
Do not infer a whole-engine percentage from fixture or checkpoint counts.
The current HitDef audit is 36 expression-routed keys out of 81 documented keys;
45 literal/absent keys include enums and require appropriate schemas. That is
not a complete-feature count. KO/round clocks, remaining combat/language,
projectiles/reversals/teams, BGM, modes, rendering/assets and final conformance
remain substantial. The controller/trigger tables are dated baseline data, not
an updated completion ledger. Refresh a concise evidence ledger if the user
resumes, then stop at the agreed next batch boundary. **Currently paused.**

## Historical checkpoint evidence

The sections below preserve prior verification details. Their suggested next
actions are historical and do not authorize work during the current pause.

## Latest verified checkpoint: M4bc

Read STAGE-CLEAR.md. Reversal/predecessor9ef4c313 (M4bb). Stage loads DebugBG
with truncation toward zero before nonzero conversion, then clears magenta in
Stage.Draw before layer0. Production and standalone viewer share the path.
Eleven native programs record132 observations,44 source hashes, exact owned
black/magenta region metadata and viewport points. No original art/binaries or
window pixels are committed. Default fit keeps black side bars; stage0/stretch
fills edge samples. Omitted/0/.5/-.5/foo sample black;1/-1/2/1.5 magenta.

Final pipeline44482 completed exit0: m4bc-validation.log. Twelve final apps/
fifteen captures (suffix go-m4bc-verified), all11 color regions/88,000 pixels and
33 viewport points pass prepared-input/config/binary/PNG/runtime audit.
Binary48df6ff9c82e06c1f73b459cd3265a0ceaef0edd18ec9310eda8a22e6b2c5548;
results m4bc-final-app-audit.json. Four fresh native pixel/numeric replays pass
m4bc-replays.log. Scoped sequential races pass m4bc-scoped-race.log:
System4.713s, stock tests6.502s; stage builds. Root module checks pass. Stock7s
was visually inspected: two recovered fighters/no persistent effects. No GUI
client remains. Native original executable remains unchanged.

Diagnostics are preserved: m4bc-before-fill.json proves8,000 original mismatch
pixels; stageclear-half-go-m4bc-before-truncation and m4bc-half-before-check.log
prove the initial float-boolean coercion error. Initial seven app runs use
suffix go-m4bc-final and are not the final binary. Native record/verify tool is
tests/stage_clear_probe.py; Go cannot record a golden. Sources are frozen.
No whole-frame, no-clear persistence, pause/tint/foreground or general renderer
completion claim. M4az's six alpha precision crops remain unwaived.

Next: continue with fatal-guard sound/effect/pause selection and KO/round clocks,
then remaining HitDef schemas/renderer/mode gates. Local commits remain
explicitly authorized; no pause/check-in or remote push. Keep all edits/data/docs
under MUGEN, temp ignored. M4bb9ef4c313 is the preceding full-race checkpoint;
all90 lethality cases passed there along with the rest of the corpus.

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

M4bc closes the bounded diagnostic fill gap. Complete background controllers,
no-clear persistence applicability, pause/tint/foreground and renderer behavior
remain separate gates. Do not relax the strict pixel expectations.

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
