# Environment shake — M4cv

EnvShake now reaches typed compilation, the global shake timeline, authored
coordinate conversion and production world drawing. This is a bounded consumer
checkpoint. Full framebuffer parity remains unproven; strict failures below
stay visible and the original native inputs and expectations remain unchanged.

## Native controller behavior

Sixty-six programs load and pin 792 numeric readings; six reject. The beta differs
from its manual in several cases.

| Parameter | Measured behavior |
| --- | --- |
| time | Omission defaults to zero. Integer literals required; runtime fractions truncate. |
| ampl | Omission defaults to raw -4 author units, including profiles640/1280. Float literals reject, but runtime fractions persist as float32. |
| freq | Defaults to60. Absolute magnitude; exact zero becomes0.001. Tiny nonzero magnitudes, including±0.0001, survive. |
| phase | Defaults to90 when the normalized frequency is at least90, otherwise0. Negative explicit phases persist. |

Evaluation is time -> ampl -> phase -> freq (tag1234), regardless of declaration
order, including zero/negative durations. An omitted phase uses the evaluated
frequency without evaluating it again. Unknown triggers, excess tuple elements
and invalid literals reject before any controller in the state has effects.
Missing redirected values become zero.

Amplitude converts from the current state's owner coordinates to the shared
320-wide camera units once. The native displacement is
`-floor(float32(amplitude*sin(phase))+0.5)`: +2.5 rounds to-3, -2.5 to+2.
The profile tests distinguish retaining runtime fractions from truncating them.
Helpers use the shared timeline. Foreign-state and other ownership combinations
remain interaction variants rather than an engine-wide compatibility claim.

Requests are deferred until the frame boundary. A ScreenPos getter in the same
CNS pass still sees the previous shake. Positive duration consumes its issuing
tick without advancing its initial phase. Thus time1 queues an already-expired
replacement and can cancel the current shake at the boundary. A standalone
zero/negative/missing-time request clears an earlier pending request in the
same frame but preserves a shake that was already active. This policy is
separate from contact/fall requests. Global phase continues through CNS Pause
and SuperPause in the measured six-frame traces.

## Drawing and native composition

World rendering moves, while HUD rendering remains fixed. Ordinary shaking
exposes raw black at the viewport edges, including diagnostic pink stages.
Positive EnvColor remains fixed across those gaps. With AllPalFX, exposed black
receives the final viewport transform (138,225,255 in the complex calibration),
while debug pink inside the world receives its measured source/final path
(99,90,248). Negative EnvColor plus AllPalFX yields138,225,255 inside and in the
gap. Positive EnvColor12,34,56 stays fixed; with AllPalFX it becomes59,35,111.
Outer letterboxes stay black. A one-pixel GPU backdrop implements these measured
branches without changing the ordinary black-gap behavior.

The profile scenes also exposed an independent HUD omission: animated fight
components discarded their AIR blend fields. HUD components now forward those
fields through the shared ApplyBlend and palette/sampler consumers. The stock
AS208D48 portrait panel therefore blends with the stage instead of drawing
opaque gray. Wider HUD layering, destination-factor composition on intermediate
surfaces and framebuffer rounding remain explicit variants.

## Evidence and strict limits

Native screenshots select45 bounded static comparisons, each a393,040-pixel crop
and39 actor/stage/HUD/viewport/letterbox samples (1,755 samples total). Twenty-one
phase-dependent scenes are numeric diagnostics only: capture timing can change
their quantized displacement. That selection was frozen from native observations
before Go implementation, never chosen from a passing Go result.

The first matrix passes38/45 strict scenes. Seven640/1280-coordinate profiles
fail, exposing the HUD alpha omission and existing fractional viewport rounding.
Revision2 fixes the alpha omission and completed the entire45-scene matrix:
38 exact crops (14,935,520 RGB pixels),1,727/1,755 exact samples. Seven profile
reports retain252 or876 differing world pixels (up to two channel steps) and
four sampled HUD colors with one-channel precision differences each. They are
**unwaived failures**. Do not report45 exact applications or full RGB parity.

A separately named native1280x960 viewport calibration preserves a clipped947px
capture; its numeric clipboard extraction did not complete. It is diagnostic,
not promoted accepted evidence. Native-only CPU interpolation experiments do
not reproduce all original720px rounding and justify no sampler or expectation
change. Original captures, crops and strict probe statuses remain untouched.
The exact retained failures are:

| Fixture suffix after envshake- | World pixels differing | HUD sample errors |
| --- | ---: | ---: |
| extra-profile-1280-runtime | 252 | 4 |
| extra-profile-640-runtime | 876 | 4 |
| profile-1280-explicit-1 | 252 | 4 |
| profile-1280-explicit-4 | 252 | 4 |
| profile-1280 | 252 | 4 |
| profile-640-explicit-4 | 876 | 4 |
| profile-640 | 876 | 4 |

The profile1280 explicit-amplitude1 control quantizes to zero displacement yet
retains the same profile precision error. It separates the framebuffer gate
from an incorrectly moving camera. Native captures and strict comparisons stay
unchanged; overall45-scene RGB conformance is false.

The pre-existing12 M4cq fractional/nonquarter raster counterexamples also remain
unwaived. Final viewport/platform conformance remains in the broader renderer
allocation; controller-specific failures stay tracked here rather than hidden.

Selected native shake/environment/global/order replay passes201.549s. Ten
EnvColor/contact/fall regression apps pass. Stockwin/rematch/win is visually
inspected, with82balanced owned events and1,975,603private PCM frames.
Match1736.103s;packageelapsedtotal1820.896s. All14,998frozen inputs verified at
completion. The strict image matrix remains FALSE for the seven explicitly
preserved profiles above.

The read-only `tests/env_shake_render_probe.py` verifies authored/native source,
executable/configuration/assets/capture hashes, every crop pixel and every
sample. It cannot author expectations. Batch drivers collect every failure and
return nonzero whenever a strict case fails.

Remaining variants include reset/round/KO-slowdown, more mixed coordinate/state
owners, dynamic image captures and full framebuffer precision. M4cw subsequently captures12 independent contact/fall frequency controls and
corrects the shared parser's minimum clamp to magnitude/zero-only normalization.
See [SHAKE-FREQUENCY.md](SHAKE-FREQUENCY.md). No previous HitDef/Fall/EnvShake
fixture or expected result was changed.

Native resources, captures, goldens and frozen inputs were excluded.
