# Fatal guard sound, spark and pause controls

M4bd is a test/evidence checkpoint following M4bb's lethal-contact implementation.
It adds no production behavior. Reversal/predecessor: `54d8c86e` (M4bc).
Twelve original programs compare fatal chip, ordinary fatal hit, safe chip and
chip protected by `guard.kill = 0` against the immutable 1.1 beta 1 executable.
They record 144 numeric observations and 40 authored source hashes.

## Native findings

Each family uses receiver life 5. Fatal guard uses damage `1,10`; ordinary hit
uses `10,1`; safe guard uses `10,1`; protected guard uses `10,10` and disables
guard lethality. Fatal guard normalizes reaction/cues to an ordinary hit while
retaining the chip damage and guard.kill decision established in
[HIT-LETHALITY.md](HIT-LETHALITY.md).

| Observation | Fatal guard / fatal hit | Safe / protected guard |
| --- | --- | --- |
| Receiver state | 5000 | 150 |
| Guarded context | 0 | 1 |
| Contact tone | 1323 Hz hit tone | 1764 Hz guard tone |
| Existing channel 0 voice | Interrupted | Preserved |
| Spark action | Own 9992 | Own 9993 |
| Spark frame anchor in 240p units | (-50, -50) | (-110, -50) |
| Attacker contact-hold passes | 8 | 3 |
| Initial receiver hitshaketime | 11 | 3 |

The pause programs explicitly select hitpause `7,11` and guardpause `2,3`.
An ignorehitpause counter observes seven/two frozen passes plus the resume pass;
these are not eight/three frozen ticks. The receiver shake is sampled at first
contact. A one-shot Time >= 8 snapshot with a sentinel confirms hit/guard
counters, state, life and fall context after the hold. This does not establish
absolute KO slowdown or round-transition clocks.

Audio programs use the original owned tone bank, an empty owned HUD sound bank
and `nokosnd` to isolate contact output. The pre-existing 441 Hz channel 0 and
882 Hz channel 1 loops distinguish interruption from global silence. Four
native segments per program give 16 stable segments. Twenty selected Go PCM
windows compare 160 stereo amplitudes using the existing tolerance of 8.

Spark programs use the same owned RGB ramp for both actions, with AIR X offsets
0 and 60 as an independent spatial discriminator. Native marker measurements
preceded assignment of action metadata. Four native cores contain 276 pixels
each and share SHA256
`5b077cf1947ce2dbe811076af1f3549be4013f1becd5a8148108fc8237a1c238`.
The Go semantic replay selects the correct action/resource/anchor. It now also
honors the pinned language version, rather than relying on an implicit default.

## Verification and open production rendering failure

- Four new PCM replays pass; all 11 spark semantic cases and four Match pause
  programs pass under race checks. Logs: `temp/m4bd-audio-integrated.log` and
  `temp/m4bd-cues-integrated.log` (Character 1.771s / 1.321s; Match 3.918s).
- Full Character race regression passes in `temp/m4bd-character-race.log`
  (11.149s), including all 33 PCM programs and 11 spark cases. The full scoped
  suite was last run at M4bb (`temp/m4bb-full-race.log`, Match 618.564s).
  M4bc's scoped production/stock checks also pass; M4bd changes tests/data/docs.
- Fresh native fatal audio, protected spark and fatal pause reproductions pass
  with frozen inputs/goldens: `temp/m4bd-validation.log` and its replay logs.
- Four production apps/four captures pass prepared-resource, config, binary,
  PNG and runtime provenance checks. Two private-device PCM captures compare
  16 stereo amplitudes with maximum measured delta 0.0. Fatal guard selects the
  hit tone and interrupts channel 0; protected guard selects the guard tone and
  preserves it. This tests cue output, not full KO clock alignment.
- **Two strict spark raster comparisons fail.** Fatal and protected anchors
  match, but Go's blue-core bounds are one column wider on the left: native
  `(467,450)-(490,462)` versus Go `(466,450)-(490,462)` for fatal; native
  `(287,450)-(310,462)` versus Go `(286,450)-(310,462)` for protected. Each
  native 276-pixel core differs at all 276 pixels, maximum channel delta 8.
  Go's 288-pixel core SHA is
  `2c55c6b0e790d626fa7c7a6ff42ae51c0fa5969efc45a19b843be1974ef94753`.
  The cause has not been isolated. No golden, crop or tolerance was relaxed.
  These failures are additional to M4az's six open alpha precision cases.

The audit is `temp/m4bd-final-app-audit.json`; its `exact_marker = FAIL` entries
remain open gates despite successful provenance and cue selection. All four
apps use binary SHA256
`48624d81792daa306e8c9b4af585877014a51e7f581d9f46d85ed7cb659d1b9e`.
Final audio app names end in `-go-m4bd-verified`, captured at 5s; spark apps end
in `-go-m4bd-cue`, captured at 3s. The fatal spark capture was visually inspected.

Longer pilot captures are retained as diagnostics: the minimal fatal cue
fixtures omit winner state 180, so production exits when it requests that
state. The first audio pilot at 7s and spark pilot at 5s are not accepted full
match smoke evidence. Shorter captures observe contact before that boundary;
they do not prove lifecycle support for these deliberately minimal characters.
Stock lifecycle checks are documented separately in [MATCH.md](MATCH.md).

Root go.mod/go.sum integrity checks pass. The original reference executable
remains SHA256
`d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e`.
Only original fixture sources, owned SFF/SND markers, tests and documentation
are committed. Native art/binaries, screenshots, raw PCM and run copies stay
ignored under `temp/`. No production source was changed in this checkpoint.
