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

## Constant catalogue, 2026-10-16

The bundled trigger docs list 74 Const names. The two authored `constants-a/b`
fixtures query them through fvars, and seven debug clipboard pages capture values
with nine significant digits. All 73 supported names match Go's retained fields
and computed fall defence multiplier; Go tests separately check declared kinds.
Stock KFM omits data.power and receives 3000, which the port now preserves.

`Const(movement.jump.changeanim.threshold)` is rejected at load with
"Invalid cnstname movement.jump.changeanim.threshold in: Const". This was observed
for stock mugenversion=1.0 and explicitly for 1.1. The dedicated
`constant-unavailable` fixture pins the rejection; a fresh verified run matches
it. The numeric B fixture leaves that slot unavailable, without claiming a value
for it. This is an actual binary/documentation mismatch in the supplied beta.

Attempting empty constants groups or Data containing only life/attack/defence
fails with "Can't load Data constants". The exact required-field set is not yet
established, so no invented defaults are derived from those failed experiments.
The successful numeric pages use the original stock KFM constants. Fresh
`constants-a-verified` and `constants-b-verified` runs reproduce selected pages.

The optional `tests/reference_text` helper runs as Win32 inside the same private
Wine prefix and reads only the known MUGEN error dialog through Win32 text APIs.
It makes no input/focus/clipboard changes. Build it with `mugen --reference-tools`;
expected-error probes require it. The runner stores complete error evidence in
its temporary run directory and checks the stable expected rejection text.

## Resource file rules, 2026-10-16

The `resource-load` fixture records a state-file marker with its clipboard in
the neutral CMD state, allowing tests to change state-file declarations. The
pinned case matrix shows these load failures: omitted/blank primary st, missing
declared st8/st9, missing sound, and missing stcommon. Missing st10/st12 files
are ignored and the character reaches its marker state. The st9/st10 boundary
also holds with an explicit mugenversion=1.1 declaration.

Cases and engine/fixture/capture hashes are in `resource-load.json`; errors were
read directly from the private MUGEN error dialog. The runner accepts --file and
--omit-file when authoring, and --case when verifying a pinned matrix entry.
This evidence corrects the Go loader's arbitrary scan/break behavior: all ten
recognized extra state-file slots must be considered, even after gaps. Additional
merge precedence, motif/common fallback and palette behavior still need probes.

## State resource precedence and blank files, 2026-10-16

`resource-order` records marker 111 from primary st with st/st0 declarations in
either order, and marker 222 from st0 over st9 with reversed declarations and
sparse slots. Its main cns marker 900 never runs when only listed as cns.
`resource-priority` records primary marker 111 over CMD marker 333. These establish
fixed first-definition precedence across state files. Same-file duplicates are established separately by the state-binding fixture
below. The harness hashes and copies a
fixture's authored .extra directory and pins --state-file-order on replay.

Additional resource-load cases show blank st9 and blank sound succeed. Common
cases for blank, omitted and explicit stcommon reach common state 0. The initial
probe entered 5900 as soon as RoundState=2; the round's later control grant made
its ctrl observation differ from an already-running Go host. Requiring ctrl as
well before the forced transition removes that interference. Go begins at the
same controlled point, and all twenty resource cases compare directly. Earlier
known_go_differences were removed. Production round-control timing is still open.

The real character loader consumes these cases and reports missing/malformed
resources with DEF key/line plus resource/state source. Shared-resource search
now has explicit roots, but the exact native character/motif/data priority needs
separate conflicting-file probes. Selected external/internal palettes and
non-character load failures remain reference work.

## Transition parameters and entry precedence, 2026-10-16

The authored entry-control sequence runs ordinary states 700..706 and records
24 variables in two pages. Conflicting transition ctrl=0 / Statedef ctrl=1 yields
1, and the reverse yields 0. Statedef anim=9990 wins over transition anim=9991.
Omitted destination parameters preserve transition values. An entry animation
expression conditional on ctrl sees the transition's incoming control=0 and
selects 9991, even though the preceding state had control=1. Both the pure host
and Character match every recorded value after applying transition values before
entry evaluation. This supersedes the M2b override-wins interpretation. The
fresh ordinary-entry-parameters-replay run reproduces both pages. These probes
compare animation IDs, not additional animation-clock boundaries or owner rules.

## State block binding, 2026-10-16

The state-binding fixture defines state 0 twice. In the first block, a controller
labeled State 999 sets var(0)=111 and State 0 sets var(1)=222. The duplicate block
attempts 333/444. The reference records 111/222/0, demonstrating cosmetic header
numbers and first-definition selection. The separate state-orphan fixture puts
a controller before any Statedef and records zero, showing it is ignored.

CNS parsing now uses the preceding definition instead of indexing by controller
labels, and keeps duplicate blocks detached. Pure engine and Character tests
match all recorded variables. The state-binding-replay run reproduces the trace.
These cases do not establish whether invalid expressions in discarded duplicates
are diagnosed at load, or every accepted header spelling/range.

## Go production startup inspection, 2026-10-16

The Go application (not the reference executable) now has an owned-window smoke
runner. KFM startup captures show the Elecbyte logo, intro background, and title
menu with readable fonts and separate KFM title sprites. The successful isolated
runs are temp/app-runs/kfm-startup1 and kfm-startup3. A concurrent-test repeat,
kfm-startup2, exited with code 0 after two captures; no cause is established.
The runner records failures rather than accepting missing windows. These images
are inspection evidence, not native pixel/timing comparisons. Original stock
content remains unchanged, and captures/binaries/copies are untracked.

## Next reference work

Add authored diagnostic characters/animations in disposable run copies, fixed
input/tick scenarios, and observable assertions for expression types/evaluation
order, negative-state transitions, animation timing, and ownership. Record each
fixture's source/hash, invocation, observation and interpretation. Use exact
discrete comparisons and justified numeric/visual tolerances. Do not assume
control of the reference's RNG: the startup log reports session/match seeds but
no equivalent seed-setting mechanism has been established.
