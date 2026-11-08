# MUGEN Go engine

A Go/Ebitengine rewrite of Paintown's MUGEN engine, targeting Elecbyte
**M.U.G.E.N 1.1 beta 1**. The current build is semi-playable: stock KFM/KFM720
matches run with stages, HUD, sound, combat, rounds and rematches. Full engine
and asset compatibility remains in progress. Storyboards honor their canvas,
retain uncleared scenes and render colored fades consistently across display
refreshes; see
[storyboard verification](docs/STORYBOARD-CANVAS.md).

## Requirements and data

- Go **1.26 or newer** and Bash for the [mugen.sh launcher](mugen.sh). Dependencies are pinned in
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
Linux amd64 standalone build, normal startup, connected gameplay segments and
windowed keyboard combat with audio have passed. Other platforms are deferred.

**Run every command below from the repository root.**

## Run

Launch the title/menu and character/stage selection:

```bash
game/mugen/mugen.sh --game --data-dir game/mugen/temp/mugen-1.1b1
```

Stages belong under the selected data root's `stages/` directory, but the menu
uses a selection list rather than scanning that directory. Add each stage DEF
under `[ExtraStages]` in the active motif's `select.def`. With the supplied
`data/mugen1/system.def` motif, that file is `data/select.def`, for example:

```ini
[ExtraStages]
stages/venice.def
stages/deejay.def
```

Keep each stage's SFF and any music referenced by its DEF at the authored paths.
Restart the game after editing the list; select a stage after confirming both
characters. Characters similarly belong under `chars/` and need entries under
`[Characters]`. A motif can have its own selection file, such as
`data/kfm/select.def` for the alternate KFM motif.

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
team HUD and turns handoff work. Hold forward/back immediately before a Turns
match or reserve loads to rotate the remaining roster; Up swaps halves when
four members remain. Defeated members stay out. Team Arcade and Team Co-op ask
P1 for the opponent's team type after the human selections. In Watch, P1 selects
each team's mode and roster in sequence, then both sides use AI.

AI executes authored CMD motions/chords through the same state controllers as
human input, with difficulty-dependent reaction and defence. Arcade/team-arcade/co-op progress through ordered opponents, victory, continue
and ending screens. Survival carries/replenishes life and shows its streak;
training provides unlimited time, KO protection and refills; watch uses AI on both
sides. Mixed-team health/round budgets and Survival recovery now follow the
supplied beta; exact native AI decisions remain precision work. Remaining
media/asset compatibility is still in progress. Menu mode names do not imply complete mode
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

Body collisions use CNS ground/air widths and temporary `Width` values when
the fighters' hurtboxes overlap vertically, including air/ground and air/air
contact. Fighters separated vertically can pass each other. Projectile hits
do not push their owner backward at the corner; direct and helper attacks can.
`PosFreeze` stops velocity-driven movement while body push and CNS time continue.
Pause/SuperPause use their separate actor clocks and movement allowances.

Character CMD files can use `[Remap]`: `x = a` makes authored `x` commands read
the configured `a` input; a blank `a =` disables logical `a`. Mappings use one
input snapshot, so several logical buttons may share an input and swaps work
without cascading. Holds, charged releases, key-controlled helpers and authored
AI commands use these mappings. Menu bindings continue to use the keyboard
configuration below.

### Adding characters

Put character folders or ZIPs in your data directory's `chars/`, then add entries
to the motif's `select.def` (normally `data/select.def`):

```ini
[Characters]
fighter, stages/kfm.def
fighter.zip, stages/kfm.def
"other fighter.zip:Alternate.def", stages/kfm.def
```

`fighter.zip` uses `fighter.def` at the archive root. Use `pack.zip:path/file.def`
for another DEF or a nested folder. Quoted filenames retain spaces; Windows
separators and case-insensitive asset lookup work inside packs. Portraits,
outfits, helpers, intros/endings and rematches use the same loaded pack.

Quick matches also accept ZIP entries, for example:

```bash
game/mugen/mugen.sh --quick-match 'fighter.zip,other.zip:Alternate.def'
```

Each archive expands once into temporary storage and is reused until shutdown;
normal exit, startup errors and handled termination remove its expanded files.
The original ZIP is preserved. Packs support up to 1 GiB of expanded content and
65,536 entries. No assets or local configuration files need to be added to Git.

### Keyboard overrides in mugen.yaml

Place `mugen.yaml` in your selected `--data-dir`, alongside `data/`, `chars/` and
`stages/`. This is a local configuration file; `game/mugen/mugen.yaml` is ignored
by Git. Create your file using the example below. A binding can be one key or a
list of alternatives:

```yaml
input:
  p1:
    keyboard:
      a: [KeyA, KeyNumpad1]
      x: KeyZ
      start: [KeyEnter, KeyNumpadEnter]
      z: []
  p2:
    keyboard:
      a: [KeyU, KeyNumpad7]
```

Either listed key activates the action in menus and gameplay. These are
alternatives, not chords or macros. Each list replaces that action's previous
binding; omitted actions retain `mugen.cfg` settings or their built-in defaults.
`none`, `disabled` and `[]` disable an action. Duplicate keys are ignored; an
unknown key name reports a warning and leaves the entire binding unchanged.
Restart the game after editing. Startup output lists the active keys.

Supported actions are `jump` (`up`), `crouch` (`down`), `left`, `right`, `a`, `b`,
`c`, `x`, `y`, `z`, `start` and `escape`. Direction bindings also move menu cursors;
`start` also confirms selections. Names are case insensitive, and the `Key`
prefix is optional. Both players can have their own lists; assigning the same
physical key to both players activates both mappings.

The first valid override file wins. Search order is the data root's `mugen.yaml`,
its `data/mugen.yaml`, `~/.paintown/mugen.yaml`, the working directory's
`mugen.yaml`, `game/mugen/mugen.yaml`, then `mugen.yaml` beside the executable.
Only that file is overlaid on `mugen.cfg`; YAML files are not combined.

Background music supports WAV, MP3 and Ogg Vorbis from motif, stage and storyboard
settings, with looping, sample loop points and independent music volume. NoMusic
pauses/resumes the playback cursor while sound effects continue. Linux MIDI and tracker support uses optional system libraries described below.
Physical CD playback remains unavailable. The stock stage references
sound/kfm.mid, which is absent from the supplied local distribution.

Stage presentation includes camera zoom, moving floors (`zoffsetlink`), background
scale/clip controls, linked elements, animated parallax floors, reflections, and
legacy `hires` stage defaults. Stages declaring both `tensionhigh` and
`tensionlow` use the 1.1 vertical edge camera and height-based zoom fitting;
these parameters override `verticalfollow` and `floortension`. Horizontal zoom
fits the player span plus both stage-space tension margins. Horizontal camera
bounds describe the fully zoomed-out view; zooming in permits more camera travel
within the same stage edges. Modern stage backgrounds retain fractional camera
scrolling at the game's resolution. Pre-1.0 stages (including an omitted
`mugenversion`) round only background scrolling to whole stage units; fighter,
effect and script coordinates retain their precision. Tiled backgrounds
follow camera travel; authored
BG controllers can pause, hide and change their animations. Native rendering
precision is still tracked in the progress ledger.

Camera fitting includes temporary `Width` edge bars and clips its fitting region
to the stage edges. `ScreenBound = 0` still permits offscreen fighter movement;
the camera's limits do not clamp those fighter positions.

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
view. The current checklist has32/34 implemented items (94% of checklist items, not
native conformance or remaining effort); see the explicit partial rows.
The current Linux implementation supports one-on-one, simul/turns, arcade and
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

Legacy SFF v1 characters now load the ACT files declared as `pal1`–`pal12` in
their DEF, preserving private sprite colors. Outfits follow selection and round
reset; legacy HUD portraits retain their default bank as observed in1.1b1.
Modern 1.x characters keep their authored `RemapPal` initialization, including
those that still use SFF v1.

Shared-palette PCX sprites may omit their palette trailer. TrueType font paths
support Windows separators, mixed-case folders and absolute filenames on Linux;
missing fonts use the reported fallback described in [font notes](docs/BUILD.md).

Linux legacy music optionally uses installed `libfluidsynth.so.3` for MIDI and
`libopenmpt.so.0` for MOD/S3M/XM/IT. MIDI needs a GM SoundFont: set
`MUGEN_SOUNDFONT=/path/to/font.sf2`, or use an installed TimGM6mb/FluidR3_GM font.
No codec libraries or music assets are bundled. WAV/MP3/Ogg need no extra codec
installation. Missing tracks/codecs produce a sound warning; physical CD audio
is unavailable. Linux is the current validation target.
