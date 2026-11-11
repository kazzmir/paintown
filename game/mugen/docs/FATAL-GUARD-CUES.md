# Fatal guard sound, spark and pause controls

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

Spark programs use the same owned RGB ramp for both actions, with AIR X offsets 0
and 60 as an independent spatial discriminator. Native marker measurements
preceded assignment of action metadata. The Go semantic replay selects the correct
action/resource/anchor. It now also honors the pinned language version, rather
than relying on an implicit default.

## Verification and open production rendering failure

Final audio app names end in `-go-m4bd-verified`, captured at 5s; spark apps end
in `-go-m4bd-cue`, captured at 3s. The fatal spark capture was visually inspected.

Longer pilot captures are retained as diagnostics: the minimal fatal cue
fixtures omit winner state 180, so production exits when it requests that
state. The first audio pilot at 7s and spark pilot at 5s are not accepted full
match smoke evidence. Shorter captures observe contact before that boundary;
they do not prove lifecycle support for these deliberately minimal characters.
Stock lifecycle checks are documented separately in [MATCH.md](MATCH.md).

Root go.mod/go.sum integrity checks pass. Only original fixture sources, owned
SFF/SND markers, tests and documentation are committed. No production source was
changed in this checkpoint.
