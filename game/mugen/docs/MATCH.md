# Match and runtime overview

Use [PROGRESS.md](PROGRESS.md) for completion status and
[README.md](../README.md) for builds, launch options, keyboard/gamepad overrides
and adding content. This overview describes the current runtime; older checkpoint
research is available in Git history, not a second implementation checklist.

The launcher connects title, options, character/team/stage selection, versus,
fights, results and rematches. Arcade and co-op campaigns schedule opponents,
victory, continue and endings. Survival carries the surviving roster forward;
training protects against KO and refills life/power; watch and idle demos use
AI input. Names, selected roster entries, palettes, storyboards, music and stage
settings travel with the request. Archive expansions belong to the engine and
are retired after audio readers stop using them.

A direct production match from the repository root:

```bash
game/mugen/mugen.sh --game --data-dir game/mugen/temp/mugen-1.1b1 --quick-match chars/kfm/kfm.def,chars/kfm720/kfm720.def --stage stages/kfm.def
```

`match.Match` owns teams, active/benched fighters, world, stage, fight definition,
round phases, results, timer, camera and shared simulation timeline. Round phases
cover pre-intro, authored Intro assertions, Round/Fight announcements, active
control, pre-over, win/lose poses, reset and match completion. Reset retires
helpers/effects/command history while carrying the configured persistent data.
Each character retains its own script/animation/resource coordinate space.

The shared actor pass resolves contacts from the previous active definitions,
then runs ordered CNS, damage continuation, physics and clocks. Helpers join the
same world; body push follows actor updates. Borrowed states retain their owner
and geometry context. Keyboard and opt-in gamepad adapters feed logical inputs
through CMD remaps, charge/motion/chord recognition and authored state controllers.
AI uses the same command/CNS path with timed plans.

The renderer composes stage backgrounds, fighter/helper/effect layers, floor
shadows/reflections, HUD, foreground and global palette effects. It uses actual
SFF/AIR/fonts for portraits, bars, timer, announcements and win icons. Music and
SND voices share the production audio service; NoMusic and background pause/mute
policies retain their separate cursor semantics.

User Pause holds the complete match, HUD and CNS GameTime while input/audio
continue. Scroll Lock advances one pass. Training exposes cooperative, AI and
manual dummy controls through its pause menu, including guard, stance, jumps,
spacing and button jam. M minimizes/restores the menu; attack or Pause resumes.
Exact native dummy timing/menu parity remains unverified.

Validation uses five connected segments with source fingerprints. Completed
segments stay out of default runs until their covered source changes. Existing
flows span state execution, combat/recovery, entities/bindings, effects/pause and
full matches/modes/assets. They reuse supplied assets and delete scratch on both
success and failure. See [test segments](../tests/SEGMENTS.md).

Remaining compatibility includes physical controller/native joystick acceptance,
broader fonts/media/packs, native AI/rules and renderer/audio precision. The stock
and SF2 flows establish their observed scope, not arbitrary content equivalence.
Original SF2 later-round script stalls also reproduce in native; no further mod
repairs are planned. Physical CD, unavailable licensed fonts and other platforms
remain deferred. Detailed current limits belong in the single progress ledger.
