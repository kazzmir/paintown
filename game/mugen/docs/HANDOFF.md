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

Latest asset batch: AIR1.1 offset/scale/angle/blend interpolation now reaches
characters, effects, backgrounds, HUD and storyboard layers. Storyboards consume
text/font/color/typewriter/start/end, scene windows and timed SND volume/pan;
scene exit retires their audio owner. Authored/localized VictoryQuote reaches
the win screen with wrapping/window clipping. Config.Language uses the beta key.
A misplaced config field caused a compile failure, was corrected, and the
reopened segments then passed. All five segments are checked with race detection;
completed state-flow was preserved on the rerun. Scratch and clients are gone.

Next: final Linux build and real window/audio smoke. Physical CD (.da) playback
remains unavailable; all implemented file formats can run without a CD drive.
MIDI/tracker require optional system codecs (see README). Exact native visual/
audio/recovery/AI calibration remains deferred. Missing licensed Windows TTFs
have an observable fallback, so CJK glyph coverage needs an installed font.
The distribution's referenced sound/kfm.mid is absent.

Use supplied data temp/mugen-1.1b1/ in place and the isolated wrapper from
AGENTS.md for host-dependent runs. Avoid the historical app_smoke asset-copy
path; change it to a small configuration overlay before final acceptance.
