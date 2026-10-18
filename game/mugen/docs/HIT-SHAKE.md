# HitDef contact shake (M4bs)

Target: local immutable M.U.G.E.N 1.1 beta 1. This batch adds envshake.time,
envshake.ampl, envshake.phase and envshake.freq schemas and a shared camera
shake consumer. Numeric routing is 68/81; nine literal schemas bring typed
key coverage to 77/81 (95.1%). The four stored-fall fields remain for M4bt.
These are HitDef key counts, not whole-engine completion.

Time and amplitude reject direct float literals; runtime time truncates while
runtime amplitude retains its fraction. Frequency/phase accept floats. Bottom
becomes zero and unknown expressions/extra components reject before state
side effects. Defaults are time 0, frequency 60 and raw author-local amplitude -4.
Absent phase chooses 90 for evaluated frequency >= 90, otherwise 0, without
executing the frequency expression again. The measured order after palette
fields is time, amplitude, phase, frequency, independent of declaration order.
Frequency clamps to at least float32(.001), including explicit zero.

Amplitude converts once from author to receiver coordinates and then into
base 320 camera units. The global clock stores float32 amplitude/phase/frequency.
Offset is -floor(float32(amplitude*sin(phase))+.5) in those units. Rounding
before the final sign produces asymmetric half boundaries: amplitude +2.5 at
phase 90 gives -3, while -2.5 gives +2. Fractional amplitude 4.75 at phase 30 gives -2;
at phase 90.5 it gives -5. A changing phase can move an initially exact half
boundary across the rounding threshold.

Requests install at the shared world frame boundary. The issuing frame consumes
one duration tick without advancing the supplied phase. Time 2.75 truncates 2,
so only the next receiver pass sees the effect. Existing shake advances before
a valid new request replaces it. Nonpositive time, including omitted time,
preserves an active effect. Guard and None contacts do not request shake.
Round reset clears active/pending requests. ScreenPosY includes the current
offset scaled into the observing character's coordinates; CameraPosY remains
the actual camera position. The renderer shifts the complete world surface
and leaves the separately fitted HUD fixed.

Twenty-six hitshake-* programs pin native numeric/default/error/order, clock,
profile and guard/None behavior. Pre-native manifests are
m4bs-prepared-inputs.json, m4bs-prepared-lifetime.json and
m4bs-prepared-runtime-profile.json under ignored temp/. Inputs and numeric
expectations remain unchanged. Seventeen static native render measurements
pin the exact rigid displacement of all 7,935 receiver garment pixels. The
probe also checks prepared P2 resources when mixed-profile fixtures use a
separate opponent definition. Reference art/captures remain uncommitted.
See HANDOFF.md for final validation results.

The standalone EnvShake controller remains a stub with its own schema and
interaction work pending. Broad simultaneous-request/helper/foreign/pause,
zoom/camera-edge and other render-profile combinations are not established by
these controls. Existing alpha/spark, recovery/round/mode/asset gates remain
unwaived. FallEnvShake's stored payload/consumption follows in M4bt.

Validation: 26 Match races 17.581s, full Character 11.362s, core races and related
Match regressions pass. All 17 rigid render comparisons and 18 apps / 19 captures
pass provenance; two fresh native reproductions pass. HANDOFF.md records exact
artifacts, production hashes and the rejected initial profile recipe.
