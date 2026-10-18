# MUGEN Go engine

A Go/Ebitengine rewrite of Paintown's MUGEN engine, targeting Elecbyte
**M.U.G.E.N 1.1 beta 1**. The current build is semi-playable: stock KFM/KFM720
matches run with stages, HUD, sound, combat, rounds and rematches. Full engine
and asset compatibility remains in progress. Storyboard intros now honor their
own canvas and scale correctly into the game/window; see
[storyboard verification](docs/STORYBOARD-CANVAS.md).

## Requirements and data

- Go **1.24 or newer** and Bash for the [mugen.sh launcher](mugen.sh). Dependencies are pinned in
  [mugen.mod](mugen.mod) and [mugen.sum](mugen.sum).
- A graphical desktop with working OpenGL and audio. Linux builds require a C
  toolchain, `pkg-config`, ALSA development files and X11 development libraries
  (X11, Xrandr, Xxf86vm, Xi, Xcursor and Xinerama).
- The supplied MUGEN 1.1 beta 1 distribution, extracted to
  `game/mugen/temp/mugen-1.1b1/`. It must contain `data/mugen.cfg`, `chars/`,
  `stages/`, `font/` and the other stock resources. Use `--data-dir` to run
  with content elsewhere. The full integration corpus expects the stock copy
  at the path above.

The reference distribution and build outputs are ignored by Git; supply the
data separately after cloning. The Go game runs directly on the host. Wine is
only needed for optional comparisons against the original Windows executable.
Linux is the currently exercised development platform.

**Run every command below from the repository root.**

## Run

Launch the title/menu and character/stage selection:

```bash
game/mugen/mugen.sh --game --data-dir game/mugen/temp/mugen-1.1b1
```

Start a two-player KFM versus KFM720 match immediately:

```bash
game/mugen/mugen.sh --game \
  --data-dir game/mugen/temp/mugen-1.1b1 \
  --quick-match chars/kfm/kfm.def,chars/kfm720/kfm720.def \
  --stage stages/kfm.def
```

The launcher compiles/runs the Go application and keeps build caches under
`game/mugen/temp/`. Quick match uses two human-controlled players. Single-player
menu selection has a basic opponent policy; full arcade progression and AI
compatibility remain unfinished. Menu mode names do not imply complete mode
rules. See [match notes](docs/MATCH.md) for details.

| Action | Player 1 | Player 2 |
| --- | --- | --- |
| Move / jump / crouch | Arrow keys | W / A / S / D |
| a / b / c attacks | `,` / `.` / `/` | F / G / H |
| x / y / z attacks | L / `;` / `'` | R / T / Y |
| Start / confirm | Enter | U |

Confirm both characters, select a stage with P1 left/right, and confirm to
continue through versus into the fight. At the result, P1 Enter rematches.
Escape returns to the title during a fight or result. These are stock bindings;
configuration overrides can change them.

## Build a standalone executable

```bash
mkdir -p game/mugen/temp/bin game/mugen/temp/go-build game/mugen/temp/tmp
GOCACHE="$PWD/game/mugen/temp/go-build" \
GOTMPDIR="$PWD/game/mugen/temp/tmp" \
go build -modfile=game/mugen/mugen.mod -mod=readonly \
  -o game/mugen/temp/bin/mugen-go ./game/mugen/system

game/mugen/temp/bin/mugen-go --data-dir game/mugen/temp/mugen-1.1b1
```

The executable accepts the same `--quick-match` and `--stage` flags. It loads
external data at runtime; the executable alone does not include the assets.
The scoped module overlay preserves the repository's root `go.mod`/`go.sum`.
See [build/font notes](docs/BUILD.md) for dependency, font and display details.

## Tests

```bash
# Build/test every MUGEN package against the stock reference corpus.
game/mugen/mugen.sh --check -p 1 -timeout=20m

# Full race checks; the last full run took about seventeen minutes.
game/mugen/mugen.sh --check -race -p 1 -count=1 -timeout=20m

# Focused stock character loading and state compilation.
game/mugen/mugen.sh --check -run '^TestStockPlayerLoadAndCompilationInventory$' -v
```

Many tests require a display because Character/font/Match still import
Ebitengine, including tests that open no window. Parser/evaluator/core tests can
run independently. Full Go regression checks replay recorded native observations
without running Wine. [Test instructions](tests/README.md) cover diagnostic
applications, reference probes and capture requirements; [build notes](docs/BUILD.md)
describe the isolated virtual display used for automated checks.

## Implementation progress and measured coverage

Snapshot: **2026-10-18, through M4bx**. Recent changes cover damage/power transfer,
lethal hit/guard normalization, fall damage, KO velocities, stage diagnostic
fill, native sound/spark-selection/pause comparisons and compiled HitDef
hit/guard pause expressions with measured default/evaluation order. Storyboard
canvas scaling is corrected, and ground hit/slide plus dependent guard timing
expressions now follow native defaults and evaluation order. Air/down hit times
now compile as expressions, with corrected launch/down timer selection. Fall
flags and recovery delay now compile, and CanRecover follows the measured
fall clock; see [recovery evidence and limits](docs/HIT-FALL-RECOVERY.md).
The stock sweep loop is fixed by choosing Trip at contact and allowing CNS
recovery to finish; see [trip recovery](docs/TRIP-RECOVERY.md).
Fall X/Y velocity and hit gravity now compile as expressions, with verified
defaults, profile conversion and consumers; see [fall vectors](docs/FALL-VECTORS.md).
HitFallSet now follows native value/default rules, including the beta ignoring
its documented velocity fields; see [HitFallSet](docs/HITFALLSET.md).
Downed hits now respect `down.bounce`, including stock bounce/get-up timing;
see [downed-hit bounce](docs/DOWN-BOUNCE.md).
Guard distance now uses native integer expressions and preserves omitted/negative
range updates; see [guard distance](docs/GUARD-DISTANCE.md).
Literal masks/enums, contact animation metadata, None contacts and trip defence
now follow measured beta behavior; see [literal schemas](docs/HIT-LITERALS.md).
Chain selectors and per-move/projectile juggle costs now have measured consumers;
see [chains and juggle](docs/HIT-CHAINS-JUGGLE.md).
Distance limits and snap now have native timing, offset and duration consumers;
see [distance/snap constraints](docs/HIT-CONSTRAINTS.md).
Hit palette effects now change fighter colors with native arithmetic and
lifetimes while preserving HUD portraits; see [palette effects](docs/HIT-PALETTE.md).
Contact shake and stored fall shake now use the shared frame clock and render
with native rounding and profile conversion; the HUD stays fixed. See
[contact shake](docs/HIT-SHAKE.md) and [fall shake](docs/FALL-SHAKE.md).
The user-authorized 27-key allocation is complete; its M4bt full scoped races
pass. M4bu adds native KO slowdown, timer/pause boundaries and separate
control/pose/reset clocks with focused/native/application verification. See
[round lifecycle](docs/ROUND-LIFECYCLE.md). M4bv adds wins/draws, match completion,
win-pose holds, late damage/double KO and persistent reset, with 26 controls /
1,560 native integers. See [round results and remaining gates](docs/ROUND-RESULTS.md).
Legacy HitVel X/Y now returns native game-space floats, including heading,
guard/air/down, redirects and fractional profiles. The three preserved projectile
counterexamples pass unchanged; see [legacy velocity](docs/LEGACY-HITVEL.md).
Complete input-driven
matches/rematches and final full scoped races now pass (2,707 test/subtest
results). See [complete match validation](docs/COMPLETE-MATCHES.md).

| Measured scope | Covered / total | Percentage | Meaning |
| --- | ---: | ---: | --- |
| Documented controller names with dispatcher registrations | 87 / 89 | **97.8%** | Includes partial implementations and stubs; complete parameter/runtime support remains open. |
| Documented trigger names recognized by static validation | 127 / 127 | **100%** | Recognized vocabulary; environment bindings and all variants still need coverage. |
| Loaded stock character state programs compiling successfully | 216 / 216 | **100%** | 108 states per KFM variant, including loaded common states; execution paths and interactions are a separate obligation. |
| Documented HitDef keys routed through the expression compiler | 72 / 81 | **88.9%** | Numeric expression routing; nine other documented keys are literal masks/enums. |
| Documented HitDef keys with typed schemas | 81 / 81 | **100%** | 72 numeric and nine literal schemas with bounded native consumers; full interactions remain open. |

These percentages measure the named scopes. **Whole-engine behavioral
compatibility has not yet been quantified.** They cannot be averaged into an
overall completion percentage or treated as line/branch test coverage.
Controller counts come from [the dispatcher](engine/statemachine.go) against
the 89-name [reference inventory](docs/COMPATIBILITY.md); the unregistered names
are ModifyExplod and ExplodBindTime. Trigger recognition comes from
[names.go](evaluator/names.go). Stock compilation is checked by
[player_test.go](tests/player_test.go), and the HitDef audit is documented in
[HITDEF-SCHEMA.md](docs/HITDEF-SCHEMA.md). The inventory's historical status
columns describe the original baseline; use the current assessment for progress.

Working areas include typed expressions and state scheduling; helpers, targets,
custom states/throws and binding; stock sprite/font/resource loading; production
fight HUD/effects and sound; mixed character coordinates; and a connected
title → selection → versus → fight → result → rematch/exit path. Many behaviors
have focused native numeric, pixel or audio evidence.

Major remaining work includes full controller/trigger schemas, broader recovery/juggle interactions,
projectiles/reversals/teams, exact KO/round clocks, complete modes and AI, BGM,
broader camera/render/effect behavior and final asset/conformance coverage.
Six alpha precision comparisons and two spark raster comparisons remain open.
Missing stock Windows outline-font payloads use a reported Go Regular fallback,
which does not reproduce their glyphs or metrics.

The [progress assessment](docs/PROGRESS-ASSESSMENT.md) maps remaining work to
M0–M8; [checkpoints](docs/CHECKPOINTS.md) record validation and reversal points.
Development uses bounded feature batches, focused development checks,
native comparisons and broader regression runs at integration boundaries, with
findings and a local commit at each checkpoint. The
[current behavior-coverage ledger](docs/COVERAGE.md) records behavior and
remaining gates. All implementation changes stay under `game/mugen/`.
See the [documentation index](docs/README.md) for topic evidence.
