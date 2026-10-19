# Liedown timer, input and authored recovery (M4cb)

The remaining accepted GetHitVar(recovertime) selector now has an integer
binding and an engine consumer. It is separate from fall.recovertime, which
controls air recovery eligibility. Twenty-one frozen beta1 controls pin 252
scalar observations and 95 CNS/CMD/AIR/stage/input-recipe hashes: ten timer
programs, five normalized input programs and six authored recovery routes.
All exact native originals and numeric goldens remain immutable. Additional
zeros are sentinels, not extra feature claims.

## Measured behavior

State 5110 counts the stored liedown timer down once per active pass, after
CNS, even when its authored state type is S. A custom L/H state numbered 710
does not count it down or invent a get-up transition. With data.liedown.time=5,
5110's first CNS reads 5; later Time=1/2/4 reads 4/3/1. The next pass sees zero
and enters 5120. Default stock 60, explicit 0 and negative -3 are separately
measured. Zero/negative waits through Time=0, then gets up at Time=1. Those
values no longer fall back to 60. State 5120's I entry restores the configured
counter before its first CNS. A deliberately I-authored 5110 resets the counter
each pass before decrementing and remains at 4; it does not time out like H.
A dead receiver does not get up when its timer expires. A seven-tick Pause
freezes progression and preserves the active-pass deadline in the control.
The existing animation-complete dead transition to 5150 is retained.

Every measured physical input press AND release reduces the positive remaining
timer to floor(3*remaining/4). Ordinary ticking still subtracts one. All six
attack buttons, directions and Start have controls; holding a key only produces
the press/release reductions. Inputs are sampled before directional latching,
and horizontal masks use physical direction across character facing.
Five normalized programs verify the exact integer arithmetic and accelerated
pass counts (2,10,6,6,12). They avoid absolute wall-clock timing as an assertion.
Their 18 pulses generate 36 balanced events on owned native windows. The six
route controls add 12 balanced events. No global focus or keyboard injection is
used. Simultaneous multi-key edges, rapid changes within a simulation tick,
turns while held, paused-input buffering and gamepads remain interaction gates.

The old global recovery shortcut could move arbitrary L/H states to 5200 and
arbitrary A/H states to 5210/5040 merely from a named command. Beta1 custom
controls stay in their authored 5020 for both air and lying state types. That
shortcut is removed. The actual bundled 5050 CNS selects 5200 near the ground,
5210 farther up, and neither while rising below its velocity threshold or when
fall.recover is disabled. These controls execute the unchanged native 5050
source with a fixture position freeze and stable destination markers. Marker
5210 uses I; CanRecover remains true from the retained eligible fall data.
Eligibility therefore no longer acquires an extra MoveType=H restriction.
The old Go-only recovery adapter test now asserts eligibility without inventing
an unprovided transition. Stock common CNS continues to own recovery physics.

## Reproduction and provenance

Numeric timers use reference_probe.py with --fixture-constants --bank fvar
--pages 0 --clipboard-in-cmd --mugenversion 1.1 --fixture-stage --stage probe-stage.
Owned input recipes use tests/recovery_probe.py --fixture NAME --name FRESH,
which compares exact recorded values by default. --record is for new frozen
originals. Both tools prepare only disposable reference copies. Input recipes
are .input.json; the production Match replay verifies their hashes and supplies
the equivalent logical pulses through Character.InputSource.

Run `PYTHONDONTWRITEBYTECODE=1 python3 game/mugen/tests/recovery_evidence.py`
from the repository root. It audits all 34 M4ca/M4cb programs / 372 values /
three target rejections, authored/prepared CNS/CMD/AIR/stage/config/executable,
exact PNG glyph masks and balanced owned events. Only native FirstRun startup
persistence and line endings are normalized; font/number tolerances are absent.
Raw mashing/delta measurements remain diagnostic under ignored
 temp/m4cb-raw-input-diagnostics and temp/recovery-input-runs. Their absolute
remaining times vary with window startup, so normalized replacements have new
names and retain the original controls unchanged. Multi-key diagnostic timing
is not used to claim simultaneous-input parity.

An additional frozen omitted-liedown.time control is rejected by beta1 with
"Can't load Data constants". Its original source/capture stays diagnostic.
Required Data-constant load validation remains an asset/schema gate; this
checkpoint describes valid-character runtime behavior and explicit zero/negative
values. Broader required-constant validation must compare the complete Data
schema rather than infer a new omission default from the executable rejection.

## Validation boundary and remaining work

Focused Match races PASS 102.887s, covering new metadata/timer/input/routes,
existing chains, fall recovery and real stock sweep recovery. Full Character /
evaluator / engine / CNS races PASS 12.697 / 1.022 / 1.497 / 1.013s. The first
input replay attempt collected recipe sidecars as programs; the shared harness
now excludes .input.json and validates them as hashed recipes instead. The
corrected programs pass. Earlier logs are diagnostic, never acceptance.
All runtime/test/probe source is frozen in temp/m4cb-final-inputs.json (470
hashes) before final full scoped races and production application validation.
Final scoped races PASS: 2,774 test/subtest results across 24 tested and 12
compiled-only packages, no individual skips/failures. Match 1,009.698s; full
command 1,083.286s. All 470 frozen hashes pass. Fresh native Pause/taps/far-route reproductions and a four-capture stock
win/rematch/win replay with private PCM also pass; the final three app images
were inspected. Source/configuration/definition/executable/PNG/event/PCM/module
audits pass. All clients and sessions are closed. Exact results and artifacts
are recorded in HANDOFF.md.

All 34 beta-accepted documented GetHitVar selectors now have runtime routes;
the manual-only hitid and zoff are rejected as beta1 does. This is a selector
routing inventory, not exhaustive behavioral parity. Helper/foreign/projectile/
team contacts, broad pause/recovery physics interactions, extreme integer inputs
and all other controller/trigger families remain separate gates. Eighteen
trigger families are still unbound. Renderer alpha/spark, fonts/platforms,
projectile/reversal lifecycle, modes/AI, assets and BGM remain open.
