# MUGEN Go engine

A Go/Ebitengine rewrite of Paintown's MUGEN engine, targeting Elecbyte
**M.U.G.E.N 1.1 beta 1**. The current build is semi-playable: stock KFM/KFM720
matches run with stages, HUD, sound, combat, rounds and rematches. Full engine
and asset compatibility remains in progress.

## Requirements and data

- Go **1.24 or newer** and Bash for the launcher. Dependencies are pinned in
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
game/mugen/mugen --game --data-dir game/mugen/temp/mugen-1.1b1
```

Start a two-player KFM versus KFM720 match immediately:

```bash
game/mugen/mugen --game \
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
game/mugen/mugen --check -timeout=20m

# Full race checks; the Match package alone currently takes about ten minutes.
game/mugen/mugen --check -race -timeout=20m

# Focused stock character loading and state compilation.
game/mugen/mugen --check -run '^TestStockPlayerLoadAndCompilationInventory$' -v
```

Many tests require a display because Character/font/Match still import
Ebitengine, including tests that open no window. Parser/evaluator/core tests can
run independently. Full Go regression checks replay recorded native observations
without running Wine. [Test instructions](tests/README.md) cover diagnostic
applications, reference probes and capture requirements; [build notes](docs/BUILD.md)
describe the isolated virtual display used for automated checks.

## Implementation progress and measured coverage

Snapshot: **2026-10-17, through M4bd**. Recent changes cover damage/power transfer,
lethal hit/guard normalization, fall damage, KO velocities, stage diagnostic
fill and native sound/spark-selection/pause comparisons.

| Measured scope | Covered / total | Percentage | Meaning |
| --- | ---: | ---: | --- |
| Documented controller names with dispatcher registrations | 87 / 89 | **97.8%** | Includes partial implementations and stubs; complete parameter/runtime support remains open. |
| Documented trigger names recognized by static validation | 127 / 127 | **100%** | Recognized vocabulary; environment bindings and all variants still need coverage. |
| Loaded stock character state programs compiling successfully | 216 / 216 | **100%** | 108 states per KFM variant, including loaded common states; execution paths and interactions are a separate obligation. |
| Documented HitDef keys routed through the expression compiler | 36 / 81 | **44.4%** | The other 45 keys are literal or absent, including enum/attribute fields that need appropriate schemas. |

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

Major remaining work includes full controller/trigger schemas, recovery/juggle,
projectiles/reversals/teams, exact KO/round clocks, complete modes and AI, BGM,
broader camera/render/effect behavior and final asset/conformance coverage.
Six alpha precision comparisons and two spark raster comparisons remain open.
Missing stock Windows outline-font payloads use a reported Go Regular fallback,
which does not reproduce their glyphs or metrics.

The [progress assessment](docs/PROGRESS-ASSESSMENT.md) maps remaining work to
M0–M8; [checkpoints](docs/CHECKPOINTS.md) record validation and reversal points.
The next workflow uses bounded feature batches, focused development checks,
native comparisons and broader regression runs at integration boundaries, with
findings and a local commit at each checkpoint. A refreshed behavior-coverage
ledger will guide future allocations. All implementation changes stay under
`game/mugen/`. See the [documentation index](docs/README.md) for topic evidence.
