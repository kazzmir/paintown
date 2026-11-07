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
`game/mugen/temp/`. Quick match uses two human-controlled players. Team VS/Team Arcade menus offer
Single, Simul (two active members) and Turns (two to four members); choose mode
with Up/Down, turns count with Left/Right, then confirm each roster member.
Simul partners use AI; co-op routes the second human to P3. Team outcomes, stock
team HUD and turns handoff work. AI executes authored CMD motions/chords through the same state controllers as
human input, with difficulty-dependent reaction and defence. Arcade/team-arcade/co-op progress through ordered opponents, victory, continue
and ending screens. Survival carries/replenishes life and shows its streak;
training provides unlimited time, KO protection and refills; watch uses AI on both
sides. Recovery calibration and exact native AI decisions remain deferred with
precision. Remaining media/asset compatibility is still in progress. Menu mode names do not imply complete mode
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
configuration overrides can change them. Ctrl-D cycles debug output through
players/helpers and off when enabled in mugen.cfg. Authored clipboard text appears
at the bottom of the window; multi-hit combo totals appear in the fight HUD.

Background music supports WAV, MP3 and Ogg Vorbis from motif, stage and storyboard
settings, with looping, sample loop points and independent music volume. NoMusic
pauses/resumes the playback cursor while sound effects continue. Linux MIDI and tracker support uses optional system libraries described below.
Physical CD playback remains unavailable. The stock stage references
sound/kfm.mid, which is absent from the supplied local distribution.

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

## Progress and validation

[Project progress](docs/PROGRESS.md) is the single completion checklist for the
whole port: state machine, controllers, combat, match flow, modes, assets, audio
and platforms. It shows completed scenarios and concrete partial/missing work.
The checklist replaces test counts and typed-routing percentages as the progress
view. The current Linux implementation supports one-on-one, simul/turns, arcade and
survival progression, training/watch, authored CMD and AI input, and storyboard
text/sound/windows plus AIR1.1 interpolation. Full native1.1 equivalence remains
in progress: precision calibration and physical CD audio are still open.

```bash
game/mugen/mugen.sh --coverage --gaps   # lightweight source/feature report
game/mugen/mugen.sh --check --list      # consolidated segment completion
game/mugen/mugen.sh --check             # pending/reopened segments only
game/mugen/mugen.sh --acceptance        # explicit playable end-state scenario
```

Tests are consolidated into five [state-machine segments](tests/SEGMENTS.md).
Completed scenarios stay checked and are excluded until related source changes
or a specific regression reopens them. Implement coherent batches, validate the
affected end state, and record completion once. The full historical corpus is an
explicit --check-all diagnostic, not the normal workflow.

Generated testing data stays out of Git. Segment runs reuse shared assets/cache
and remove scratch automatically. The 2026-11-06 cleanup reclaimed126.58 GiB from
old run banks while preserving the reference distribution and working tools.
See the [short handoff](docs/HANDOFF.md) and [implementation plan](docs/ITEM-3-PLAN.md).

Linux legacy music optionally uses installed `libfluidsynth.so.3` for MIDI and
`libopenmpt.so.0` for MOD/S3M/XM/IT. MIDI needs a GM SoundFont: set
`MUGEN_SOUNDFONT=/path/to/font.sf2`, or use an installed TimGM6mb/FluidR3_GM font.
No codec libraries or music assets are bundled. WAV/MP3/Ogg need no extra codec
installation. Missing tracks/codecs produce a sound warning; physical CD audio
is unavailable. Linux is the current validation target.
