# ForceFeedback — supplied beta Null operation

The supplied M.U.G.E.N 1.1 beta 1 lowers ForceFeedback to the same controller
kind as Null. Its optional waveform/time/freq/ampl/self fields are not parsed,
validated or evaluated. Ordinary controller triggers still run. The Go dispatcher
now returns without calling a host/device hook; the Character compatibility
entry point documents the same behavior. No gamepad vibration is added.

This is a measured limitation of this exact beta target. Neither an absent
input device nor the native recorder's nojoy option establishes that conclusion.
The target executable SHA and compiler identity bytes are pinned in
tests/testdata/reference/forcefeedback-native-ledger.json.

## Native evidence and version scope

18 original programs were frozen before observation, with 80 source/probe hashes.
All were accepted, capturing 216 numeric values. Every native-only row and the
review sheet were inspected before promotion/implementation. Configurations
exactly finalize FirstRun1->0 and LF->CRLF. Sources/captures/goldens are immutable.

Waveform sine/square/sinesquare/off/unknown, bare-float/malformed/unknown-trigger
time, frequency/amplitude/self assignments, unknown fields, a malformed Null
control and a1.0 character profile all load. Optional parameter assignments stay
zero. A trigger assignment advances Var0 to9 by Time8: the controller's generic
trigger path is active even though its operation is Null.

Static identity supports the runtime observations: successful ForceFeedback and
Null name comparisons store the same ESI=0 value at controller kind+0x10
(at0x447f5a and0x4485c9), then jump to the same0x4472c2 continuation without
parameter parsing. The common0x444d70 call checks minimum language version; it
is not an execution dispatcher. Do not transfer this Null behavior to deprecated
GameMakeAnim/MakeDust or to other engine versions.

## Counterexample, validation and limits

The existing Character stub happened to do nothing, so all18 numeric baseline
replays already matched. A device-recording host exposed the real dispatch bug:
the old handler called its ForceFeedback hook once in both1.0/1.1 profiles.
The regression now verifies zero device calls while preserving trigger assignment.

Focused races pass846 engine/character results and119 native Match results
(ForceFeedback plus all98 shadow programs). Four actual application crops match
exactly over2,035,200 RGB pixels, including the real1.0 production loader profile.
Stock21/26/45 screenshots show win → rematch → win;82balanced owned keyboard
events and1,912,813 private PCM frames pass. Module/inventory/provenance checks
pass. The audit pins17,409 frozen paths and one final source/config/binary lineage.

The replay harness adds an explicit optional state_language_version field,
defaulting to its previous1.1 behavior, and excludes native metadata ledgers from
program globs. The first baseline count failure (20 files versus18 programs) is
preserved as a harness diagnostic, not a native behavioral rejection.

The latest full scoped race remains M4da's4,506 results/36 packages. This focused
checkpoint changes only the Null-operation device boundary and its evidence.
Earlier shadow/angle/EnvShake strict differences remain UNWAIVED. Source routes
remain74 typed/15 fallback; two stubs, nine partial,18 bounded and60 route-only
families. No overall engine completion percentage follows from those counts.
