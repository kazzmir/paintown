# MUGEN Go engine

A Go/Ebitengine rewrite of Paintown's MUGEN engine, targeting Elecbyte
**M.U.G.E.N 1.1 beta 1**. The current build is semi-playable: stock KFM/KFM720
matches run with stages, HUD, sound, combat, rounds and rematches. Full engine
and asset compatibility remains in progress. Storyboards honor their canvas,
retain uncleared scenes and render colored fades consistently across display
refreshes; see
[storyboard verification](docs/STORYBOARD-CANVAS.md).
Grounded command moves now turn toward the opponent before state entry;
walking uses the authored turning animation and preserves physical direction
after a crossover. Ongoing locked attacks retain their facing.
Air jumps honor each character's height and per-flight count settings: release
and press jump again in midair. Landing restores the available air jumps.
Combat damage now uses the character’s base attack/defence stats along with
authored damage adjustments; healing and absolute life additions stay unscaled.
Blocking tracks travelling projectiles and attacking helpers independently of
their owner, using each attack’s current position and guard range.

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

A complete pack can be used as its own data root. Its `data/mugen.cfg` selects
the motif, which supplies the character/stage list and presentation assets:

```bash
game/mugen/mugen.sh --game --data-dir game/mugen/temp/sf2_mugen
```

Pack defaults and local YAML overrides determine the controls. Compatibility
findings for the supplied SF2 pack are recorded in the [progress ledger](docs/PROGRESS.md).
Its selection and legacy loading fixes now reach live fights. All 12 roster
entries compile; full gameplay across every character remains unverified. The
original motif displays eight normal portraits and places four boss slots below
the visible screen, including in the bundled native engine.

Several supplied SF2 character scripts omit a later-round initialization exit;
the original beta also stalls with this pattern. Create a corrected working
copy once, then use it for play:

```bash
python3 game/mugen/tools/sf2_playable.py
game/mugen/mugen.sh --game --data-dir game/mugen/temp/sf2_mugen_playable
```

The tool discovers all matching scripts (ten in the supplied pack), links media,
and copies the repaired scripts plus small writable configuration/log files.
For a working copy made by an earlier version, extend its repairs with:

```bash
python3 game/mugen/tools/sf2_playable.py --update
```

Updates preserve configuration and refuse to overwrite locally edited scripts.
Keep the original pack available. Neither pack nor local YAML belongs in Git.

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

When `--stage` is omitted, quick matches use `[Debug] StartStage` from the
selected `mugen.cfg`. An explicit `--stage` takes precedence.

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

Idle titles now enter the motif's `[Demo Mode]` sequence, including optional
selection/VS previews, AI combat, return time and intro cycling. Either player's
input returns to the title. The motif controls demo music, lifebars and debug
text. Title background controllers also advance.

`fontN.height` overrides TrueType size in motifs, fight HUDs and storyboards;
bitmap fonts keep their atlas cell sizes. `[Config] DrawShadows` controls floor
rendering. `[Misc] PauseOnDefocus`, `SFXBackgroundMode` and `BGMBackgroundMode`
control background simulation and audio. BGM `Pause` preserves the track cursor;
`Mute` advances it silently; `Play` continues normally.

Keyboard input and opt-in gamepad input share menus and gameplay. Gamepads use
portable layout names or explicit raw controls in YAML; see below. Linux
physical-controller acceptance and native joystick-number mappings remain open.

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
Options changes apply to the current session. Game Speed updates the live
scheduler immediately, and subsequent matches inherit that clock rate.

At the fight screen, Pause freezes the complete simulation; Scroll Lock advances
one tick while paused. Holding either key does not repeat. Escape still exits.
`[Input] AnyKeyUnpauses=1` lets another fresh keyboard press resume an ordinary
paused fight. Music playback continues independently of the held simulation.

In Training, Pause opens the dummy menu: choose Cooperative, AI or Manual
control, automatic guarding, stance/jumps, spacing and repeated buttons. Manual
uses P2 input; Cooperative applies the selected dummy settings; AI uses authored
CMD plans. Move with P1 directions, change values with left/right, and resume
with an attack button or Pause. M minimizes/restores the paused menu. Scroll
Lock still advances one tick. Exact native dummy movement/attack timing remains
part of fidelity work.

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

Each archive expands once for the active selection/matches. Returning to title,
normal exit, startup errors and handled termination remove its expanded files;
opening selection again reloads the source pack as needed.
The original ZIP is preserved. Packs support up to 1 GiB of expanded content and
65,536 entries. No assets or local configuration files need to be added to Git.

Invalid AIR collision-box counts report the animation file, action and line at
load time. The port limits declarations and per-element collision-box copies to
1,048,576 allocations per AIR file; this is a port resource ceiling, not a claim
about the beta's exact rejection rules.

### Language selection

Set the two-letter language code in your data root's `data/mugen.cfg`, then
restart the game:

```ini
[Config]
Language = "ja"
```

Localized groups such as `[ja.Files]`, `[ja.Info]` and `[ja.Scene 0 hello]`
replace their complete English groups. Omitted keys use their defaults; they
do not inherit English values. If a localized group is absent, the undecorated
group supplies English fallback. This applies to characters, stages, motifs,
storyboards and definition fonts, including selection and VS portraits.
Motif `select` paths resolve beside the motif before the shared data directory.

Character commands/states and individual background elements stay undecorated.
Standalone AIR banks also stay unchanged; use localized `[Files]` to select
another animation file. Victory quotes retain their language banks. Fonts must
provide the desired glyphs; the embedded fallback does not cover full CJK text.

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

`[Input] P1.UseKeyboard` / `P2.UseKeyboard` in `mugen.cfg` enable each player's
keyboard controls. YAML `input.p1.keyboard_enabled: true` (or P2/false) explicitly
overrides that setting. Omitted cfg fields enable keyboards. Escape and runtime
hotkeys remain available when a player's keyboard controls are disabled.

Supported actions are `jump` (`up`), `crouch` (`down`), `left`, `right`, `a`, `b`,
`c`, `x`, `y`, `z`, `start` and `escape`. Direction bindings also move menu cursors;
`start` also confirms selections. Names are case insensitive, and the `Key`
prefix is optional. Both players can have their own lists; assigning the same
physical key to both players activates both mappings.

Character CMD definitions control attack timing. A plain button command such as
`x` matches a new press; `/x` matches holding and `~x` matches release. Authored
`buffer.time` keeps a matched command active for that window.
CMD `[Defaults]` apply throughout the file, regardless of placement. The first
duplicate command setting or remap wins. `command.time` has a minimum of 1 tick;
`buffer.time` is limited to 1–30 ticks. Every character CMD must contain at least
one named, nonempty `[Command]` definition.

The first valid override file wins. Search order is the data root's `mugen.yaml`,
its `data/mugen.yaml`, `~/.paintown/mugen.yaml`, the working directory's
`mugen.yaml`, `game/mugen/mugen.yaml`, then `mugen.yaml` beside the executable.
Only that file is overlaid on `mugen.cfg`; YAML files are not combined.

### Gamepad overrides in mugen.yaml

Enable a controller for either player in the same local override file:

```yaml
input:
  p1:
    gamepad:
      enabled: true
      deadzone: 0.5
    joy:
      a: south
      x: [west, "button:2"]
      z: none
  p2:
    gamepad:
      enabled: true
```

Controllers are disabled by default. A valid `joy` binding also enables that player
unless `gamepad.enabled: false` is explicit. The first available controllers
fill enabled P1/P2 seats in SDK ID order. A connected controller keeps its seat
when the other disconnects; a newly connected device fills the free seat.
Assignments are session-local, so reconnecting both can change their order.
Keyboard controls remain usable alongside controllers.

Defaults are D-pad/left stick for directions, `south`/`east`/`r1` for a/b/c,
`west`/`north`/`l1` for x/y/z, `start` for confirm and `back` for escape.
Names describe button positions, independent of printed Xbox/PlayStation labels.
Other names are `l2`, `r2`, `left_stick`, `right_stick`, and
`dpad_up`/`dpad_down`/`dpad_left`/`dpad_right`. Axes require a polarity:
`left_x-`, `left_x+`, `left_y-`, `left_y+` or the corresponding `right_x`/`right_y`.
Vertical negative points up. `deadzone` must be at least 0 and less than 1;
an axis activates only beyond that magnitude.

For a device without a standard layout, use `"button:0"` or `"axis:0+"` /
`"axis:0-"` with zero-based **Ebitengine raw** indices. Standard names require
an SDK layout mapping. Lists replace an action's defaults, omitted actions
inherit, and `none`, `disabled` or `[]` disable it. Invalid lists warn and keep
the previous binding. Holding a button keeps gameplay input active and produces
one menu press until released. Adding another bound control to an already held
action does not create a fresh press.

Legacy `[P1 Joystick]` / `[P2 Joystick]` numbers are retained when parsing cfg,
but are not translated to this SDK's raw indices. The adapter has connected
scenario coverage with injected devices; physical Linux controllers still need
acceptance testing. Your existing local YAML is never rewritten.

Background music supports WAV, MP3 and Ogg Vorbis from motif, stage and storyboard
settings, with looping, sample loop points and independent music volume. NoMusic
pauses/resumes the playback cursor while sound effects continue. Linux MIDI and tracker support uses optional system libraries described below.
WAV music and SND effects accept mono/stereo PCM8/16/24/32 and float32/64,
including standard PCM/float [extensible headers](https://learn.microsoft.com/en-us/windows/win32/api/mmreg/ns-mmreg-waveformatextensible).
Music streams in bounded blocks; loop points still count source sample frames.
Physical CD playback remains unavailable. The stock stage references
sound/kfm.mid, which is absent from the supplied local distribution.

True-color backgrounds use linear filtering. For normal/parallax backgrounds,
`mask=0` forces opacity after RGB premultiplication, making fully transparent
pixels black; `mask=1` preserves transparency. Animated backgrounds stay masked.

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
view. The current checklist has32/35 implemented items (91% of checklist items, not
native conformance or remaining effort); see the explicit partial rows. The
audit added a separate input-device row for the gamepad gap, expanding the
previous 34-item checklist rather than reopening completed state mechanics.
The current Linux implementation supports one-on-one, simul/turns, arcade and
survival progression, training/watch, authored CMD and AI input, and storyboard
text/sound/windows plus AIR1.1 interpolation. Full native1.1 equivalence remains
in progress. The documented camera, sprite/shadow, alpha-composition and KO-echo
differences are closed for the checked scenes; the progress ledger records each
checkpoint. Broader content/rules/AI compatibility and physical CD audio remain.

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

Local palette effects share the mapped fighter/HUD textures without recoloring
unaffected HUD sprites. AfterImage trails retain each pose's AIR interpolation
clock and outfit across animation changes; the connected lifecycle covers Pause,
expiry/resume, remapping and round reset. One Linux scene matches all five beta
trail positions; broader rendering fidelity remains open.

Shared-palette PCX sprites may omit their palette trailer. TrueType font paths
support Windows separators, mixed-case folders and absolute filenames on Linux;
missing fonts use the reported fallback described in [font notes](docs/BUILD.md).

Legacy `.fnt` metadata accepts inline comments, variable glyph widths and
hexadecimal map characters. Malformed settings report the file and line. PCX
loading discards row padding without a second decoded image; V1/V2 archives
share a total decode limit and linked sprites share their pixels. The connected
Linux scenario covers bank changes and correction/reload of bad assets. Native
equivalence for the authored legacy font remains unverified.

Linux legacy music optionally uses installed `libfluidsynth.so.3` for MIDI and
`libopenmpt.so.0` for MOD/S3M/XM/IT. MIDI needs a GM SoundFont: set
`MUGEN_SOUNDFONT=/path/to/font.sf2`, or use an installed TimGM6mb/FluidR3_GM font.
No codec libraries or music assets are bundled. WAV/MP3/Ogg need no extra codec
installation. Missing tracks/codecs produce a sound warning; physical CD audio
is unavailable. Linux is the current validation target.
