# Go MUGEN port assessment

Historical baseline. For current progress and remaining work at the M4bd review
pause, read [PROGRESS-ASSESSMENT.md](PROGRESS-ASSESSMENT.md).

Assessment date: 2026-10-16. Latest commit touching the port: `7df33761`
(2026-04-14). Status: assessment and planning only; implementation awaits review.

The port is a useful engine prototype, but it is not close to demonstrated
M.U.G.E.N 1.1 beta 1 compatibility. The principal gaps are behavioral: expression
evaluation, state scheduling, combat, entity ownership, assets, and match flow.
Registering the remaining controller names will not close these gaps.

The proposed implementation is in [PLAN.md](PLAN.md). The exhaustive controller
and trigger inventory and stock asset census are in
[COMPATIBILITY.md](COMPATIBILITY.md).

## Evidence and limits

Inspected the Go port, the local reference documentation/content, and selected
corresponding C++ implementations in `src/mugen/`. The reference distribution is
`temp/mugen-1.1b1/` from `game/mugen/`, or `../temp/mugen-1.1b1/` from this docs directory.
Use the links in the compatibility inventory for the exact files.

Findings below are source-confirmed unless explicitly labeled as a test result
or a question for reference testing. No new regression tests or engine changes
have been made. The Elecbyte executable has not been launched or instrumented;
behavioral parity with it has not been measured. Wine is installed, but its
ability to run this distribution remains unverified. Existing Go tests run
against the separate `data-new/mugen/` tree, not the bundled 1.1 distribution.

## Baseline verification

Commands ran from the repository root with `GOFLAGS=-mod=readonly`,
`GOPROXY=off`, and a build cache in `game/mugen/temp/go-build`:

| Command | Result | What it establishes |
| --- | --- | --- |
| `go test ./game/mugen/...` | FAIL | `tests/character/physics_check.go:12` and `tests/character/main.go:180` both define `main`. The full tree does not build as a test target. |
| `go test -v ./game/mugen/tests` | PASS, 11 tests, no skips on this machine | A small set of parser/configuration and v1 asset checks passes with the available external data. |

The passing tests include warnings for a missing motif font and an unusable
relative unresolved-log path. The motif test intentionally supplies incomplete
assets and still succeeds. Seven skip sites allow data-dependent checks to
disappear on machines without the external data. No behavioral state-machine,
expression, input-sequence, combat, round, or reference-comparison tests exist.
No measured percentage of functional compatibility follows from these tests.

## Findings in priority order

### A01 — The normal application cannot start a match

`system/versus.go:36` explicitly returns to the title where the fight should
start. `system/select.go:199` does not pass a match description containing the
selected players, palettes, stage, and mode onward. Win, continue, and game-over
screens at `system/options.go:279` are black-screen stubs. `motif.Files.Fight`
is parsed but no production match/HUD implementation consumes it.

The character application in `tests/character/main.go` is a separate diagnostic
fight harness; its existence does not provide the game's missing flow.

### A02 — Stock 1.1 assets cannot load completely

`sff/sff.go:40` rejects any SFF whose major version is not 1. Of the reference
distribution's 27 SFF files, 15 are v2: 13 have version bytes reported as
`2.0.0.0` and two as `2.0.1.0`. The latter is the format called v2.01 in the
bundled release history; do not confuse it with a version named v2.1.

Both KFM character sprite files, all three stage sprite files, the default
`mugen1` motif/fight sprites, and the SFF bitmap fonts are affected. Stock data
uses raw indexed data, RLE8, LZ5, and PNG32 records. The format requirements
extend to RLE5, PNG8, PNG24, palette links, sprite links, and palette selection,
which the stock corpus alone does not cover.

`font/font.go:54` accepts only the old binary `ElecbyteFnt` container. It cannot
read FNT v2 DEF files backed by SFF or TrueType. The default reference config
selects `data/mugen1/system.def` and a 1280x720 game space, so these are startup
requirements, not optional future asset enhancements.

### A03 — Expression semantics differ from the target language

`evaluator/evaluator.go:68` evaluates everything as `float64`. The reference
specifies distinct 32-bit integers, single-precision floats, and `bottom`.
For example, integer `5/6` must produce integer zero, unlike the port's division.
Division by zero returns zero and modulo zero can panic (`evaluator.go:180`).
Unknown identifiers return a large finite sentinel, which can be treated as
truthy, and unknown calls return zero. Neither models propagation of `bottom`.

`evaluator/parser.go:138` omits assignment precedence and logical XOR;
evaluation omits the bitwise binary operators. There is no trigger redirection,
and old condition syntax such as `AnimElem = n, ...`, `TimeMod`, and
`HitDefAttr` needs dedicated handling. `IfElse` lazily evaluates one branch,
where the reference distinguishes eager `IfElse` from lazy `Cond`.

Important stock triggers such as `AnimElemTime` are absent. `AnimElem = n`
is treated as a comparison with the current frame index instead of an event
relative to the element's start. `MoveType` is not evaluated even though it is
exposed on the environment interface. `P2BodyDist` and `P2Dist` currently use
the same axis distance. The custom evaluator is process-global mutable state
installed while checking controller triggers (`character/statemachine.go:342`).

### A04 — Parsing destroys information required at runtime

`parsers/parser.go:237` reconstructs values by inserting spaces between tokens.
Operators, quotes, commas, numeric spelling, and nesting are not preserved as a
source expression. `cns/cns.go:499` also removes quotes from string-valued raw
expressions. A correct expression engine therefore requires a source-preserving
front end, rather than fixes solely inside `Evaluate`.

Statedef values at `cns/cns.go:340` are converted to numeric literals at load
time. Most should accept expressions evaluated on entry. `[State ...]` header
association, duplicate sections, malformed inputs, Windows paths, and merge
precedence need explicit compatibility tests. The header's label must not
accidentally create an unrelated state or attach controllers to an old state.

### A05 — State entry, triggers, persistence, and hitpause are inconsistent

`character/statemachine.go:243` and `character/character.go:1596` both apply
entry attributes. This can overwrite a ChangeState `ctrl`/`anim` override on
the subsequent state pass. Omitted sprite priority becomes zero in the parser,
although execution expects an omission sentinel. `poweradd`/`facep2` are stored
but unused; hit-definition, move-hit, and hit-count persistence are ignored.

`persistent > 1` has an empty implementation branch at `statemachine.go:302`.
Invalid triggers are discarded, so a controller can run after one of its
conditions disappears. Unknown controllers are silently skipped. Trigger groups
are accepted across number gaps, although the documentation says to stop at
the first missing group. A `triggerall`-only controller is accepted without a
numbered group and needs validation against the target's requirements.

`Character.Update` skips state evaluation entirely during hitpause
(`character.go:349`), making the inner `ignorehitpause` check unreachable.
The last hitpause tick can also disagree with `IsPaused()` after the counter is
decremented. Negative states have no ownership/key-control eligibility rules.
Transition limits silently stop processing rather than provide a useful error.

### A06 — There is no coherent match-owned simulation model

The same `PlayerID` identifies human input and entities: helpers/projectiles
inherit their owner's value (`character.go:730`, `character.go:768`). Hit
deduplication keys by this field, so different entities can alias each other.
The `Target` slice doubles as the opponent list; it cannot represent acquired
HitDef targets, target IDs, teams, nearest-enemy queries, and redirects correctly.

Many required operations need world-level ownership and lifetime handling:
target bindings, entity creation/removal, pause/superpause, shared effects,
round state, and cameras. Adding these only as local Character methods will
make timing and update order increasingly difficult to reason about.

### A07 — Combat does not enforce the parsed HitDef rules

`character/collision.go:45` and `character.go:1704` resolve box overlap without
checking an active HitDef, hit/guard attributes, HitBy/NotHitBy masks, juggle,
priority, reversal, team rules, or hit overrides. Protection masks are stored
and decremented but not consulted by hit resolution. ReversalDef only stores
a map. Several HitDef fields are missing or parsed only as literals.

`ProcessHit` applies normal hit damage and pause before choosing a guard state;
guard damage/velocity/pause and MoveGuarded reporting are not correctly separated.
Custom defender states and animation ownership for throws are absent.

`HitTime` decrements twice on ordinary unpaused ticks (`character.go:240` and
`character.go:308`). Move-hit/contact/guarded status is reset before controllers
can reliably observe the preceding collision, rather than maintaining the
documented elapsed-time semantics. Hardcoded transitions in
`HandleBuiltinTransitions` can override character/common CNS behavior and allow
some actions without checking control. They should be replaced with documented
engine rules plus correctly executed common states.

### A08 — Helpers, projectiles, targets, and explods are incomplete

Helper creation lacks most parameters, proper unique identity, and documented
negative-state/key-control rules. `DestroySelf` is empty. `Projectile` reads
generic `anim`/`vel` fields and creates a full Character that runs ordinary
states, rather than implementing the projectile parameter set and lifecycle.
All eight Target controller methods and all three BindTo methods are empty.

`ChangeAnim2` is empty and `SelfState` cannot restore separate state ownership.
Explods always render the first frame (`character/player.go:346`), do not
implement 1.1 space/bindID behavior, and omit most timing/binding/lifetime
parameters. `RemoveExplod(-1)` does not implement the default remove-all behavior.

### A09 — Visual effects and audio are largely disconnected

Afterimages, palette effects/remapping, transparency, screen color/shake, width,
and screen bounds are stubs. Angle/offset/priority setters exist, but the player
renderer does not consume all of their state. Character size scaling is used
for collision while the draw path uses only the caller's scale, causing another
potential mismatch between art and boxes. Helper/explod Y offsets use the opposite
sign to the character application's main draw convention.

`handlePlaySnd` and `handlePause` are empty; character `.snd` files are not
loaded by `LoadPlayer`. Menu SND/WAV playback is present. Background music
filenames are parsed but no music playback path consumes them. Sound channels,
panning, pause, stop, and common-versus-character sound ownership remain work.

### A10 — 1.1 coordinate spaces, zoom, and UI are missing

`system/main.go:142` fixes logical rendering at 320x240.
Character DEF parsing retains only `[Files]`; motif/stage models do not implement
the full localcoord transforms. Camera zoom limits, zoomdelta, edge/screen
triggers, and the new Explod coordinate behavior are not implemented. Stock
KFM720 uses 1280x720 local coordinates, making mixed-resolution tests essential.

Background elements already implement useful tiling, parallax, sinusoidal
movement, blending, and controllers. Their timing, reset behavior, clipping,
and zoom interaction still need reference tests. Title update bypasses the
background's aggregate controller update. Stage shadows/reflections are parsed
without a complete render path; stage music is not loaded.

Input supports keyboard polling and some command matching, but lacks simultaneous
button chords, timed release/charge syntax, strict sequence modifiers, complete
direction semantics, and actual gamepad reads. `Command()` falls back to raw
inputs, bypassing command definitions. Numeric zero doubles as an invalid key
and the valid Ebitengine KeyA value; `mergeExtendedConfig` skips it, so the
shipped `KeyA` override cannot be applied correctly. Game modes share selection
scaffolding without their distinct match/progression/team/AI rules.

### A11 — Invalid assets can be accepted partially or crash/hang

SFF v1 swallows read/decode errors and appends missing images. Stage/motif and
player/common-state loaders similarly swallow required-file failures.
PCX decoding does not validate a scanline stride against width before slicing;
truncated RLE returns a partially filled image. FNT bounds checking adds uint32
offsets before validating, allowing overflow. Zero-duration-only animations can
loop indefinitely in `air.Player.Update`. These need small malformed fixtures
and bounded decoding/animation execution as part of format support.

## Comparison with the original Paintown MUGEN engine

| Area | C++ code worth studying | Go port status / implication |
| --- | --- | --- |
| State entry and expression compilation | `src/mugen/character.cpp` (`State::transitionTo`), `compiler.cpp`, parser/AST code | C++ compiles entry expressions; Go reduces many to literals. Reuse the concepts, verify numeric semantics against Elecbyte. |
| Controller activation | `src/mugen/state-controller.cpp` | C++ contains meaningful persistence, parameter evaluation, variable and sound behavior that was reduced or stubbed in Go. |
| Custom states / throws | `src/mugen/character.cpp` (`useCharacterData`, defender state selection) | State-owner switching exists in C++ but is missing in Go. Build explicit ownership rather than sharing mutable player state. |
| SFF v2 | `src/mugen/sff.cpp` | C++ has palette/link and RLE8/RLE5/LZ5 implementations. Its displayed decoder switch omits raw and PNG formats, so copying it alone still misses stock 1.1 data. |
| Match/combat/HUD/entity lifecycle | `src/mugen/stage.cpp`, `match.cpp`, `run-match.cpp`, `characterhud.cpp`, `helper.cpp`, `projectile.cpp` | Much of the game-level integration has no equivalent completed path in Go. |
| UI/assets | `src/mugen/font.cpp`, `background.cpp`, `storyboard.cpp`, `character-select.cpp` | Useful prior art for resource resolution and timing; not independent evidence of complete 1.1 support. |

The original engine is a design/reference source, not the acceptance oracle.
One concrete caution: `src/mugen/character.cpp` around line 3455 records a conflict
between the written negative-state transition rules and an observed executable
test. The plan explicitly includes a reference probe for this kind of ambiguity.

## Conclusion for planning

Preserve the useful format/background/UI work where tests support it. Establish
a typed expression compiler and a deterministic match/world core, then complete
behavior in dependency order. Deliver an early stock KFM match milestone, but
retain a separate completion gate for all documented controllers, triggers,
parameters, asset formats, and 1.1-specific interactions. No full-compatibility
percentage or reliable calendar estimate is supported by the present evidence.
