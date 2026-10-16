# Reference execution evidence

The pinned distribution is labeled M.U.G.E.N 1.1 beta 1 in readme.txt
(2013-07-27). Its executable reports **M.U.G.E.N ver 1.1.0 Beta 1 P1
(2013.08.11)**. Compatibility experiments use that supplied executable and the
109-file hash manifest. The engine runs from a copy. Read-only extraction and
compression tools write only to disposable output directories; the original
distribution is unchanged.

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

## Sprite extraction and compression, 2026-10-16

The bundled `sff2png.exe` identifies as SFF2PNG v0.2 (2010-07-12). Invoked with
`chars/kfm/kfm.sff` and an output prefix in `temp/reference-runs/extract-kfm`,
it exited successfully and wrote 281 indexed PNGs. The Go decoder matches all
281 opaque RGBA pixel hashes; axes/group/item are checked against source headers.
The recorded oracle is `tests/testdata/kfm-sprites.json`, including source/tool
hashes. Transparency masking is tested separately because the tool exports
opaque palettes. This is decoder evidence, not animation or fight parity.

The bundled `sprmake2.exe` identifies as SprMaker 2.01beta (2010-06-16). It
successfully encoded six original patterned sprites to exercise RLE5, PNG8/24/32
and uncompressed RGB/RGBA. The fixture is `sff/testdata/formats.sff`; its
definition and standard-library Python input generator accompany it. The Go
decoder matches every visible pixel/alpha against the input formulas. RLE5
runs encode length minus one, unlike the incomplete C++ reader's implementation.
Raw RGB/RGBA bytes are in channel order without a decoded-size prefix; compressed
formats include that prefix. These findings have focused regressions.

## Animation loop trace, 2026-10-16

`tests/reference_probe.py` prepares a disposable distribution copy with the
authored `tests/testdata/reference/animation-loop.cns` and `.air`. Stock KFM
constants/assets/commands remain available; the command state is replaced by a
neutral Null controller so it cannot overwrite trace variables. The 2/3/4-tick
action loops at element 2. CNS records its first 30 ticks in player variables,
then displays two six-parameter lines per capture (the documented printf limit).

Four captures from `temp/reference-runs/animation-loop-clean2` supplied 48
encoded observations covering 24 ticks. Exact matching against the bundled
f-4x6 debug glyph masks reads the numbers, and captures were inspected visually.
The fixture JSON records source/engine/capture hashes and decoded tick values.
The shared Go clock matches all of them, including three loop-boundary ticks.
This establishes the finite-loop timing used by those assertions, not complete
animation or rendering parity.

The finalized runner reproduced pages 0 and 30 in a fresh
`temp/reference-runs/animation-loop-verified` run with all assertions passing.

The automatic runner checks hashes and numeric observations. It requires
Wine, DISPLAY, xdotool, ImageMagick import, and Pillow, and captures only its own
spawned process window. It sends no keyboard/mouse events or focus changes. The
private prefix is shared with these reference runs and must not be used by an
unrelated Wine process. Each invocation creates a fresh named run directory.

Exploration found that Lua player query exports exist, but the bundled Lua
wrapper scripts do not expose a usable per-tick capture hook at initialization.
Input/console attempts did not produce a reliable trace; they are not part of
the runner. Clipboard captures provide the reproducible route used here.

## State order and persistence, 2026-10-16

The named `state-order` fixture records 32145, showing -3/-2/-1 followed by a
two-state positive chain. Counters after each ChangeState stay zero; the final
state observes StateNo=31, PrevStateNo=30 and Time=0. This resolves the earlier
C++ comment ambiguity for ordinary self-owned negative states. Ownership and
helper eligibility are separate outstanding cases.

The `persistence` fixture records intermittent conditions true except at ticks
1,4,7,10. Persistent=2 activates at 0,2,5,8,11 and evaluates its side-effecting
trigger eight times. Persistent=0's side-effecting trigger evaluates once. This
disproves the earlier interpretation that N counts only successful conditions
and that one-shot trigger expressions continue after activation. The executor
now applies a pre-trigger cooldown/one-shot gate. Negative-state re-entry and
pause interactions still need probes.

Numerical evidence and engine/source/capture hashes are committed alongside the
authored fixtures. Fresh `state-order-verified` and `persistence-verified` runs
reproduced every observation. The production Character and headless state-pass
host execute the same programs against those numerical assertions.

## Next reference work

Add authored diagnostic characters/animations in disposable run copies, fixed
input/tick scenarios, and observable assertions for expression types/evaluation
order, negative-state transitions, animation timing, and ownership. Record each
fixture's source/hash, invocation, observation and interpretation. Use exact
discrete comparisons and justified numeric/visual tolerances. Do not assume
control of the reference's RNG: the startup log reports session/match seeds but
no equivalent seed-setting mechanism has been established.
