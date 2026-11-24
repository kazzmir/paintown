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

`match.Match` owns teams, active/benched fighters, world, stage, fight definition,
round phases, results, timer, camera and shared simulation timeline. Round phases
cover pre-intro, authored Intro assertions, Round/Fight announcements, active
control, pre-over, win/lose poses, reset and match completion. Reset retires
helpers/effects/command history while carrying the configured persistent data.
Each character retains its own script/animation/resource coordinate space.

External contact destinations publish their state number immediately, while entry
attributes wait through attacker hitpause. Ignorehitpause controllers can run
before that entry commits. Ordinary Pause movetime is preserved during owner
hitpause while the shared pause clock advances. Finite TargetBind duration
continues through global Pause; a live binding still follows an allowed anchor,
and the receiver refreshes its cached bound flag when its CNS resumes. Self
bindings retain their actor clock. These rules preserve native throw turning,
release and own-state recovery.

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
manual dummy controls through its pause menu. Cooperative rows control guard,
stance/jumps, axis spacing and button jam; AI has an independent level row;
Manual uses P2 input. Spacing bands are Close 0–50, Medium 100–150 and Far at
least 280 base units between fighter axes. Jam alternates six held/released
ticks; W Jump releases up while rising and presses it while descending. Auto
guard uses incoming guard flags, including helpers/projectiles. Start or M
hides/restores the whole menu; attack or Pause resumes. Escape returns to
Training selection, and a new selection resets dummy settings.

Options uses small utility rows independently of title art/fonts. Its bounded
settings, team page, confirmed defaults and session Load/Save reach gameplay and
audio. CFG team KO ordinal 0 ignores a surviving AI partner when a human Simul
leader is KOed; 1 permits that AI to continue. An AI partner's KO alone does not
end the team; co-op human and CPU teams retain their ordinary survival rules.
Team factory reset uses 200% advantage. Input F3 restores selected-player startup
bindings after confirmation. Session snapshots preserve live input edits without
writing files; portable input editing is intentional. See README for controls.

Validation uses five connected segments with source fingerprints. Completed
segments stay out of default runs until their covered source changes. Existing
flows span state execution, combat/recovery, entities/bindings, effects/pause and
full matches/modes/assets. They reuse supplied assets and delete scratch on both
success and failure. See [test segments](../tests/SEGMENTS.md).

Remaining compatibility includes physical controller/native joystick acceptance,
broader fonts/media/packs, native AI/rules and renderer/audio precision. The stock
and supplied-content flows establish their observed scope, not arbitrary content
equivalence. Later-round script stalls reproduced in the target are content
defects; no further mod repairs are planned. Physical CD, unavailable licensed
fonts and other platforms
remain deferred. Detailed current limits belong in the single progress ledger.
