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

For a reproducible initial match, add `--match-seed 16807` to the launch command.
The game logs the actual seed. Omit the option for normal clock-based startup;
later matches advance the independent session sequence.

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
mkdir -p game/mugen/temp/bin game/mugen/temp/go-build game/mugen/temp/_tmp
GOCACHE="$PWD/game/mugen/temp/go-build" \
GOTMPDIR="$PWD/game/mugen/temp/_tmp" \
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
game/mugen/mugen.sh --check -p 1 -timeout=40m

# Full race checks; allow roughly forty minutes for the full native replay corpus.
game/mugen/mugen.sh --check -race -p 1 -count=1 -timeout=40m

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

Snapshot: **2026-11-06; through M4dj validation**.

M4dj adds native-backed NoKO lifetime, retained death state, self-KO/corpse-contact
and paused-KO behavior. All **44 new native programs** pass; **30/30 new world
crops are exact (100% of this bounded corpus)** across **15,264,000 RGB pixels**.
The fresh full race suite passes **4,918 results across 36 packages**. Stock
win/rematch/win and private audio pass. [Evidence and limits](docs/NO-KO.md).
Earlier guard/walk and missing-state corrections remain covered; earlier HUD
and renderer differences remain unwaived.

The playable one-on-one engine includes compiled state programs and expressions,
combat and authored recovery, projectiles/reversals, helpers and binding,
rounds/results/rematches, sound, palette effects, angles/transparency, EnvColor
and EnvShake. The stock sweep recovery loop is fixed. Topic evidence and limits
are recorded in the [coverage ledger](docs/COVERAGE.md) and
[compatibility inventory](docs/COMPATIBILITY.md).

AfterImage/AfterImageTime include trail recording, palette aging, shared player/
helper capacity, stopped-context retention, saved palette mappings and invisible
current actors. All **189 native replay programs**, including nine load
rejections, pass. The fresh actor image matrix has **46 of 48 crops exact across
23,404,800 RGB pixels**. Its two 45-degree current/trail image failures remain
unwaived; a current-only control reproduces the shared renderer issue without
AfterImage. Quarter-turn trails match exactly. See
[actor evidence](docs/AFTERIMAGE-ACTORS.md) and [history/schema evidence](docs/AFTERIMAGE.md).

Projectile trails now include typed nested fields, global actor/projectile
capacity, ordered admission, saved palettes and measured motion/recording/expiry
clocks. All **159 new behavioral programs**, including five schema rejections,
pass alongside older Projectile/AfterImage regressions. The fresh projectile
image matrix has **152 of 154 crops exact (98.7% of this corpus)** across
77,337,600 RGB pixels. Its two missing-shadow cases were unwaived at M4cz; M4da now matches both controls. See
[projectile evidence](docs/PROJECTILE-TRAILS.md).

Shared floor shadows now render for players/helpers and flagged projectiles/
Explods, with measured stage geometry/fade, palette/blend behavior, suppression
flags and creation schemas. All **98 native numeric/schema programs pass**,
including three load rejections. All **95 visual applications run**; **35/95
crops match exactly (36.8% of this corpus)**. The other 60 retain strict failures:
59 have maximum RGB error 1; the scaled actor has maximum error 2. Both old
projectile-shadow controls now match exactly. [Shadow evidence](docs/SHADOWS.md).

M4dj's full scoped race run passes **4,918 results across 36 packages**
(25 tested, 11 compiled-only). All four old shake masks pass. Stock
win → rematch → win is visually inspected with 82 balanced input events and
private audio. Earlier angle/EnvShake/actor 45-degree failures remain unwaived.

ForceFeedback now matches the supplied beta's Null operation, with18 native
programs and four exact application controls. It ignores optional fields,
evaluates ordinary triggers and makes no host/device call. This target behavior
reduces the source stub count to two. [Evidence](docs/FORCE-FEEDBACK.md).

Explod creation now compiles ownership/remapping with the beta's expression
phases and character-resource palette consumer. All **63 native numeric/schema
programs pass**, including seven expected rejections. All **56/56 app crops match
exactly (100% of this corpus)** across **28,492,800 RGB pixels**. Fresh focused
races and stock win/rematch/win with audio pass.
[Evidence and bounds](docs/EXPLOD-CREATION-PALETTES.md).

Common-resource and helper palette contexts now include native common ownership
defaults, explicit shared local PalFX and forced-map replacement/base fallback.
All **42 new programs and 42 new app crops pass (100% of each corpus)**;
21,369,600 new RGB pixels compare exactly. Four earlier palette crops, focused
races, stock rematch and private audio also pass.
[Context evidence](docs/EXPLOD-PALETTE-CONTEXTS.md).

Explod creation drawing now includes typed angles, transparency/alpha defaults,
AIR blend overrides and conditional expression evaluation. All **47 native
numeric/schema programs pass**, including five expected rejections. **36/42 app
crops match exactly (85.7% of this corpus)**; six fractional/projected rotation
cases retain strict pixel failures. The sampling experiments remain outside
production. [Drawing evidence and bounds](docs/EXPLOD-CREATION-DRAWING.md).

Explod random displacement and capacity now use the native expression/draw order
and a shared pool for roots, helpers, retired owners and hit sparks. Low limits
clamp to eight; saturation drops new objects without terminating the game. All
**53 native programs pass**, including eight expected rejections; **45/45 new
app crops match exactly (100% of this corpus)** across **22,896,000 RGB pixels**.
The full suite and fresh stock rematch/audio checks pass.
[Admission and random evidence](docs/EXPLOD-CAPACITY-RANDOM.md).

ModifyExplod now retains native quoted transparency tokens and skips their
unused alpha expressions. All **20 native programs pass**, including one expected
rejection; **19/19 app crops match exactly (100% of this corpus)** across
**9,667,200 RGB pixels**. Focused races and stock rematch/audio pass.
[Token evidence and validation limits](docs/MODIFY-EXPLOD-ENUMS.md).

AssertSpecial now has typed native flag validation and global background,
foreground and bar visibility consumers with measured pause/reset behavior.
All **48 native programs pass**, including eight expected rejections. All
**40/40 world crops match exactly (100% of this corpus)** across **20,352,000
RGB pixels**. **5/40 complete world+HUD crops are exact (12.5%)**; 35 strict HUD
failures remain open, including the no-flags baseline. Focused races and stock
rematch/audio pass. [Flag evidence and open HUD gates](docs/ASSERT-SPECIAL.md).

Item 3 continues with remaining Explod consumers, AssertSpecial,
system animations/dust, clipboard and residual controller schemas.
Broader modes/AI/progression, assets, fonts and final renderer/platform conformance
remain item 4. There is no measured whole-engine completion percentage. The
[previous progress narrative](docs/PROGRESS-HISTORY-M4CW.md) preserves earlier details.

| Measured scope | Covered / total | Percentage | Meaning |
| --- | ---: | ---: | --- |
| NoKO native lifetime/contact/entry programs in M4dj | 44 / 44 | **100%** | This recorded corpus; audio and wider entity/round variants remain. |
| NoKO world outcome crops in M4dj | 30 / 30 | **100%** | Exact fixed world observers; earlier HUD/raster obligations stay open. |
| AssertSpecial numeric/schema programs in M4dh | 48 / 48 | **100%** | Recorded literal/global-clock corpus, including eight expected rejections; other consumers remain. |
| AssertSpecial world crops in M4dh | 40 / 40 | **100%** | World pixels only; complete HUD comparisons retain strict failures. |
| Complete AssertSpecial world+HUD crops in M4dh | 5 / 40 | **12.5%** | 35 strict HUD failures remain; no tolerance or comparison region changed. |
| ModifyExplod token native programs in M4dg | 20 / 20 | **100%** | Recorded schema/evaluation corpus, including one expected rejection. |
| ModifyExplod token app crops in M4dg | 19 / 19 | **100%** | Exact pixels in this corpus; older renderer failures remain open. |
| Explod admission/random native programs in M4df | 53 / 53 | **100%** | Recorded schema/phase/count/RNG corpus, including eight expected rejections. |
| Explod admission/random app crops in M4df | 45 / 45 | **100%** | Exact pixels in this corpus; earlier shared sampling failures remain open. |
| Explod creation drawing native programs in M4de | 47 / 47 | **100%** | Recorded numeric/schema corpus, including five expected rejections. |
| Explod creation drawing app crops in M4de | 36 / 42 | **85.7%** | Six strict fractional/projected sampling failures remain; no tolerance changed. |
| Explod palette context native programs in M4dd | 42 / 42 | **100%** | Common/helper/wildcard context corpus; broader asset/profile variants remain open. |
| Explod palette context app crops in M4dd | 42 / 42 | **100%** | Exact pixels in this new corpus; four additional old controls also pass. |
| Explod creation palette numeric/schema programs in M4dc | 63 / 63 | **100%** | Recorded corpus, including seven expected rejections. |
| Explod creation palette application crops in M4dc | 56 / 56 | **100%** | Exact pixels in this corpus; common/helper contexts remain separate. |
| Shadow native numeric/schema programs in M4da | 98 / 98 | **100%** | This recorded corpus only; includes three expected load rejections. |
| Exact shadow image crops in M4da | 35 / 95 | **36.8%** | Strict pixels; 60 one/two-byte fringe/scale failures remain. No tolerance relaxed. |
| Projectile image programs in the historical M4cz corpus | 152 / 154 | **98.7%** | M4cz result; M4da separately fixes both missing-shadow controls. The whole image corpus was not rerun here. |
| Documented controller names with dispatcher registrations | 89 / 89 | **100%** | Names only; two stubs and other partial consumers remain. Not full controller compatibility. |
| Documented controller names with typed compiler routes | 75 / 89 | **84.3%** | Parameter routing only; native variants and dependent consumers remain incomplete. [Source audit](docs/CONTROLLER-BINDINGS.md). |
| Documented trigger names recognized by static validation | 127 / 127 | **100%** | Recognized vocabulary; environment bindings and all variants still need coverage. |
| Trigger families with candidate runtime routes | 120 / 127 | **94.5%** | Source routing only; seven unbound, variants incomplete. [Audit](docs/TRIGGER-BINDINGS.md). |
| Documented GetHitVar selector routes | 34 / 36 | **94.4%** | All 34 beta-accepted selectors have routes; two manual-only names reject. [Recovery evidence](docs/LIEDOWN-RECOVERY.md). |
| Loaded stock character state programs compiling successfully | 216 / 216 | **100%** | 108 states per KFM variant, including loaded common states; execution paths and interactions are a separate obligation. |
| Documented HitDef keys routed through the expression compiler | 72 / 81 | **88.9%** | Numeric expression routing; nine other documented keys are literal masks/enums. |
| Documented HitDef keys with typed schemas | 81 / 81 | **100%** | 72 numeric and nine literal schemas with bounded native consumers; full interactions remain open. |

These percentages measure the named scopes. **Whole-engine behavioral
compatibility has not yet been quantified.** They cannot be averaged into an
overall completion percentage or treated as line/branch test coverage.
Controller counts come from [the dispatcher](engine/statemachine.go) against
the 89-name [reference inventory](docs/COMPATIBILITY.md); all names now register.
Typed route counts come from the [reproducible source audit](docs/CONTROLLER-BINDINGS.json). Trigger recognition comes from
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
simultaneous/team and foreign/resource variants, broader KO/clock interactions, complete modes and AI, BGM,
broader camera/render/effect behavior and final asset/conformance coverage.
Twelve angle/raster comparisons and seven EnvShake precision profiles remain open;
the earlier eight alpha/spark counterexamples pass unchanged goldens.
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
