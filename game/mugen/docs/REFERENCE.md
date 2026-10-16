# Reference execution evidence

The pinned distribution is labeled M.U.G.E.N 1.1 beta 1 in readme.txt
(2013-07-27). Its executable reports **M.U.G.E.N ver 1.1.0 Beta 1 P1
(2013.08.11)**. Compatibility experiments use that supplied executable and the
109-file hash manifest. The original distribution is not executed or modified.

## Wine startup, 2026-10-16

Environment: Wine 9.0, a private Win32 prefix at `temp/wine-reference`, and a
full distribution copy at `temp/reference-runs/stock`. Invocation from that copy:
`wine mugen.exe kfm kfm -s kfm`, with a 25-second timeout. Wine debug output was
captured to `temp/reference-runs/quickmatch.log`; the executable wrote
`temp/reference-runs/stock/mugen.log`. The private wineserver was stopped after
the experiment. Temporary logs/prefix/copies are excluded from commits.

Observed in the engine's own status log:

- Graphics initialized in its OpenGL mode with a 1280x720 game coordinate space.
- The default motif, fonts, sound, fight definition and Lua initialization loaded.
- Quick VS entered the gameflow loop and loaded stage, KFM CMD/CNS/common states,
  SFF, AIR, SND and palettes.
- The engine reported 3012 expressions (979 on trigger lines) for stock KFM.
- Startup reached `Game loop init` and `Match loop init` before the timeout.

This establishes that the supplied executable can load a stock quick match in
this environment. It does not establish any Go parity, a completed round,
deterministic input replay, or a semantic reference result. The help invocation
`wine mugen.exe -h` remained open until its 40-second timeout; help exit behavior
is not used as a success signal.

## Next reference work

Add authored diagnostic characters/animations in disposable run copies, fixed
input/tick scenarios, and observable assertions for expression types/evaluation
order, negative-state transitions, animation timing, and ownership. Record each
fixture's source/hash, invocation, observation and interpretation. Use exact
discrete comparisons and justified numeric/visual tolerances. Do not assume
control of the reference's RNG: the startup log reports session/match seeds but
no equivalent seed-setting mechanism has been established.
