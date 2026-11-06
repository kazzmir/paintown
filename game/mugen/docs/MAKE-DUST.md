# MakeDust — bounded native implementation (M4dn)

MakeDust now has a typed compiler, a real common-resource producer and a shared
round-phase spacing clock. It replaces the last source controller stub. This is
a bounded implementation checkpoint; the strict failures below remain open.

## Schema and evaluation

`spacing` defaults to 3, takes an integer expression and clamps values below 1 to 1.
It evaluates before the shared-clock gate. A skipped request does not evaluate
positions or consume their jitter draws.

The first `pos` uses integer expressions; bare floating literals reject, while
runtime floating results coerce to integers. `pos2` accepts floating expressions.
Either position accepts a single X expression, optionally followed by one trailing
comma, with Y defaulting to 0. Empty values, empty first axes, extra components,
two axes plus a trailing comma, quoted numbers and unknown used expressions reject.
Unused unknown keys are ignored. Supplying `pos2=0,0` creates a second cloud.

Each axis expression precedes its own shared RNG draw. The common-unit jitter is
`raw % 3 - 1`. The first cloud is admitted before the second position expressions;
failed admission still allows those expressions and both second-cloud draws.
Bottom values use the measured zero defaults without skipping later expressions.
The native assignment and Random sentinels are retained in the fixtures.

## Clock and scene consumers

Native spacing uses a shared round clock, rather than controller Time or an
unbroken GameTime modulo. The observed initial-round relationships are:

| Native phase | Spacing counter in the measured programs |
| --- | --- |
| Preintro | GameTime+1 |
| Intro and round call | GameTime−29 |
| Fight call and active play | GameTime−99; active Time+31 |
| KO | Resets at the phase transition |

The Go timeline stores the phase origin. Actual RoundState transitions reset it;
the internal RoundCall and startActive splits do not. Moving-owner probes during
two paused passes prove that spacing keeps advancing under Pause/SuperPause.
The dust animation clock is separate: actual two-frame controls freeze under
Pause, PauseBG0 and SuperPauseBG0, while an unpaused control reaches its second frame.

Positions use the actual actor position, state-owner coordinate scale and facing
for X offsets, then common-unit jitter before integer conversion. The common
sprite itself remains unflipped. This differs from GameMakeAnim's origin/jitter
ordering. Common action 120 is mandatory; an own action 120 does not substitute.

Dust shares ordinary Explod admission, remains absent from NumExplod, survives
its helper's destruction and uses the existing scene lifetime consumer. Missing
action 120 consumes the draws but no slot; an action containing an unavailable
sprite occupies a slot. GameMakeAnim's pool remains independent in both creation
orders. Dust follows AllPalFX, ignores local PalFX/BGPalFX, has no shadow and draws
over an actor at the measured priority tie. Real actors/backgrounds and resource
positives accompany those observations.

## Evidence and results

The corpus has **166 programs:144 accepted and22 expected load rejections**, with
**1,728 numeric observations**. Every original program, parsed intent, native-only
row and image was reviewed before promotion or implementation. Eight native
ledgers and configuration-finalization records accompany the source fixtures.

- **163/166 dust/control replay cases pass (98.2% of this corpus).** Three early
  CameraPos values remain strict failures: Go80 versus native20 at GameTime 1.
- All **138 older GameMakeAnim and53 ordinary-pool programs pass**. The combined
  replay has354/357 passing cases; parent tests make373 passed/5 failed results.
- **890 core race results pass.** All 185 production Go files are byte-identical
  to that validated revision. The latest full scoped suite remains M4dl's
  historical5,028 results/36 packages; no new full-suite pass is claimed here.
- All **144 application scenes run**. **116 are exact (80.6% of this corpus)**,
  covering **59,020,800 exact RGB pixels**. All 144 pairs/24scene sheets and three
  enlarged failure details were reviewed. The 28 failures remain unwaived.
- Five older palette/order applications, four shake masks and reviewed stock
  win→rematch→win pass. Stock input has82 balanced owned events; private stereo
  44.1kHz capture has1,973,590 PCM frames.

The 28 strict world failures comprise seven startup/paused-pose scenes, twelve
held round/fight/KO overlays, two maximum-one RGB cases, one overlapping/bottom
control (654pixels, maximum8), and six ordinary finite-frame controls (360–526
pixels, maximum32). The finite/overlap samples have matching coverage masks in
the enlarged controls; their color cause still needs isolation. Do not change a
sampler, frame clock or tolerance based on that observation alone.

All 23 final gates retain their real exits: native replay and world matrix exit1;
the other 21 gates pass, and the outer run exits1. Separate outer/matrix frozen
source/client guards pass. The evidence audit checks3,584 original paths,166 exact
configurations,149 common-AIR replacements,6,806 native binary-resource copies,
all captures/seeds/actual RGB differences, the shared154-application binary,
owned input and private PCM. Root modules remain unchanged. No binary is committed.

## Scope corrections and remaining work

Twenty-one earlier probes rejected an invalid empty state 5000 setup. Two corrected
continuity observers never reached their capture. They remain unpromoted diagnostics,
not dust coverage. A promotion count caught abbreviated exclusion names before
runtime; mistakenly generated derived expectations/ledgers were preserved privately,
and the valid 19 existing JSON files were verified before corrected ledgers were
written. Original native inputs and captures were never changed.

The old KO warning is invalid state 5080, not 5000. The base own AIR contains 9999/9100;
9996 is a common appendix. New inert5080+Null controls remove that fixture warning
without claiming a real death-pose implementation.

Sixteen additional camera-only Null programs remain raw and are not counted as
dust coverage. They establish initial raw common-unit camera publication, followed
by converted stage coordinates at GameTime 2, across positive/negative starts and
320/640/1280 actor profiles. They also retain native prior actor poses when PosSet
and Pause occur in the same issuing pass. Multi-entity/boot publication and paused
presentation need independent follow-up before changing shared camera/cache code.

HUD inspection found that its clocks already advance during Pause, while drawing
lacks the measured component displaytime/one-pass expiration. Close that consumer,
startup/pause presentation and finite/overlap color controls, then continue clipboard,
variable aliases, StateTypeSet/HitAdd, wider flags and prior strict obligations.
Broader modes/BGM/assets/fonts remain the separate item 4 allocation.

Routing is now **77/89 typed (86.5%)**, with zero source stubs,12 fallbacks,nine
partial consumers, 20 bounded consumers and 60 route-only rows. These are source
counts, not an overall engine completion percentage.
