# Stored fall shake (M4bt)

Target: immutable local M.U.G.E.N 1.1 beta 1. The final four allocated keys are
fall.envshake.time, fall.envshake.ampl, fall.envshake.phase and fall.envshake.freq.
They use the contact-shake typed schema: strict integer literals for time and
amplitude, runtime integer time/floating amplitude, floating frequency/phase,
bottom zero and atomic rejection of unknown expressions or malformed tuples.
Defaults after a hit are time 0, raw author amplitude -4, frequency 60 and phase 0;
missing phase instead becomes 90 when evaluated frequency >= 90. Frequency clamps
to float32(0.001). Default phase does not repeat frequency side effects.

The joint tagged control pins all contact fields before all stored-fall fields,
with each family ordered time, amplitude, phase, frequency, after the palette
fields. Source declaration order does not change this sequence.

Contact stores the payload in receiver coordinates, scaling amplitude exactly
once. A mixed author 640 / receiver 320 hit stores default amplitude -2. Guard
contacts clear all four stored fields to zero. GetHitVar(fall.envshake.time)
and ampl return int32; freq and phase return float32. Runtime amplitude 4.75
therefore reads 4 through the integer getter while remaining 4.75 for rendering.

FallEnvShake submits the stored payload to the same shared global shake clock
and consumes a positive duration to zero. It retains amplitude/frequency/phase,
so a later query still returns them. Zero/negative durations are ignored and
remain unchanged. The controller works even if the hit has fall=0. These
observations take precedence over the manual's nonzero-duration wording.
The common-state controller now produces an actual shake. Contact and fall
requests share the rounding, issuing-frame duration, coordinate conversion,
ScreenPosY, world rendering and round reset documented in [HIT-SHAKE.md](HIT-SHAKE.md).
Standalone EnvShake remains unfinished and is not promoted by this consumer.

Twenty-two fallshake-* native programs pin 336 observations, three load
rejections and 88 pre-native CNS/AIR/CMD/stage hashes. Manifests in ignored temp/:
m4bt-prepared-inputs.json, m4bt-prepared-lifetime.json and
m4bt-prepared-joint.json. Sixteen static render controls pin exact rigid offsets
of all 7,935 garment pixels, including finite expiry, negative duration, fall=0,
guard, mixed profiles and fractional physical amplitude. Native source inputs,
numeric values and tolerances were never changed or derived from Go.

All 81 documented HitDef keys now have typed routing: 72 numeric and nine literal
schemas. [HITDEF-KEYS.json](HITDEF-KEYS.json) records the source audit against the
pinned local manual. This completes the user-authorized 27-key allocation; it
does not establish all HitDef interaction combinations or whole-engine parity.
Broad simultaneous-request/helper/foreign/projectile/team/pause/camera cases,
full recovery/round clocks, standalone effects, modes/AI/BGM/assets/fonts and
existing strict alpha/spark failures remain open. Final integration/native/app
results are recorded in HANDOFF.md.

Final scoped race checks pass, including Match in 942.741s and Character in
11.918s. All 48 focused contact/fall Match controls pass in 32.032s; core/query/
controller checks and two fresh native reproductions pass. All 16 exact rigid
render comparisons and 17 apps/18 captures pass provenance. Representative fall
and stock captures were visually inspected. HANDOFF.md records frozen production
hashes, the binary digest and final artifact paths.
