# Implementation checkpoints

The user approved PLAN.md on 2026-10-16 and authorized local commits for
milestones/checkpoints. No remote push is authorized. All source, fixtures,
documentation, and tooling stay under `game/mugen/`.

## M4w — Shared fight HUD projection and native stage cropping

Ten native cases pin 240 numeric values and twenty original marker cores.
StageFit=0 changes script-visible game/screen height and the stage's top cut,
with native integer stage-unit truncation and high/low guideline allocation.
Fight HUD uses motif-width conversion and the world's physical transform/top
origin. SystemFit does not change these fight scenes; KeepAspect fits or
stretches the effective viewport into the actual window. See COORDINATES.md.

Match camera updates/reset preserve the crop. CFG/YAML flags load without
discarding explicit false or resetting absent video values. Window defaults
follow configured game dimensions; Fight Layout uses actual window dimensions.
The prior independent-HUD fit assertion is superseded. Full scoped/pure coord
races and fresh fractional-crop/stretch native replays pass. Three actual Go
windows show exact HUD marker cores and matching world marker centers. Filtered
world edges still differ and remain documented; no framebuffer parity is claimed.

M4v's reversal point is `4c1391e5`. The original distribution/root module/desktop
are preserved. Menu/SystemFit behavior, System/DirectX fallbacks, tall overdraw,
additional cut/zoom/camera schemas and raster fidelity remain open. Continue
world/entity/target identity and runtime redirection before full combat ownership.

## M4v — SuperPause, frozen contacts and presentation ownership

Thirty-eight native programs pin 600 numeric observations, ten static render
cases and three audio programs with seven stable segments/seven Go checks.
Typed SuperPause now owns duration/movetime/preemption, power, unhittable,
acquired-target defence adjustments, own/common cues, position and darkening.
See SUPERPAUSE.md for exact boundaries and remaining schemas/ownership gaps.

Frozen attack boxes may contact moving receivers. Frozen receivers enter their
hit state immediately and accumulate damage until resumption. New HitDef rearms
contacts; active-tick counter aging matches both new freeze traces and existing
hitpause traces. Positive p2defmul stacks on acquired H targets, nonpositive uses
configured 1.5, and recovery clears it. This contact ledger is deliberately not
claimed as the complete world/Target registry.

Super effects keep a spawn position and animate through either pause, including
when their caller freezes; finite actions retire after one pass. Infinite
frames survive pause expiry and anim=-1 replacement. Static scene/opponent/caller
tints and common/own screen-X-panned audio match the measured scope. Full races,
fresh clock/other-owner render and isolated audio replays pass. Real stock KFM
super integration checks 30 frozen ticks, common cues and 1000 power consumption.

Overlapping early capture/audio drafts were discarded; promoted evidence uses
serial owned windows/private audio. AGENTS.md records this workflow requirement.
No desktop/reference/root-module changes were made. M4u's reversal point is
commit `b7df46ff`. The captures expose remaining HUD projection/configuration
differences despite correct world margins; compare fit flags next. Full world,
input, combat and controller gates remain open.

## M4u — Shared GameTime and root Pause scheduling

Thirteen native programs pin 264 observations for duration, caller movement,
issuing/release boundaries, zero/missing/negative arguments, replacement,
cancellation and later-caller precedence. Native accepts omitted time as zero
and resets invalid movetime to zero, contrary to treating it as a clamped range.
The typed Pause controller and production root clock reproduce all packets.
GameTime advances through frozen local Time/position/animation. See PAUSE.md.

The Timeline has independent actor identities, one outer-frame clock for nested
entity adapters, queued requests that preserve the issuing tick, and pause
clocks shared with Match. Match/command-window authored tests exercise phase/
timer/background/effect freezing, pausebg, round reset, and end command buffering.
Those interaction/presentation/input checks are not yet native parity evidence.
All scoped races and pure timeline/engine races pass. A fresh pair-caller native
replay matches. Root modules and the reference remain unchanged.

One SuperPause pilot pins its disabled-presentation root clock for pure Timeline
tests; its production controller is still a stub. Implement the full SuperPause
schema/presentation/protection and native pause/contact/command/effect interactions
next. GameTime absolute origins and complete helper/projectile/world ownership
remain open. M4t's reversal point is commit `05dc5222`; this does not finish M2,
M4, or the whole Pause controller coverage gate.

## M4t — Landing resumption, physics order and shared tick budget

Eighteen native programs pin 336 numeric observations and one runaway rejection.
Movement precedes Physics A acceleration and S/C friction. Ground contact enters
52 without clamping position or zeroing velocity, including MoveType H, and runs
Time=0 CNS/physics again immediately. Negative passes and final time/animation
clocks run once per tick. PosFreeze stops movement while acceleration continues.
State 105 remains exempt from automatic Physics A landing. See PHYSICS.md for
the exact values, custom-state probes and limits.

The shared StatePassBudget now counts CNS transitions across engine/landing
resumes; engine entries and terminal passes do not consume it. Native completes
2510 successive engine landings in one tick, beyond the CNS loop budget. A chain
near the CNS limit can land/resume successfully; its additional transition
fails at pending state 706, matching both production and a headless budget test.

Native custom states 40/50/51/52/100/105 with Physics N retain their authored
states and clocks. Removed Go's forced jump/down/landing/run exits; common CNS
owns those transitions. The production mixed KFM/KFM720 input test now requires
both characters to jump, reach 52 and return to controlled idle on the ground.

Full scoped races and pure engine races pass. Fresh frozen-landing and shared-
budget overflow replays match. The first full race run exposed missing geometry
getters in the new headless test host; those were supplied, and final races pass.
The first transition capture requested a page past the 40-fvar bank; corrected
fixtures use page 28. Root module files remain unchanged. M4s's reversal point
is commit `697fd8e1`. Full world/body/physics/pause and ownership semantics remain
open; no complete state-machine or combat claim is made.
Actual 1280x720 mixed-stock windows were inspected at physics-stock-match/;
their idle scene/timer smoke is distinct from the injected jump test.

## M4s — Native state continuation limit and pending-state diagnostics

Fifteen native programs pin seven accepted packets (84 integer values) and
eight runtime runaway rejections. The former ten-state cap rejected a legal
24-iteration chain. Native self/alternating/terminal and negative-launched cases
establish the actual boundary: 2502 positive executions may complete, while a
further continuation fails with the displayed 2500-loop message naming the
pending state. Source/AIR/executable hashes and rejection text are checked.

Read-only inspection of the pinned executable confirms why the displayed and
execution counts differ: an initial call precedes a post-increment counter test
using greater-than 2500. No binary or reference assets are edited or committed.
See REFERENCE.md for addresses and the exact inference boundaries.

The shared headless executor now follows that boundary and retains source-file
context in runaway diagnostics. Pure engine races and the full scoped race suite
pass, with both hosts exercising all 15 native programs. Fresh terminal-overflow
and maximum negative-launched success replays match. Root modules are unchanged.
M4r's reversal point is commit `4d53ebd2`.

Remaining: the budget across separate engine/physics resumptions, landing/physics
phase timing, all world/entity ownership and Pause/SuperPause. The current
Character landing path enters 52 in finishTick without running its Time=0 CNS
until the next tick; compare that path next. This checkpoint does not establish
complete per-tick state scheduling or full M2/M4 compatibility.

## M4r — Empty-state rejection and validation before merge

Five minimal native cases distinguish empty positive/negative declarations,
explicit entry attributes, same-file duplicates and later-file definitions.
The beta rejects all four effective empty declarations during character loading,
including an unreferenced final state. A duplicate empty state after a valid
same-file definition is ignored, retaining marker 111. An empty state in st0
still fails even when st already owns that ID. This corrects M4q's unproven
assumption that empty declarations were accepted and only needed runtime work.

The loader now validates first definitions in every loaded st/stN/CMD/common
file before merge, preserving the native same-file duplicate rule. It reuses
those compiled programs in the player and includes DEF Files key, source,
state and line in load errors. The constants-only CNS role remains distinct.
The executor's existing empty-state rejection remains correct and unchanged.

Full scoped races pass, including all stock loads, the five source/hash-pinned
cases, and a fresh shadowed-state rejection replay. A final loader/resource race
run also passes after simplifying the file list. One earlier command inherited
the unavailable desktop display and failed at GLFW initialization; subsequent
runs explicitly use the task's Xvfb. No desktop configuration changed.
M4q's reversal point is commit `7a1b9ec9`. Next measure the provisional ten-state
continuation cap, then continue world/entity, pause and full combat semantics.

## M4q — Native viewport profiles, framing and version gates

Twelve native cases cover seven accepted stage/game/local profiles, same-pass
ScreenPos after PosSet, legacy GameWidth/Height/ScreenPos/CameraPos acceptance,
and legacy ScreenWidth/ScreenHeight/CameraZoom rejection. The profile matrix pins
168 float32 observations; all accepted packets match production Character ticks.
Stage aspect determines match height in these scenes, even with a declared 1.1
stage or a differing configured height. The shared viewport supplies contextual
coordinates and propagates through match resets/entity creation. Zoom-one/zero-
camera observations do not establish full camera behavior.

The renderer fits world/HUD separately and draws actors/effects at common game
resolution. The actual 1280x720 owned capture viewport-spark-hd/ confirms the
previously missing world margins. Filter edges still differ from native. The
first capture remained 320x240 because the source config comments its window
settings; the harness now supplies explicit YAML dimensions and validates them.
See COORDINATES.md for provenance and remaining rendering limits.

Character loading retains language version and compiles nonempty states before
gameplay. Three measured 1.1 trigger gates apply to typed parameters, entry and
triggers, including shared AST cache hits; controller source remains immutable.
Empty states are still deliberately skipped pending their runtime comparison.
Other language-version/legacy-handler schemas remain incomplete.

Full scoped races pass, including all stock resource/state loads, viewport
packets and loader/cache version regressions. Fresh stage11 and legacy rejection
replays pass. Root module hashes are unchanged. M4p's reversal point is commit
`253649ce`. Continue native empty-state execution/entry and complete combat/entity
semantics; this checkpoint does not finish M4 or full compatibility.

## M4p — Native spark geometry and production contact effects

Added a shared CPU effect queue/scene with independent AIR clocks, finite
one-pass expiry, explicit infinite frames and scene cleanup. Match binds own/
common resources before state execution. Typed HitDef sparkno/guard.sparkno/
sparkxy expressions preserve source maps; omitted defaults come from CNS Data.
Production drawing consumes actual frames, axes, facing/offset/scale/palette
state and AIR A/A1/S/ASnDm blend metadata, with retired-cache pruning. Other
HitDef fields and full Explod/AIR/world behavior remain incomplete.

Eight authored native programs cover own hit/guard, two offsets, common/default
selection, disabled/missing common effects and Data-prefix coercion. Five
persistent RGB markers and two confirmed absences are pinned with numeric/source/
asset/window/core/recipe hashes. Go frame anchors and resource choices match.
Native common-default and guard replays match the same geometry/core hashes.
The beta accepts Data sparkno=S9992 but exposes zero rather than an own default;
the parser now matches that observation. See EFFECTS.md for precise calibration.

Full races and pure engine/effect/AIR races pass, including independent clocks,
expiry/clear/errors, typed host isolation and actual KFM common spark emission.
Normal/additive markers were inspected through real owned Go windows at
spark-own-production and spark-add-production; additive RGB is visible on the
stage. This is GPU-route evidence, not native framebuffer/blend parity.

The controlled native scene exposes a framing discrepancy: its 1280x720 stage
fits height at scale three with horizontal margins. The provisional M4d Go
width/cut-suggestion path differs. Resolve world/HUD framing and screen/game
coordinate bindings next, then continue remaining combat/entity gates. Native
pause/clock, dynamic Width/mixed profiles, layers/palettes, full AIR/Explod and
capacity rules remain open. M4o's reversal point is commit `6ba7c9c1`.

## M4o — Root KO voices, echoes and global suppression

The shared post-damage root tick now plays own-bank sound 11,0 once on KO,
stopping numbered voice channel 0. KO.echo enables additional voices at native
KO counter values 20 and 50, with 50% and 25% volume requests. The counter begins
at one on the detection pass. Echoes use the same measured screen-X panning as
contact cues. A root's nokosnd assertion suppresses every root's scheduled KO
voice for that tick; suppressing the initial voice does not cancel later echoes.
Missing KO samples warn and continue. Full KO state/kill/revival, helper ownership
and Pause/SuperPause timing remain separate open obligations.

Four authored programs with complete numeric constants pin 21 native segments
and 25 Go tick checks: basic voice, echoes, continuous opponent suppression and
initial-only suppression followed by audible echoes. Short 100-ms Fourier
windows and six-tick Go windows isolate quarter-second original tones. Gain
matches integer mixer truncation, including 25% producing a ratio below exact
one quarter at stock SFX gain. Full races pass and a fresh initial-suppression
replay matches seven segments, source/numeric packets and recipe hashes.
The fixture windows establish plateaus; exact first-sample world-clock boundary
parity and pause interactions are not claimed.

Initial captures were contaminated by the stock spoken KO announcement. Setting
negative announcement IDs alone did not remove it in this distribution. The
final reproducible recipe routes only the disposable HUD to a valid original
empty SND bank; its bank/fight hashes are pinned. Original reference files and
desktop audio routes remain untouched. Earlier captures stay diagnostic-only
under temp/. Optional --ko generation preserves prior default bank bytes.

Next common hit/guard effects, then complete combat/entity and sound configuration
edges. Root KO audio does not close full match/KO or sound compatibility gates.
M4n's reversal point is commit `99372c03`.

## M4n — HitDef sound schema, contact cues and voice interruption

Hit/guard sound parameters now compile expressions, integer tuples, resource
prefixes and item-zero omission before execution. The shared source map stays
immutable across hosts. S selects attacker SND, F or omission selects common
SND. Native leading-prefix consumption also rejects raw Facing as acing;
parentheses protect expressions. Other HitDef fields still use legacy parsing
and require the planned complete schema audit.

A successful hit stops the defender's numbered channel 0 before get-hit CNS
can emit a replacement. HitOverride to an idle state still interrupts it;
guarding preserves it. Other numbered voices remain. Contact cues play their
chosen immutable bank and expire naturally. The beta passes defender screen X
measured from the left edge into its panner: -70 world X gives 90 in 240p space,
and -40 gives 120. Both symmetric scenes match native PCM. Complete camera/zoom,
mixed-coordinate and ownership edges remain to measure.

Nine authored audio programs pin 32 native segments and 45 Go tick checks. They
cover normal hit, guard, override, two positions, distinct common versus own
tones, group-only F syntax, initial omissions and a later HitDef omitting prior
choices. Omissions emit no cue in these scenes and clear the earlier choice.
Five grammar programs add three accepts (including assignments) and two load
rejections. An assignment expression's native fvar packet confirms execution.
Full races pass; fresh override-sound and contact-sound-common-single replays
match levels/relative onsets and the disposable common-bank recipe hashes.

The real stock KFM logical walk/punch/hit/recovery test now also requires a
common hit command and nonzero shared-mixer PCM. Source banks are bound by M4m
before state execution. The original bank generator optionally adds one-second
contact/common tones without changing its prior default output. Initial tests
ran before the last captures had tick-check metadata and failed on missing
fixtures/checks; after promoting complete evidence, every comparison passes.

Next: KO voice/echo and sound configuration edges, common hit/guard effects,
complete HitDef/protection/priority/reversal schemas, and full world/entity
ownership. None of this promotes a whole controller to Verified or closes M4.
M4m's reversal point is commit `b616a7e4`.

## M4m — Production sound stream, fight banks and HUD cues

Menus and fights now share the CPU mixer through one endless Ebitengine audio
player. Stock SampleRate, BufferSize, WavChannels, master/SFX/BGM volumes and
resampling metadata load from [Sound]; the device buffer uses 2^BufferSize
frames. YAML false/zero overrides retain presence. Live SFX gain changes affect
active voices, Sound=0 opens no player, and disabled playback drains commands.
Invalid device resource bounds have contextual errors; these limits are not
claims about native invalid-config fallbacks. BGM and native-quality resampling
are retained settings, not implemented playback features.

Each fresh match/round character binds its own SND and the fight's common bank
before entry controllers. Match resets and leaving a fight issue an ordered
scene clear that also retires decoded assets. This cleanup is a provisional
lifecycle policy; complete native round/custom-state behavior remains open.
HUD round/fight/KO/DKO/time-over and power-level cues use the fight SND, respect
sound delays, and fire once per event. Exact native announcement offsets and
complete layout variants still depend on the open round scheduler work.

A new authored HUD-tone recipe patches only disposable fight definitions. The
native round cue produces the same measured stereo gain as the Go HUD stream;
two native segments pin a one-second tone and expiry. Recipe/fight/source/bank/
config/PCM hashes are retained in hud-sound.json. Native replay verifies those
levels. No absolute announcement start-time parity is claimed.

Full races pass, including production title/select/versus/fight/two-round/result/
rematch/exit with PCM reads, bank binding, delayed/one-shot HUD cues, power cues,
live mute/unmute and looping-scene cleanup. Actual device playback is captured
at temp/app-runs/sound-match-playback through task-local ALSA -> private Pulse
null sink. The 44100-Hz stereo capture contains 433534 frames (peak 32767, RMS
1692.49); owned-window images at 2/5/10 seconds were inspected. This establishes
nonzero production device output, not waveform parity. ALSA's control-device
enumeration warning did not prevent the explicit private PCM route.

Next: contact/guard/KO voice and effect cues, complete HitDef sound expressions
and bank selectors, channel-0 interruption, entity/round lifecycle, BGM and
storyboard audio. The full playable M4 and full sound compatibility gates remain
open. M4l's reversal point is commit `dfe8ae1b`.

## M4l — Typed sound commands and native PCM mixer checks

PlaySnd, SndPan and StopSnd now compile typed parameters into an ordered CPU
command queue. Sound banks stay immutable; audio owners are separate from input
slots, including helper/projectile allocation. The thread-safe S16 stereo mixer
handles channel replacement, low-priority requests, automatic voices, owner
stops, global StopSnd -1, loops, finite sample expiry and PCM decoding/resampling.
Fragmented reads preserve frame alignment through reset. Production playback
and match resource binding are the next checkpoint; no fight audio claim yet.

Twenty-three authored programs cover 24 native cases: twelve audio cases and
twelve accepted/rejected grammar cases. Native capture uses only a private,
authenticated PulseAudio null sink. Fifty-five stable tone segments produce
63 Go tick checks, including stereo samples and both 1.0/1.1 character versions.
Source/AIR/original tone-bank/executable/config/PCM hashes and settings are pinned.
A fresh sound-channels-audio-replay matches amplitudes and relative cue onsets.
The full scoped race suite and display-independent mixer/evaluator races pass.
See AUDIO.md and tests/README.md for reproduction and evidence limits.

The beta's behavior differs from plausible manual interpretations: PlaySnd pan
uses common-game pixels while SndPan uses character-local units; pan supplies
the value when both offsets exist but abspan still selects absolute mode.
Deprecated volume is parsed/evaluated but ignored. Tested freqmul values do not
change pitch, even for 1.0 characters; nonpositive values still start a sample.
A leading F/S is stripped before expression parsing, including Facing/FVar;
parentheses protect identifiers. Unavailable samples warn and continue.
Measured integer panning/volume truncation and center-channel asymmetry match.

Open: production/HUD/hit cues, device and round lifecycle, channel-0 hit/guard
interruption, complete ownership, arbitrary-rate/libresample fidelity, capacity
fallbacks, extreme/bottom arguments, stereo/config edges, BGM and storyboards.
The queue cap and linear resampler are diagnosed provisional implementations.
Known-trigger validation is opt-in for sound, not complete language validation.
No whole sound controller or asset row is promoted to Verified.

## M4k — Guard distance, move-type order and body push

Guard range now belongs to the attacker, in its local units, and extends in the
attacker's facing. Defender facing and vertical separation do not affect the
query. The upper boundary is strict; the same axis is eligible. MoveType A is
sufficient without an active HitDef. Range starts from size.attack.dist, explicit
HitDef guard.dist/AttackDist changes it, an omitted later guard.dist retains it,
and ordinary nonpersistent state entry resets it. AttackDist now compiles its
required integer expression and updates this shared value.

The cross-player clock proves state passes prioritize attackers over idle/hit
players, retaining P1-first order when both attack. Priority is selected before
passes: the I-to-A tick still follows the idle order. Production now partitions
root passes accordingly. This refines M4g/M4i ordering; static contact/life
observations alone had not established cross-player script order. Complete
helper/projectile, pause and physics ordering remains open.

Native spatial probes distinguish guard range from push/turn side effects.
Custom states retain facing instead of turning every tick. PlayerPush now has
typed boolean control, a per-tick reset, and pair suppression when either player
disables pushing. Ordinary ground overlaps split equally. For axes less than ten
240p units apart, the beta adds spacing of ten minus their separation before
splitting the correction. Exact, small, signed, fractional and cutoff probes
pin this behavior; a 1280/320 pair confirms its coordinate scaling. Previous
pure geometry tests had not established this native near-axis correction.

Twenty-three source-pinned programs cover 26 native cases, including two mixed
range directions and mixed push, and all compare production ticks. Full races
pass and guard-distance-clock-replay independently reproduces its packets.
The harness adds explicit fixture-owned constants, preserving existing modes;
push fixtures pin numeric KFM defaults with ten-pixel body widths. Failed drafts
with incomplete/duplicate constant groups remain diagnostic-only under temp/.
The native life-only Data group rejection also needs a dedicated character
constant-loader validation audit; it is not a state-parser rejection rule.

Complete auto-turn eligibility, airborne/corner/pause push, dynamic Width,
projectile guard range, HitDef expression/default coverage and full world/entity
ordering remain open. This is a measured guard/push checkpoint, not the full M4
gate or a Verified whole-controller/trigger claim.

## M4j — HitBy/NotHitBy protection slots and attack posture

Replaced unused string flag maps with two shared timed protection slots. HitBy
sets allowed state/attack masks; NotHitBy complements each component separately.
All active slots must allow a contact. Partial posture-only, attack-only and
empty masks are distinct; HitBy SCA has no allowed attack classes, while
NotHitBy SCA blocks all postures. A blocked HitDef stays active and can connect
after protection expires. Time defaults to 1, time=0 clears, time=-1 persists,
and both controllers overwrite the same slot selected by value/value2.
Typed compilation evaluates duration before mutation and diagnoses malformed or
missing attributes. The old raw methods/handlers are removed.

Twenty-five native protection programs (24 accepted, one load rejection) match
production collisions, including last-tick/expiry, slot intersection/replacement,
empty/partial masks and parameter precedence. The beta accepts both value and
value2 and uses value, contrary to the manual's one-parameter rule. It rejects
omission of both. Attack posture comes from the attacker's current StateType,
including a change after HitDef execution; the authored attr posture does not
control protection matching. Two further HitOverride probes establish the same
rule, now shared by contact snapshots without mutating authored definitions.
GetHitVar(damage) exposes pending damage during the contact pass and clears
after damage processing; later native observations return zero.

Full tests/races pass; protect-dynamic-pose-replay independently reproduces the
native case. Native resource/source hashes remain pinned; all edits stay scoped.
Complete invalid mask/value fallbacks, pause/superpause aging, custom-state and
helper/projectile ownership, hit flags, priority/reversal and guard-entry distance
remain open. No entire controller/trigger inventory row is promoted to Verified.

## M4i — Contact activation, separate clocks and guard response

Twelve new source-pinned native programs distinguish contact activation and
clocks from the earlier static trade observations. A newly defined HitDef first
connects on the next tick, before negative/positive state passes. Shared roots
now decrement existing clocks, collect prior definitions, snapshot contacts,
enter get-hit states, age defenses, run state passes, deduct damage, then finish
physics/animation/time. This supersedes the state-before-contact assumption in
M4g. Its trade fixtures still pass but did not discriminate those schedules.

Attacker hitpause and receiver hitshake are separate. Hitpause suppresses normal
CNS/time/animation and releases when its pre-pass decrement reaches zero. Victim
CNS and Time advance during hitshake; common CNS decides when recoil begins.
GetHitVar(hittime) counts down after hitshake reaches zero, stops at -1, and
HitOver becomes true below zero. Timed HitOverride slots remain active through
contact before aging/renewal, preserving the measured last-tick behavior. This
supersedes the unmeasured M2b shared ShakeTime/last-paused-tick interpretation.

Guard response now chooses guard flags by posture, guard damage, guard-specific
pause/shake, velocities, hit/slide/control times and 150/152/154 states. Guard-end
140 remains eligible. Matching HitOverride suppresses guard response. Received
guard vectors scale without mutating attacker data; contact counters distinguish
MoveGuarded from MoveHit. GetHitVar damage/guarded/hitshaketime are typed integer
bindings. Air guard reports hittime=-1 and slidetime=0. Omitted guard velocity,
pause and control defaults match the executable. In this beta guard.slidetime
inherits ground.slidetime, contradicting the manual's guard.hittime default.

Full tests/races pass, including all new captures and the previous coordinate,
trade and override cases. A fresh pause-release replay reproduces the six victim
clock packets. Original guard-end fixtures and reference assets are unchanged.
Guard entry distance/input timing, multiple contacts, hit flags, damage scaling,
priority/reversal/HitBy, helpers/projectiles and complete GetHitVar remain open.
This closes measured clock/guard cases, not all combat or the M4 gate.

## M4h — Stock HitOverride and get-hit context

Registered/compiled HitOverride with typed state/slot/time/forceair values and
shared display-independent hit attribute filters. Eight timed slots survive
state entry, lowest matching slot wins, and blank/omitted attributes or missing
stateno disable the slot. The last two omissions are accepted by the beta despite
the manual calling those parameters required. Stock time=0 clearing works.
Both KFM variants now compile all 108 states; the three diagnosed missing-
controller gaps are replaced by a complete stock compilation gate.

Twelve source-pinned native cases match default duration, interval expiry,
nonmatching/projectile attributes, slots, forceair, clearing and omission forms.
Native override first enters the standard 5000/5010/5020 get-hit state without
executing its controllers, then the selected override: PrevStateNo exposes that
intermediate entry. Forceair selects air vectors/time while allowing the target
Statedef to remain S; it does not force fall when air.fall=0. Stored hit-air
context is now separate from current StateType/fall. Native deferred damage is
applied only if the resulting contact state remains movetype H; I destinations
retain life while H destinations take damage. These observations are matched by
the shared production tick. A fresh forceair replay matches; full races pass.

Full HitOverride/custom-state behavior is still partial: matching p1stateno /
p2getp1state exclusions, invalid-value fallbacks, hitpause duration edges,
helper/projectile scheduling and ownership remain open. HitDef defaults/flags,
priority/guard/reversal and complete GetHitVar semantics are also incomplete.
No full controller/trigger inventory row is promoted to Verified.

During this checkpoint all desktop outputs disconnected. The native captures
stalled at frame 5 and Ebitengine tests failed with no-monitor initialization.
Xvfb 21.1.12-1ubuntu1.8 was downloaded/extracted only under temp/virtual-display;
a private authenticated :97 server and software GL restored tests/probes. No
system package/display configuration was changed. See BUILD.md for provenance
and environment. The failed early override captures remain as diagnostic evidence.

## M4g — Shared root-player tick/contact phases

The state-before-contact schedule described here is superseded by M4i. The
original static trade observations did not distinguish the two schedules.

Production matches and the character diagnostic now use character.TickCharacters:
all root state passes run before contact collection, eligible contacts are
snapshotted before any victim transition disables another attack, then contact
entry/positive-state continuation runs before damage and physics/time. This fixes
the previous P1-update-before-P2-state-pass advantage. Positive continuation uses
the existing executor and does not rerun negative states; engine-driven builtin
transitions also run their new Time=0 positive state before timers advance.

Three native trade fixtures pin first-pass and next-tick Life/P2Life/state/contact
observations. Equal-priority hits trade, get-hit Time=0 sees 1000/1000 life before
deduction, and Time=1 sees 975/975 and both players in 5000. The same observations
hold with 8,8 hitpause: the first contact-state pass runs despite the newly started
pause, while HitShakeOver still sees pause state. A fresh paused-trade replay
matches. The older hit-coordinate/default fixtures now use the production group
tick too. P2Life/P2StateNo and type queries use the current opponent context.

Full tests/races pass, including a regression preventing repeated negative passes
and Time=0 entry effects across hitpause. This is root contact ordering evidence,
not complete world phases: priority/attribute/guard/overrides, entity identity,
multiple contacts, helpers/projectiles, push/physics ordering, KO accounting and
pause duration edges remain open. Character/asset imports still initialize
Ebitengine, so full display-independent Match extraction is not complete.

## M3i — Selected v2 palettes and compiled RemapPal

Added instance-local palette maps over immutable SFF data. RemapPal now compiles
both literal/expression tuples with documented -1,0 defaults and validates all
arguments before mutation. Its former raw no-op path is removed. Mapping applies
one lookup (non-transitive), supports identity/destination removal and wildcard
reset, enforces eight slots, and checks source/destination depth/existence.
Invalid resources/capacity failures warn without altering mappings. These rules
are backed by the supplied controller documentation and authored tests; no full
native RemapPal conformance claim is made yet.

Common 5900 now applies PalNo to both stock characters, including round reset.
Character and HUD portrait rendering use the same mapping-aware sprite cache;
revision changes refresh colors, idempotent remaps preserve cached images, and
retired round owners are pruned on draw. Current helper/projectile ownpal paths
share or clone palette state, while their full parameter/lifecycle behavior
remains open. Tests compare every indexed pixel using the selected palette in
both stock archives and preserve source pixels/masking.

Full tests/races pass. Owned-window capture selected-palette-fight/ shows P2's
alternate stock colors at the correct mixed-resolution size. SFF v1 external
palN selection still uses the old fallback and needs separate native fixtures;
custom animation ownership, Explod ownpal and the complete helper palette rules
are pending. This closes the stock v2 selection/rendering gap, not the M3 gate.

## M4f — Received hit coordinates and native float overflow

Received HitDef vectors now copy/convert into the defender's local units;
shared attacker slices stay unchanged. Current knockback is no longer assigned
with an invented reversed sign on contact: get-hit CNS applies stored vectors
through HitVelSet. Explicit acceleration/fall vectors, omitted defaults and
unchanged entry velocities match six native contact cases across 320/1280 player
spaces and common game widths 1280/640. Default yaccel is .35 in 240p units,
not the old .44 fallback; explicit zero remains distinct from omission.

Omitted fall.xvelocity has a native no-change sentinel in common game units.
Its reported float scales with local/game width and can become negative infinity
when the local width exceeds game width. A native edge capture initially failed
the numeric-only glyph reader; it now exactly recognizes Windows #INF output
and writes JSON-safe labels. A fresh smallgame replay matches. Character/helpers/
projectiles retain the common profile as well as their local profile.

Separate language probes confirm float arithmetic, Exp and exponent overflow
produce IEEE infinity, with ordinary infinity comparisons and reciprocal zero.
The evaluator no longer converts infinity to bottom; NaN/domain failures remain
bottom. Scientific literals are now tokenized. The beta accepts decimal-point
forms (1.0e2, .5E+1, 2.E2) and rejects 1e2; Go preserves that restriction.
Native-pinned Character tests and the JSONL runner cover the new semantics;
JSONL traces use a nonfinite label because JSON has no infinity literal.

Full regression/race checks pass. These cases do not complete HitDef schemas,
air/guard contact selection, no-change bounce application, hit/fall ownership,
world phase order or every GetHitVar field. Numeric overflow/domain/conversion
edges beyond the recorded cases remain open. No full combat row is Verified.

## M4e — Guard-state ownership and movement control

Removed the per-tick guard maintenance that re-entered 140 and skipped custom
120 logic. Common CNS owns guard start/hold/hit transitions. The documented
engine return from 140 now waits for AnimTime=0 and selects standing/crouched/
air states, replacing the fixed 20-tick delay. Guard entry, jump/crouch/walk and
crouch release require control.

Three hash-pinned native fixtures match 36 recorded state/time observations:
a five-tick guard-end animation returns to state 0, a custom 120 stays in 120
without an engine-imposed 130/140 transition, and an uncontrolled crouched 11
stays crouched. A fresh guard-end replay matches. Authored tests also hold each
movement direction with Ctrl=0. Full tests/races pass. This does not complete
native guard-entry eligibility, guard flags/damage/velocity/pause, or combat phase
ordering; those remain separate work.

## M4d — First production fight connection

Selection now retains DEF/name/palette/mode/music metadata, confirms P1/P2
independently and offers roster/extra stage choices before versus. Missing roster
resources report paths instead of silently emptying the selection. Versus starts
FightState rather than returning to title. A shared match.Match loads stock
players/stage/fight assets, owns round/intro/control/timer/KO/time-over/result
state, resets characters between rounds and retains wins. Character input can
come from an instance-local logical source. RoundNo, PalNo, AILevel, LifeMax and
PowerMax now have real instance data. The former per-tick forced 191/180 entry
is removed so scripted intro/victory states can finish.

Stage uploads are lazy on the drawing thread. The production fight draws mixed
character scales, a cropped stage, life/power bars, names/portraits, time, win
icons and announcements from fight.def. It offers result Start/rematch and
Esc/menu. A basic deterministic AI policy uses the logical input path in current
single-player routes; full arcade/team/mode progression is not implemented.
Quick-match CLI and owned-window smoke captures exercise the production state.

Full tests/races pass, including stock mixed spawn/intros, KO/reset/two-win
completion, life-fraction time-over, real logical walk/punch/hit/recovery, and an
injected-key title/select/stage/versus/fight/result/rematch/exit test. The first
attack assertion incorrectly used KFM's kick button; it was corrected to the
stock X punch binding. Captures first-mixed-fight show matching character sizes;
the portrait scale mismatch was corrected for the HUD coordinate space.

This is a connected fight checkpoint, not the completed M4 gate. Native round
phase/camera ordering, simultaneous combat phases, correct guard/recovery,
hit-vector scaling, selected palette application, channels/common effects/audio,
HUD alpha/interpolation and complete mode progression remain open. ResetBG,
max-draw handling and full aspect/overdraw/zoom rules are not implemented.
See MATCH.md for controls, validation and concrete limitations.

## M4c — Mixed-player geometry foundation

Two hash-pinned native cases use separate 320/1280-localcoord players. Their
Pos/P2Dist/P2BodyDist observations now match Character: starts -70/-280,
axis distances 140/560 and front distances 120/480. P2BodyDist formerly reused
axis distance; it now subtracts the appropriately converted body fronts and
retains axis-based Y distance. A fresh inverse-case replay matches.

Cross-player positions, facing/guard-distance checks, collision extents and push
displacement convert into the querying player's units. Authored tests check
symmetric mixed-resolution collision, separation, equal physical push movement,
and facing when raw coordinates give the wrong ordering. Collision/push phase
ordering, airborne/overlap/custom Width semantics, acquired targets, hit-vector
conversion and camera transforms remain incomplete. Full race tests pass.
This checkpoint does not yet connect a production Match.

## M4b — Coordinate profiles and scaled constants

Added width-ratio coordinate spaces, retained character/motif/stage/game profiles,
and contextual dimension validation. Fractional localcoord width probes show
320.5 and 640.5 truncate to 320/640; the parser accepts this coercion rather than
rejecting them. Config.GameWidth/GameHeight are distinct from video window size.
Helpers/projectiles inherit their creator's local profile.

Const240p/Const480p/Const720p now bind to that profile and return float32. Five
source-pinned native cases match all 60 captured fvars (30 nonzero conversion/
type observations); the division case confirms float return behavior. Bad arity
is rejected in compiled expressions, and bottom/missing context are tested.
A fresh fractional640 native replay reproduces every value.

Production menu layout honors motif localcoord, background default windows use
its size, and the drawing origin uses the actual logical horizontal center.
Physical window dimensions no longer double implicitly. The high-resolution
MUGEN title now fits and centers correctly in an owned-window inspection; the
first capture exposed and helped correct the remaining fixed 160-pixel origin.
Full regression/race checks pass; root module files remain unchanged.

Mixed-player position/collision conversion, game/stage camera/crop/overdraw,
custom ownership and full fight/storyboard spaces remain open. These are profile
and expression foundations, not a completed Match/Fight state. See COORDINATES.md.

## M3h — Fight/HUD resource definitions

Added fight.Load for the stock fight definition, main SFF/SND/fonts, embedded
animations, fightfx sprites/AIR and common sounds. It retains the raw layout AST,
attributes and source locations for the production HUD renderer rather than
inventing replacement graphics. Required and declared missing/malformed assets
retain the parent Files key/line and resource path. Font lookup follows the
same documented directory order as the motif; font-height keys are not filenames.
DEF extraction can now reuse an already-parsed AST.

Both stock fight definitions load their complete declared asset groups and
retain life/power/time/round layout attributes. All four motifs hand off their
resolved fight definition correctly; big shares mugen1/fight.def. Regressions cover missing/malformed
resources in all six file categories. Full tests/races pass; root module hashes
are unchanged. Loading a HUD definition does not render it or complete fight
flow: bars, portraits, round announcements/effects and audio consumption remain
M4 presentation work, and language/font-height variants are still pending.

## M4a — Stock keyboard symbols and explicit binding presence

Replaced the incorrect DOS scancode conversion with SDL 1.2 symbols used by the
supplied beta config. All 22 stock bindings now map correctly, including P1
arrows/punctuation and P2 letters. Keys retain explicit presence independently of
numeric value: KeyA=0 works in legacy config, YAML overrides and input polling.
Unknown/unbound keys cannot silently poll A. Joystick identifiers remain raw
instead of passing through keyboard conversion; the gamepad adapter is pending.

Named overrides use Ebitengine's key-name parser, including function/keypad keys;
none/disabled explicitly unbinds. The manager accepts an instance-local keyboard
source, supporting injected held/edge events without OS input. Menu confirmation
uses each player's Start binding. Device ID collection no longer duplicates
newly connected gamepads. See INPUT.md for defaults, APIs and limitations.

Tests cover the full stock binding corpus, zero/presence across value copies,
YAML KeyA/disabled overrides, unknown symbols, injected held/edge and isolated
player confirmation. Loaded stock keys reach the real facing-relative gameplay
input path. Full tests/race checks pass; root module hashes remain unchanged.
This is an input foundation: no first playable match, complete command timing,
keyboard-layout or gamepad parity is claimed.

## M0d — Production startup captures and diagnostic ownership

A repeatable app_smoke.py builds the actual application with the scoped module,
uses a disposable stock copy/explicit empty override, and captures only its owned
window at requested times. It sends no input or focus changes and stops only its
own process. Binary/config hashes and commit/dirty metadata accompany captures;
images/logs remain in temp/. This is visual inspection, not a native pixel test.

The KFM motif startup reaches the Elecbyte logo (2 s), animated intro (6 s), and
readable title menu with its correct separate title graphics (16 s). Captures
were inspected in kfm-startup1 and the isolated kfm-startup3 repeat. The earlier
kfm-startup2 repeat exited cleanly after two captures while other graphical tests
were running; its cause is unestablished and its failure/evidence remain recorded.
Do not treat this live desktop tool as deterministic input/timing conformance.

AIR/background parsers no longer warn about normal unrelated sections of shared
DEF ASTs or emit unconditional sprite/action debug prints. Unsupported attributes
inside their own sections remain diagnosed, with a regression proving both
properties. Full tests/races pass. Startup diagnostics also expose a separate
stock keyboard-number conversion issue (the current mapper is DOS-scancode based);
resolve it before relying on keyboard gameplay. No playable match or full visual
parity is claimed.

## M3g — Shared storyboard resources and application handoff

Storyboard.Load now parses once, constructs CPU sprite/animation/background
resources, loads declared SND/fonts, and delays GPU uploads until drawing. The
production storyboard state uses it and returns errors instead of printing and
skipping failed cutscenes. Runtime animation failures retain file/scene/layer
context. Motif logo/intro/select/fight paths resolve relative to the motif;
selection no longer prefixes an already-resolved path with data/ again.

All eight stock storyboards load and finish bounded CPU playback with their
configured scene durations. Their scene backgrounds are retained, including the
KFM intro/credits backgrounds previously discarded. Tests cover declared fonts/
sounds, layer 99 (the documented limit is 100 layers, not the former 10), scene
inheritance metadata, missing assets/actions/end.time, invalid start indices,
checked player construction, application failure propagation and selection paths.
The documented default BGM loop flag is false. Clearcolor=-1 is retained as a
no-clear flag; actual persistent-framebuffer rendering remains pending.

Full regression/race checks pass; root module hashes remain unchanged. This is
loading/playback integration, not complete storyboard parity: text, timed sound/
music playback, window/transform/blend behavior, zero-length scene boundaries,
first-tick animation ordering, clear persistence and reference screenshots still
need work. The public legacy Storyboard.Run stub is not used by production.

## M3f — Per-background sprite archives in production drawing

Motif TitleBGDef/SelectBGDef/VersusBGDef/OptionBGDef spr declarations now load
independent archives with contextual errors. Elements bind to that archive, and
all production title/select/options/versus draw phases select the same background's
images. A shared DrawLayer path retains masked/opaque variants, foreground order
and lazy GPU upload on the drawing thread. Backgrounds without an override still
use the motif's shared archive.

The KFM motif's separate kfmtitle.sff is loaded and its title elements reference
those sprites. Original tests cover archive ownership when sprite IDs collide,
layer selection, cache reuse, shared fallback, and missing/malformed overrides
for all four supported screen backgrounds. Full regression/race checks pass and
root module hashes remain unchanged. Screenshot parity, complete background
controller/transform behavior and storyboard resource handling remain open.

## M3e — Motif/stage resources and font directory integration

Motif main SFF/SND/font and stage SFF loaders now propagate declared-file
resolution and parsing errors with source path, section/key and line. Embedded
AIR errors propagate as well. They use the shared resolver for Windows separator
and case handling. Bitmap FNT v2 child archives resolve relative to the font DEF
and retain parent/child paths on failure. The legacy ResolvePath wrapper delegates
to that resolver; strict loaders use its error-returning API directly.

Font search follows the order documented in stock system.def: system directory,
program root, data/, font/. The old loader omitted font/, so it could silently
leave all standard motif fonts unavailable. fontN.height no longer overwrites
fontN's filename; applying the height override itself is still pending. Missing
Windows TTFs retain the explicit portable fallback described in BUILD.md.

All four stock motifs load their main archives and declared fonts, and all three
stock stages load sprites/backgrounds. Original regressions cover documented
font search priority, missing/malformed sprite/sound/font resources, exact error
locations and mixed-case Windows font-child paths. The old synthetic motif test
now supplies real temporary assets and asserts loading instead of tolerating
missing files. Full regression/race checks pass; root module hashes are unchanged.

This is resource integration, not rendering parity. Per-background SFF overrides
(such as KFM title graphics), storyboard asset/error handling, language-specific
motif sections, font height overrides, audio playback and full stage/background
behavior remain incomplete.

## M1d — CNS controller blocks and duplicate-state binding

The reference state-binding fixture records 111/222/0: controllers with cosmetic
header numbers 999 and 0 both belong to the preceding Statedef 0, and the later
duplicate Statedef 0 contributes no controllers. A separate orphan fixture shows
controllers before the first Statedef are ignored. The Go parser now attaches
to the current block, preserves the first definition, and parses duplicates into
detached blocks so their controllers cannot leak into the selected state. It no
longer invents virtual states from controller labels.

Headless and Character state passes match every recorded variable for both
fixtures. Parser assertions check the absence of phantom states, selected block
source/line and controller order. The full suite/race checks and a fresh native
binding replay pass; stock players retain their known 105/108 compilation
inventory. Malformed duplicate-block validation, unusual header spelling and
special negative-state ownership/eligibility remain separate reference cases.

## M2f — Reference-correct transition/entry ordering

An authored ordinary-state sequence records control and animation across seven
entries, with omitted/explicit transition parameters, conflicting Statedef
parameters and an entry expression reading the incoming control flag. All 24
recorded variables match the headless executor and production Character after
fixing the shared entry path. Transition ctrl/anim apply before destination
entry expressions; explicit Statedef ctrl/anim take precedence. This supersedes
M2b's incorrect assumption that controller parameters win. Entry remains a
single activation and is not reapplied during hitpause or repeated Time=0 passes.
A fresh native replay reproduces both pages. Full regression/race checks pass.

The three M3d common-state differences were isolated to the probe's round startup:
RoundState=2 becomes visible before the round's initial control grant. The probe
now waits for ctrl as well, then forces the transition. The Go runner begins at
that same controlled point. All twenty resource cases now compare directly,
without known-difference substitutions. No special ctrl default or state-0 hack
was added. Production round setup/control scheduling still belongs to M4.

The new evidence establishes these parameter interactions, not the complete
entry expression ordering, owner changes, self-transitions or all special states.

## M3d — Character resource loading and measured state-file precedence

Character loading now requires primary cns/st/cmd/sprite/anim files, propagates
all parse failures, and loads declared sound banks. Errors retain the DEF path,
Files key/line and failed resource; later state compilation/execution retains
its CNS/CMD filename. DEF metadata and raw filenames survive parsing. The shared
resource resolver accepts Windows separators and case-insensitive paths with
explicit directory order, exact-name preference and ambiguity errors.

Native probes establish first-definition precedence: st, then st0..st9 in numeric
order, then CMD states, with common states filling remaining IDs. DEF declaration
order does not change that precedence. The cns entry supplies constants only;
its states participate only if the file is also listed as a state file. Blank
numbered st entries are ignored, while primary st is required; blank sound is
optional. Blank/omitted stcommon uses standard common states. The process-global
mutable common cache is removed; independent loads see current files without
mutating earlier players. LoadOptions accepts explicit root/motif directories.

At this checkpoint, twenty pinned resource cases ran through the real loader
and state pass. Seventeen matched every recorded observation/rejection. Three
common cases retained a control difference that M2f above later isolated to the
probe's round startup. That temporary inventory was removed after the probe
waited for the native control grant and Go began at the same point. Fresh
reversed-order replay passes. Missing/malformed resources for all
seven supported resource groups, case/Windows paths, named runtime errors and
independent common loads have regression coverage.

Both stock players still load 281 sprites, 117 actions, 12 sounds and 108 states;
105 states compile, with the existing three HitOverride gaps. Palette selection
still uses the earlier ACT fallback and needs its own reference-backed integration.
Same-file duplicate-state/header semantics, exact common search precedence,
constant required fields and non-character resource validation remain open.
Full tests and race checks pass; root module hashes stay unchanged.

## M3c — Bounded sound archives, PCM decoding and resource probes

SND extraction validates counts, offsets, cycles, lengths and each embedded WAV,
retaining version/format/rate/channel/frame metadata. Both zero and EOF tail
sentinels are supported: every stock archive uses EOF, so requiring zero would
incorrectly reject them. RIFF chunk bounds, padding, PCM alignment and byte rates
are checked. CPU decoding supports integer PCM8/16/24/32 and float PCM32/64;
unrecognized codecs remain identifiable but decoding them returns an error.

All 45 sounds in the five stock banks parse and decode completely (the stock
payloads are mono PCM8). Original fixtures cover signedness, stereo, wider PCM,
float samples, odd padding, malformed chunks/links/counts and nonfinite samples.
A bounded sound-archive fuzz run completed 141173 executions without failure.
Full regression/race tests pass. Audio channels, mixing/resampling, runtime sound
controllers and non-PCM codecs remain M6 work. Per-sound/raw-bank allocation
budgets are 256 MiB/512 MiB; these are parser bounds, not measured native limits.

Reference file probes establish that st is required, missing declared sound and
common-state files are fatal, and numbered state files include st0 through st9
without requiring a continuous sequence. st10/st12 are ignored. Boundary cases
were checked for 1.1 character declarations as well. The authored resource case
matrix and error/numeric evidence are pinned for the next loader checkpoint;
the existing Go loader has not yet been changed to match these rules.

## M1c — Documented constant bindings and reference rejections

Added a typed catalogue for all 74 documented Const names. The supplied beta
rejects `movement.jump.changeanim.threshold`, including with mugenversion=1.1;
it is marked unavailable rather than assigned an invented value. All other 73
names bind through retained CNS fields, including previously discarded recovery,
friction and tuple parameters. Integer/float32 kinds follow their declared
types. The fall defence multiplier is computed from fall.defence_up. Data.power
defaults to 3000 as observed for stock KFM and initializes maximum power.

Compiled triggers, entry attributes and core controller parameters now reject
unknown/unavailable constant names before side effects. Numeric constant fields
fail with line/field context on malformed or out-of-range values. Player loading
retains the main CNS constant table when merging state files.

Validation: all 73 supported stock values match seven reference clipboard pages;
tests separately assert documented kinds, schema coverage, tuple projection,
malformed input and unavailable-name rejection. The reference parser rejection
is captured as a dedicated authored fixture and reproduced for 1.1 characters.
Fresh supported-value probe pages reproduce their assertions. Full tests/race
checks pass, and root module hashes remain unchanged.

The reference rejects omitted/incomplete Data groups with "Can't load Data
constants". The exact required-field set, other omission defaults, incomplete
tuple semantics and version/localcoord interactions remain loader/reference work;
the 73-value comparison is not a complete constants conformance claim. Added a
small Win32 helper to read only the private reference error dialog, built by
`mugen --reference-tools`; regular captures still require no input/focus changes.

## M2e — Reference state order and corrected persistence

Added authored negative-state and persistence probes to the repeatable capture
runner. The reference trace is 32145 for -3/-2/-1/positive/next-positive order;
post-transition controllers remain unexecuted, and the new positive state sees
Time=0 and the correct previous state. Character and pure tests now share
RunStatePass; both execute the authored programs against recorded variables.

Executable persistence observations **supersede M2a's documentation-based
interpretation**. Persistent N fires on the first true trigger, then suppresses
the next N-1 eligible passes before checking triggers again. False conditions
after that cooldown leave it ready. Suppressed controllers do not evaluate
triggers; a used persistent=0 trigger also stops evaluating. The intermittent
fixture fires at ticks 0,2,5,8,11, with eight trigger side effects; its one-shot
trigger evaluates once. Fixed the gate/order and replaced the earlier incorrect
success-count/one-shot tests. Pause/negative-state reset interactions still need
additional probes. The diagnosed 10-transition runaway guard is provisional.

The harness supports named CNS/AIR/CMD fixtures and records only disposable
evidence with --record; normal runs check pinned engine/fixture hashes and exact
observations. Full tests/race checks and fresh probe reruns validate this
checkpoint. No complete world/state-ownership coverage is claimed.

## M2d / M0c — Shared animation clock and executable reference trace

Replaced the background/storyboard and Character animation clocks with one
display-independent AIR player. Frame durations, zero-duration skips, infinite
holds, element selection, reset and zero-cycle diagnostics now share that path.
Character implements AnimElemTime/AnimElemNo and legacy AnimElem bindings;
ChangeAnim compiles value/elem expressions. Missing actions warn and retain the
last animation. Animation clocks freeze in hitpause. Animation ownership and
ChangeAnim2 remain pending.

An authored 2/3/4-tick action with LoopStart at element 2 runs in the supplied
engine. Twenty-four ticks / 48 encoded numeric observations were captured in
four two-line clipboard pages, using a neutral CMD state. Exact bitmap masks
read the numbers; screenshots were inspected and remain untracked. The trace
demonstrates initial AnimTime=-9, zero on the first displayed tick after wrapping,
then -6 on the next tick. Element elapsed time retains the old end value on that
boundary before resuming the loop clock. Both previous Go clocks were wrong.

The authored fixture, hash-pinned numeric observations and a repeatable capture
tool live under tests/. It uses a disposable distribution copy/private Wine
prefix, captures only its own process's window, and does not send input or
change focus. Full regression/race checks pass. Clock tests compare every
recorded element/time value; other tests cover pause, expression events, element
selection, infinite/zero schedules and query bounds. Nonzero AnimElemNo offsets,
zero/infinite edge cases and initial-element overrides still need reference
probes; their tests presently follow the documentation.

The stock player inventory now loads both characters: 281 sprites, 117 actions
and 108 states each. 105 states compile; three per character still fail explicitly
on the missing HitOverride. The test pins this known gap and fails on unexpected
compilation gaps. This is not a complete stock-compilation/playability gate.

## M3b — Font v2 bitmap banks and TrueType

Font loading now accepts FNT v2 definitions, palette and RGBA sprite banks,
invalid-bank fallback, ASCII bitmap restrictions and glyph offsets. Rendering
retains true-color/alpha glyphs and the default indexed palette instead of
turning them into empty paletted images. Added CPU glyph access and bank-aware
metrics. TrueType/OpenType/TTC uses x/image, supports UTF-8, optional antialiasing,
local/configured/system lookup and an observable Go Regular fallback. That
fallback does not reproduce Windows fonts or supply full CJK coverage.

All 19 runtime font definitions/binaries load, with numeric glyphs and metrics.
Bank/color/alpha, variable and missing-character widths, UTF-8, blend modes,
case-insensitive local lookup, missing/corrupt metadata and wrapped binary
offsets have regressions. Stock f-6x8f's last cell extends into row padding:
pixels clip to the PCX while its six-pixel advance remains. V1 bank count now
uses 256/colors instead of colors. Full regression/race tests pass; exact
reference font metrics, rasterization and placement remain unverified.

Dependency changes are confined to `mugen.mod`/`mugen.sum`, selected by the
launcher with `-modfile`; direct Go commands need that option. This avoids root
module writes. Root go.mod/go.sum hashes were checked unchanged. BUILD.md
records versions, invocation, lookup/fallback and the overlay maintenance cost.

## M3a — SFF v2/v2.01 decoding and sprite reference comparison

Added CPU-only SFF v2/v2.01 decoding for raw indexed/RGB/RGBA, RLE8, RLE5,
LZ5 and PNG8/24/32; ldata/tdata, signed axes, palette/sprite links and palette
identity are retained. Indexed images expose masked and opaque views; true-color
images preserve alpha. Optional palette selection shares immutable pixels and
does not mutate the archive. Linked/cyclic/out-of-range data and malformed
packets fail with context instead of producing empty sprites. Hardened v1 chain,
count, link and PCX validation, and retained transparent color RGB for opaque use.

Validation: all **1376 sprites across all 27 stock SFF archives** decode with
nonempty masked/opaque images. Every one of **281 KFM sprites** matches the
opaque RGBA pixel hash exported by the bundled SFF2PNG v0.2 tool. Hashes and axes
are recorded in `tests/testdata/kfm-sprites.json`; reference art stays untracked.
The bundled SprMaker 2.01beta encoded small original fixtures covering RLE5,
PNG8/24/32 and raw RGB/RGBA absent from the stock corpus. Pixel formulas, input
generator and definition accompany that fixture in `sff/testdata/`.

Full regression/race tests pass. A three-second packet fuzz run completed 121863
executions without failure. Focused packet/link/malformed-PCX tests cover
overlapping/recycled LZ5 references, masking and independent linked metadata.
This closes the initial v2 load blocker, not M3: selected character palettes,
font v2/TTF, sound/path hardening and AIR/render behavior remain unfinished.

## M2c — Compiled core controller parameters

Core movement, variable/range/parent-variable, transition, life/power/control,
angle/multiplier, HitAdd, HitVelSet, VictoryQuote, PosFreeze and RemoveExplod
parameters now compile once and evaluate as typed expressions. Missing required
parameters and malformed expressions fail before any state controller runs.
Bottom numeric parameters produce contextual runtime errors instead of silently
becoming zero. That strict runtime failure policy still needs executable probes.

The bundled CNS warning guidance permits float-to-integer truncation with a
warning. Corrected entry attributes, assignment indices, trigger integer
arguments and redirect IDs to follow that rule; integer-only operators remain
strict. Parent variable expressions evaluate in the helper while writes target
the parent. VarRangeSet evaluates its value once and supports float ranges.
Explicit transition overrides retain presence separately from negative values.
HitVelSet uses enable flags rather than multiplying velocity; the incoming-hit
x-velocity convention remains a combat/reference comparison obligation.

HitAdd now has a separate combo counter, with match-owned credit/reset still
pending. VictoryQuote supports its optional random-selection default, range and
helper restriction; the result screen does not consume it yet. SelfState state
ownership is still incomplete. Many other controllers retain legacy handlers.

Validation: full MUGEN tests pass. Added behavior regressions for expression
side-effect order, int32 overflow, float32 variable arithmetic, parent context,
range defaults/evaluate-once, movement omission/facing, defence/kill handling,
transition overrides, optional defaults and compile/runtime failures. These
expectations are documentation-backed; executable semantic parity is not claimed.

## Reviewed baseline

Assessment, plan, compatibility inventory, and working instructions captured
before engine changes. The original full test target fails with duplicate main
functions; the 11 aggregate parser/config/asset tests pass using external data.
Reference behavior has not yet been measured. The original distribution remains
untouched and is excluded from commits along with temporary build artifacts.

## M0a — Reproducible build and pinned reference baseline

Moved the console physics application into its own package, resolving the
duplicate-main build failure. Existing corpus tests now require the supplied
1.1 beta distribution, without external data paths or skip-on-missing behavior.
Pinned 109 reference content/spec/tool files by size and SHA-256; assets remain
untracked. Added UTF-8 BOM handling for the supplied font definitions and a
regression test. Configuration tests can disable ambient YAML lookup explicitly.
Diagnostics now default to stderr and accept a caller-owned writer instead of
creating a fragile working-directory-relative log file.

The launcher runs entire packages, adds `--check`, keeps build/test temporaries
under `temp/`, and holds the root module read-only. `tests/README.md` describes
the fixture setup and current Ebitengine/display coupling.

Validation: `bash -n game/mugen/mugen` and
`GOPROXY=off game/mugen/mugen --check` pass. The required reference manifest is
verified during the suite, and all 33 stock DEF files parse. No state-machine
compatibility claim is made by this checkpoint.

Reference experiment: an isolated Win32 Wine prefix can start the executable
far enough to write its status log. `mugen.exe -h` did not exit within 40 seconds;
it is not a successful gameplay verification. The log identifies the supplied
binary as **1.1.0 Beta 1 P1 (2013.08.11)**, while the readme labels the distribution
1.1 beta 1 (2013-07-27). Use the supplied, pinned executable as the oracle and
record this distinction in future comparisons. Quick-match startup is being
investigated. Headless semantic fixture execution and strict controller/trigger
diagnostics remain M0/M1/M2 work.

## M1a — Preserve expression source through container parsing

Replaced token reconstruction with a line-oriented container parser retaining
raw expressions and line/column locations. CNS/CMD now consume the preserved
text; Statedef entry expressions are retained separately for runtime compilation.
Added explicit tests for operators, quoted semicolons/commas, nested calls and
intervals, numeric spelling, Windows paths, duplicate triggers, AIR rows, select
options, malformed sections/strings, and reader failures. Required old corpus
checks and all 33 stock DEF files still pass.

Validation: new regressions demonstrated failures with the original parser;
`GOPROXY=off game/mugen/mugen --check` passes after the replacement. This fixes
source fidelity, not expression arithmetic or state execution. Those are next.

The isolated Wine quick match reached both game-loop and match-loop initialization
with the stock character/stage/assets loaded. See REFERENCE.md for the observed
evidence and limits; no Go/reference semantic comparison is claimed yet.

## M1b / M0b foundation — Typed language and headless expression replay

Added distinct int32/float32/bottom values, numeric promotion, integer division,
strict integer bitwise/modulo operations, bounded exponentiation, assignments,
all logical operators, math calls/constants, and bottom propagation. IfElse
evaluates all arguments; Cond evaluates its selected branch. Replaced the
process-global custom evaluator with environment-local optional interfaces for
trigger bindings, redirects, animation queries, and warnings. Existing game
call sites use a numeric facade; preserving bottom through controller parameter
application is still M2 work.

The expression parser now rejects unknown characters, malformed numbers/strings,
invalid assignments/intervals, and excessive nesting instead of silently dropping
tokens. It preserves literal types, implements target operator precedence and
legacy nullary clauses, and parses redirects of a single trigger. The stock
corpus revealed `AnimElem = 3, -1`; the signed legacy argument now has a
regression. All 1522 stock trigger lines parse. This is syntax evidence, not
verification of their world-bound behavior.

Added `internal/exprfixture` and the `tests/expressions` JSONL runner. Six fixtures
produce 15 deterministic observations with kind/value/warnings/variables; typed
results and side effects are asserted. Fixture errors and mismatches exit
nonzero. It uses the production evaluator and works without DISPLAY.

Validation: the full `mugen --check` suite passes. Display-independent race tests
pass for evaluator, parsers, CNS, and the expression runner. A three-second
single-worker expression fuzz run completed 108219 executions without failure.
The mathematical/precedence expectations come from bundled CNS/trigger docs;
numeric edge cases and legacy/redirect behavior still need executable reference
probes. No semantic inventory entry is yet promoted to reference-Verified.

Remaining: complete constant/GetHitVar/world trigger bindings; compile/validate
controller parameters and reject invalid trigger groups; correct state entry,
persistence, pause and ownership; extract a headless state/match core. M1 is not
declared complete by this checkpoint.

## M2a — Headless state executor and compiled trigger groups

Moved the shared executor/handlers into `engine/`, retaining public aliases in
`character/`. No Ebitengine/input/render dependency enters the new core. Trigger
programs compile once from immutable source; synchronized caches allow distinct
hosts to share programs without sharing variables/persistence.

Unknown controller types, malformed expressions, missing trigger1 and invalid
persistent/ignorehitpause literals now fail before executing any controller in
the state, with state/controller/name/type/line context. Compilation errors stay
errors on repeated execution. Added the explicit Null controller. Triggerall and
numbered groups execute in documented order, stop at number gaps, and preserve
repeated-condition AND semantics. Bottom is false at the trigger boundary.
At this checkpoint N counted successful conditions and one-shot triggers kept
evaluating. **M2e supersedes both behaviors after executable probes disproved
them.** Positive one-shot state re-entry reset remains supported.

Validation: headless `go test -race ./game/mugen/engine` passes. Regressions cover
trigger ordering/gaps/AND/OR, invalid-state atomic validation, persistence across
intermittent conditions/re-entry, hitpause filtering, self-transition abortion,
Null, and eight concurrent hosts sharing one compiled state.

Remaining: production Character.Update still skips all paused state passes;
state entry is still applied in two places, and handlers still need typed
parameter schemas. These are next, followed by world/entity ownership and the
full state runner. This checkpoint does not declare M2 complete.

## M2b — Single state entry and production hitpause/timer integration

Hitpause/shake clocks and release boundaries below are superseded by the native
M4i frame traces. Entry precedence is separately corrected by M2g.

Added compiled entry expressions and an explicit EnterState operation. CNS
defaults now match the documented S/I/N entry defaults and omission sentinels.
U preserves prior enums; omitted control/animation/velocity/priority/juggle stay
unchanged. Entry applies power, juggle, facing and hitdef/movehit/hitcount
persistence, and keeps transition control/animation overrides through Time=0
passes. Entry never runs implicitly from ExecuteState. Character initial entry
and transitions use this same path.

Production state passes now run during hitpause so ignorehitpause can work,
including the last paused tick; hit-stun decrements once per active tick.
Contact counters persist and advance on unpaused ticks, with new-contact serials
preventing immediate extra increments. MoveHitReset clears status rather than
rearming already-hit targets. PosFreeze resets each active tick; Gravity adds
acceleration once per activation. Hitboxes require an active HitDef, and default
state entry disables stale hit definitions. Incoming hits no longer mark the
defender's own attack as contacting.

State/entry failures stop the affected Character and are returned by
Player.Update; the combat application propagates them. Expression warnings are
recorded with player/state context, deduplicated and bounded. Empty states are
rejected. The old silent transition-loop cutoff now records an error; its
10-transition cap is provisional and needs comparison with the reference limit.

Validation: full `mugen --check` and focused engine/character race tests pass.
New regressions cover entry defaults/U/omission/expressions/overrides, tuple
evaluation order, no repeated entry during hitpause, combat persistence flags,
last-paused/first-active ticks, contact counters, one-tick freeze, one-shot
gravity, active-hit gating and stopped execution on missing states.

Reference obligations remain: entry attribute/override evaluation ordering,
negative-state edge cases, animation boundaries, physics/collision phase order
and the transition limit. State ownership and world-level Pause/SuperPause are
still incomplete. Typed controller parameters and animation/constant bindings
are next; these changes are unit/regression-tested, not executable-parity claims.
