# Initial camera publication

The four `bootcamera-*` programs independently measure camera queries without
creating any effect. They save both roots' boot-state observations, their first
two negative-state passes, and the corresponding `ScreenPos X`, `Pos X` and
`GameTime`. All 96 original numeric fields and eight native captures were
reviewed before promotion or production changes. Original sources, screenshots,
configs and expectations remain unchanged.

| Character width | Stage start X | Both boot cameras, GameTime 0 | P1 camera, GameTime 1 | P2 camera, GameTime 1 | Both cameras, GameTime 2 |
| --- | --- | --- | --- | --- | --- |
| 320 | 40 | 10 | 10 | 40 | 40 |
| 320 | -80 | -20 | -20 | -80 | -80 |
| 640 | 80 | 40 | 40 | 160 | 160 |
| 1280 | 80 | 80 | 80 | 320 | 320 |

These controls use the stock 1280-wide game space and a 320-wide stage. During
boot the beta publishes the stage's raw X start as a game-space value, then
converts that publication into the observer's character units. It publishes the
converted stage camera after the first root's GameTime-1 pass. This is an entity
publication boundary, rather than an inferred fixed-speed camera movement.
Earlier `duststartup`/`startupunit` diagnostics also establish the first/second
pass distinction; they remain raw and are not counted as dust coverage.

The implementation keeps the converted camera geometry and publishes the raw X
through the initial GameTime-0 update. Existing per-entity camera updates then
publish the converted X during the following update. The change is restricted
to the first round's initial update. Later-round/reset startup and vertical
publication require their own controls. No effect-specific coordinate mode,
render tolerance or reference expectation was introduced.

A first revision published too soon between booting the two roots. Its full
failure record is retained. The second revision matches all 24 camera fields
and 94/96 total fields, and fixes the three original early dust camera query
failures. The negative-start control retains two strict failures: P2's first
`ScreenPos X` is 360 instead of 305, and `Pos X` is 200 instead of 145. The native
actor is already at the right screen margin. This exposes a separate constraint
publication boundary; its cause and correction have not been inferred from the
camera result. The entire four-program suite therefore remains strict-failing.

## Native frame controls

The new `frame_step_reference_probe.py` preserves fixture-authored clipboard
output and verifies ownership for every key. Independent calibration holds
GameTime/state time/counter at 329, then advances exactly to 330, 332 and 335 with
one, two and three Scroll Lock presses while debug-paused. A two-frame invisible/
visible animation follows the same parity. All six frames and 16 balanced events
were reviewed. This validates an observer, not a controller implementation.

`frame_sequence_reference_probe.py` then shows that F4 resumes the native debug
pause. Requested one/one/two steps after F4 instead capture cumulative GameTime
372/398/433. Final Pause stops at 443. State time resets, while GameTime remains
cumulative. These images are not exact boot-step evidence. The complete native
finalization and separate guard are retained; its outer execution receipt was
truncated and no outer exit status is claimed.

The first two independent calibration intent attempts incorrectly assumed the
omitted AIR loop marker was 0 and then 1. The actual parser sentinel is -1; the
third complete intent passed. All failed helpers and original evidence remain.
Calibration's accidental live authoring-context hashes have a bounded archive:
361 tracked paths verified against Git a868f654 and private snapshots, and the
untracked root go.sum verified against the existing upstream module baseline and
its snapshot. The remaining 123 native-used paths stay original live-path checks.
Subsequent native maps hash the authoring-map JSON instead of mutable Go context.

## Validation status

M4do final session44675 completed and was awaited with exit1. The separate elevated
21,349-input/client guard passes. All185 production Go files are byte-identical
through the final run and passive audit. Focused core races pass890 results;
selected native regressions pass482 test/subtest results and fail two (the one
negative-start case and its parent). This is458/459 programs; all166 dust,138 Game,
53 pool and98 older camera/boundary/width programs pass.

All16 actual applications run with binary
`96a0047f577b9ce874799d94af0770611be7ab2143a1c7e1a8e459623c221702`.
The six targeted startup world comparisons remain strict failures. The three
GameTime-1 cases differ93,069/93,114/93,069 pixels, increased from their earlier
77,212/77,139/77,212; three GameTime-4 cases retain73,164 differences. Native
fighters retain earlier displayed positions while Go shows the current positions.
Keep this as a presentation observation; it does not prove that later roots skip
CNS on the issuing frame. The exact producer/query correction is not full-scene
conformance and does not justify a sprite-coordinate workaround.

Nine older palette/order/shake controls pass. Stock win/rematch/win passes82
balanced inputs and1,928,880 stereo44.1kHz PCM frames (SHA
`161e481f993b79458cdcd41f70c6e50e04f9c5e51008ed74c94072e46b938170`).
All15 native/Go pairs, five full-scene sheets and three stock captures are viewed.
The final35 gates preserve eight exit1 results: native, six startup worlds and the
stale inventory. The other27 pass. Exact old inventory generator/report bytes
were archived against Git before regeneration; the current check passes.

Passive audit verifies155 original native paths,109 canonical resources,656
application binary-resource copies,18 original application captures, shared binary,
seeds/recomputed RGB/owned keys/private PCM and source versions. The initial
baseline hash walk accidentally completed after its test; that diagnostic and a
self-matching cancellation guard (exit130) are preserved. A separately named R2
baseline ran against the complete frozen map before the production change.
The latest full scoped suite remains historical M4dl:5,028 results/36packages.
Older HUD, paused-pose, raster and ordinary KO-echo failures remain unwaived.

## Next independent presentation controls

Before correcting the negative-start constraint timing, distinguish both screen
edges with boot placements outside the initial screen, and compare ordinary
bounds against explicit ScreenBound controls. Save boot and first-pass camera,
position, screen position and state time. This must separate a publication change
from actual movement or a stale displayed pose.

For HUD expiry, a new authored fixture can trigger a deterministic self-KO at
GameTime 400, disable KO slowdown and retain the over phase. Pause the native
engine after warmup, seek to 399 using the calibrated step count, then capture
400 through the finite display boundary one tick at a time. Original clipboard
formatting must remain authored by the fixture. Use an independently hashed
fight.def sidecar and an existing positive stock sprite; introduce no binary.
Measure omitted/zero/positive/negative displaytime for sprite, font and finite,
looped and infinite animations; delayed starts; Pause/SuperPause. Validate the
KO issuing frame and RoundState independently. These are proposed controls, not
observed behavior or permission to implement from the manual alone.
