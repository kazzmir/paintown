# Current handoff

Use [PROGRESS.md](PROGRESS.md) as the single completion ledger and
[WORKFLOW.md](WORKFLOW.md) as the implementation/test policy. The user authorized
remaining functional work on 2026-11-07 and deferred precision. Validate Linux
only for now; other builds are deferred. Continue local source/text commits,
no pushes, committed binaries, generated banks or historical native matrices.

Recent commits: c3422f586 BGM/NoMusic/StageVar; 7c026ae2a simul/turns and team
queries/UI; 7017e8655 physical CMD motion/chord/charge/strict input and authored
AI planning. All89 controller and127 trigger families have runtime bindings.

The latest batch connects ordered arcade opponents, configured difficulty ramps,
MatchNo, victory→next opponent, loss→continue/retry, character/default ending and
credits, survival carry/results, training timer/KO/refills and watch AI. Team
power limits share the largest simul capacity. Turns/survival have functional
time-scaled recovery; its provisional27.5% maximum-health baseline comes from
[compatibility defaults](https://github.com/ikemen-engine/Ikemen-GO/blob/develop/src/resources/defaultConfig.ini),
not measured beta evidence. Exact recovery amounts/rounding and native AI choices
remain explicitly deferred calibration; do not claim native equivalence.

Four affected segments pass with race detection:18 top-level connected scenarios
across three packages. The new mode flow completes real rounds through continue,
ordered victories/ending, survival recovery/results and training/watch rules.
A font-array adapter compile error was fixed; the failed segment then passed.
All clients/scratch are gone; completed state-flow was not rerun.

Next: storyboard text/sound/window and AIR1.1 interpolation consumers, then
final Linux build/playable acceptance. Linux MIDI/MOD/S3M/XM/IT now render into
the shared mixer through optional system FluidSynth/libopenmpt (stable C APIs,
no headers or copied libraries). Set MUGEN_SOUNDFONT for a custom GM .sf2;
installed system fonts are discovered by default. Other targets and physical CD
playback remain unavailable/deferred. The supplied sound/kfm.mid is absent.

The connected music flow passed synthesis→NoMusic pause/resume→sample loop→
tracker replacement/loop→close, using tiny auto-deleted scratch songs. Both
reopened segments passed with race detection; completed others stayed checked.
Decoder setup runs outside the audio lock; replacement/close frees native
objects. No native audio driver/thread is created by the port.

The supplied data is temp/mugen-1.1b1/. Use the isolated wrapper from AGENTS.md
for host-dependent runs, serial processes and automatic scratch cleanup.
