# SuperPause behavior and native evidence

M4v connects typed SuperPause arguments to the shared Timeline, combat, power,
effect scene, sound queue and production drawing. Thirty-eight native programs
pin 600 numeric observations, including M4u's adopted superpause-three pilot and
one ordinary Pause contact comparison. Ten programs additionally pin static
render measurements; three pin seven audio segments with seven Go PCM checks.
All packets run through production Character. This is measured partial coverage,
not a whole-controller compatibility claim.

## Clocks and combat

The issuing tick completes. Omitted time is 30; negative time becomes zero.
Invalid movetime below zero or above time becomes zero. SuperPause preempts an
ordinary Pause without consuming its remaining duration or caller allowance.
A Pause requested during SuperPause waits. Requests on the same birth frame
also retain their separate durations. Poweradd changes and clamps power on the
issuing tick. Shared GameTime continues while local CNS/physics/clocks freeze.

Unhittable protects the super caller while the super clock is active. With the
flag off, the static attack box of a frozen opponent can hit the moving caller
at GameTime 1; with it on, the first hit waits until GameTime 4 in the three-tick
probe. This corrects M4u's unmeasured frozen-attacker contact exclusion.

A frozen receiver enters state 5000 on contact, before its CNS can run. Its life
remains unchanged until resumption: at relative GameTime 4 the first resumed
Time=0 pass sees GetHitVar(damage)=100 and Life=1000; at GameTime 5 it sees zero
pending damage and Life=900. Two contacts accumulate 200/800 instead. Reissuing
HitDef rearms its target contact guard. MoveHit/MoveContact are one on the contact
CNS pass and advance at active tick completion; existing hitpause traces also
remain passing.

P2defmul applies to already-acquired hit targets still in MoveType H, rather
than every opponent. A hit before acquisition is unaffected. Positive factors
stack: 2 then 2 reduces a later 100 damage to 25. Zero and negative values use
Rules.Super.TargetDefenceMul, stock 1.5, giving rounded damage 67. Leaving H
clears the target's super defence adjustment. The implementation retains a
contact/generation ledger for these cases; this does not replace the still-open
world registry, HitDef IDs or complete Target controller ownership model.
Complete damage/base-stat/kill/rounding schemas remain open.

## Effect, tint and audio

Own S-prefixed effects use the caller's AIR/SFF/palette and local-width scale;
unprefixed/F selects common fight effects. Position is sampled once at birth,
relative to facing, and does not follow later PosSet. The original 8x4 RGB marker
has native solid core X=505..529, Y=450..462 at pos=(25,-50) in the calibrated
1280x720 scene. An AIR offset of 20 advances it to X=565. A finite action retires
after one pass; an explicit -1 frame survives SuperPause ending and a later
SuperPause with anim=-1. Super effects animate through both ordinary and super
pauses, including when their caller is frozen. Other effect families' exact
pause/hitpause clocks remain open.

Static native samples support scene RGB factors (.3125,.3125,.5), opponent
factors (.5,.5,.75), and unchanged caller/super-effect colors. Tests permit one
byte for this inference. Actual Go windows confirm the distinct drawing routes,
normal effect color and measured anchor. Background filtering, full framebuffer
and every layer/palette/effect interaction are not established by these samples.

Own/common sound prefixes and a group-only sound select the intended original
tone bank. Super sound uses an automatic channel, preserves existing numbered
channel 0 voices and continues streaming during the pause. Native tone levels
pin screen-X-from-left panning, like contact sounds. At caller X=-70 the 1323-Hz
own tone measures approximately L=3092, R=4873; the distinct common tone uses
2646 Hz. Existing voice tones persist until their authored StopSnd. Audio
tolerance remains eight signed-16 units and relative onset tolerance .35 s;
absolute device/startup latency is not compared.

## Reproduction and limits

Use the existing private display/Wine/audio setup from BUILD.md and AUDIO.md.
Run native captures, Go window captures and audio probes sequentially: other
OpenGL windows can occlude captured native surfaces, and other private audio can
mix into the monitor. Earlier overlapping clock captures and *-native audio
drafts are diagnostic only. Promoted sound captures use audio-runs/*-isolated;
the final effect-clock capture is reference-runs/superpause-effect-clock-serial.
The workflow rule is recorded in AGENTS.md.

Fresh effect-clock and other-pause render replays, plus an isolated sound-own
replay, match. tests/super_render_probe.py retains source/AIR/original SFF/
executable hashes and checks the solid marker core/static samples. Its geometry
is calibrated only for these zero-camera 320-wide KFM scenes. The new optional
reference_probe.py --clipboard-delay=0 allows observations while P1 stays frozen;
default timing remains 30. Owned Go captures are app-runs/superpause-*-serial,
with hashes/samples in superpause-inspection.json under temp/.

Full scoped races pass, including a real stock KFM state-3000 super that consumes
1000 power, freezes for 30 ticks, and uses common action 100 and sound 20,0.
Root module files, original distribution and desktop configuration are preserved.
Typed argument expression/default tests do not establish all native evaluation
order, invalid values, version/resource grammar or mixed-coordinate edges.
Input buffering still has authored tests without native input. Full helper/
projectile identity, Target ownership, pause/KO/hitpause overlap and effect
layering remain open.

The same captures expose a separate HUD framing defect: native portraits begin
near X=172/Y=14, while Go begins near X=16/Y=16. Native loads mugen1/fight.def;
this is not a classic-fight fallback. M4q fixed world margins but did not prove
independent HUD fitting. StageFit/SystemFit/KeepAspect configuration and HUD
projection are the next rendering comparison.
