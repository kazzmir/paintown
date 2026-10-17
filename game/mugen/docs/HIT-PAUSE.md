# HitDef contact pause expressions

M4be completes the bounded `pausetime` / `guard.pausetime` compiler allocation
following the README checkpoint `e2addf06` (reversal point). Existing literal
pause consumers remain shared with production. Nineteen original programs
record 252 numeric observations, four load rejections and 76 authored
CNS/AIR/CMD/stage hashes against M.U.G.E.N 1.1 beta 1.

## Measured schema and defaults

| Input | Native result |
| --- | --- |
| Omitted pausetime | 0,0. |
| Omitted guard.pausetime | Both pausetime source expressions execute again at the guard-pause evaluation point. |
| pausetime = 3 | Attacker 3, receiver 0. |
| pausetime = 7,11; guard.pausetime = 2 | Guard attacker 2, receiver 11; the omitted second component retains its hit-pause source default. |
| guard.pausetime = 3, | Accepted, with the same receiver inheritance. |
| pausetime = ,4 | Rejected at load. |
| pausetime = 1,2,3 | Rejected at load. |
| Literal float in either measured pause field | Rejected: Expected int value and read float. |
| Runtime fvar components 3.9,4.9 | Truncate to 3,4. |
| Missing PlayerID destination in a component | Bottom converts to zero. |
| Negative components -3,-4 | Effective pauses/shake are zero. |

The inherited-guard program starts counters at 7/11 and uses hit-pause sources
`(var(10):=var(10)+1)-6,(var(11):=var(11)+1)-8`. Native guard contact uses 3/5
and leaves counters 9/13. Copying already evaluated hit values 2/4 would lose
both side effects and timing. A one-component guard override similarly retains
the omitted component's source expression rather than its cached integer.

Programs observe an ignorehitpause counter while MoveContact=1 and attacker
Time=4, snapshot receiver hitshaketime at first contact and latch observations
at Time>=8. The counter includes the resume pass: attacker pause7 produces8
observations, pause0 produces1. All normal fixtures retain receiver life to
avoid involving KO/round-end clocks. Source/rejection checks identify the
file, state, controller and parameter through the shared compiler.

## Parameter evaluation order

Forward and reversed declaration programs tag every current expression-routed
field, with both pause components tagged independently. Each records 40
positions across pages0/12/24/28; page overlap retains the current reader's
12-value observation format. Both native programs give identical order.

- `pausetime` executes after GivePower and before NumHits.
- `guard.pausetime` executes after guard.velocity and before
  guard.cornerpush.veloff.
- Each of these pause tuples evaluates **second component before first**.

The numeric tags are committed in
[order-forward](../tests/testdata/reference/hittiming-pause-order-forward.json)
and [order-reverse](../tests/testdata/reference/hittiming-pause-order-reverse.json).
The remaining measured field order is preserved. Source declaration order
does not determine the side effects. Default guard components repeat at their
own position in this order, even when the eventual contact is unguarded.

## Implementation and evidence

`engine/hit_pause.go` compiles versioned tuples with trigger validation and
strict float-literal rejection. It retains per-component defaults and emits
integer cues in the measured order. `engine/hitdef.go` routes both keys and
normalizes bottom/negative values before the existing literal HitDef consumer.
Runtime float conversion uses the common int32 coercion path.

The initial host/atomic-compilation checks fail on the old implementation:
expressions remain raw strings, and invalid pause fields allow a preceding
VarSet to mutate state. The diagnostic is
`temp/m4be-before-pause-compilation.log`. New shared-host and early-validation
checks pass with the implementation; source maps remain unchanged.

All19 Match replays, including four rejected inputs, pass together with M4bd's
four fatal/guard pause programs under race checks:
`temp/m4be-integrated.log` (Match14.466s). Core engine/evaluator/CNS/combat races
pass in `temp/m4be-pure-race.log`. The native source manifest is
`temp/m4be-prepared-inputs.json`; sources/goldens are frozen and their authors
must not be rerun.

Final pipeline `temp/m4be-validation.log` passes: three fresh native default,
full reverse-order and float-rejection reproductions; three production apps /
four captures; and one full scoped serial-package race suite. The full log is
`temp/m4be-full-race.log`: Match607.027s, Character11.118s, System4.730s and
stock tests6.567s. Module/native integrity and documentation links also pass.

The app audit `temp/m4be-evidence-audit.json` verifies prepared inputs, config,
binary, PNG dimensions/hashes and runtime logs. All three use binary SHA256
`0daa6080198b869d3e8d806c8f4646149267a9864c5b6c0566d50030ad179da8`.
Expression-hit and inherited-guard fixtures reach contact; the mixed stock
KFM/KFM720 capture at7s was visually inspected with both fighters and intact
stage/HUD. These are smoke/provenance checks; pause semantics are established
by the native numeric comparisons, not screenshot timing or whole-frame parity.
All new committed fixtures are text; compiled outputs/captures remain ignored.

## Remaining gate

Expression routing is now38/81 documented HitDef keys (46.9%);43 remain literal
or absent, including enums/attributes that need appropriate schemas. This is
a schema count, not a whole-engine or complete-behavior percentage.

This allocation covers the measured root hit/guard contexts and defaults.
Absolute KO slowdown/round clocks, other hit/slide/control timing fields,
large-duration/overflow boundaries and broad foreign/helper/projectile/
Pause/SuperPause combinations remain open. The six M4az alpha comparisons and
two M4bd spark raster comparisons are unchanged open failures. No rendering
golden or tolerance was altered. See [COVERAGE.md](COVERAGE.md) for the next
unclosed workstreams. Stop after the M4be checkpoint; another field family is
a new allocation.

These expression/default guarantees belong to the compiled HitDef controller
path. The raw cns.ParseHitDef API consumes numeric strings; independent callers
and other emitters require their own normalization and coverage.
