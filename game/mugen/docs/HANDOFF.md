# Current handoff

Follow [AGENTS.md](../AGENTS.md). Author only game/mugen/ and preserve unrelated
work. Local source/text checkpoints are authorized; no pushes, committed assets,
binaries, user configuration, subagents or goals. Keep documentation portable;
[PROGRESS.md](PROGRESS.md) is the single completion ledger.

K1–K7, U1 runtime interaction and U3 Options/Training acceptance remain closed.
K8/U4 now has an implemented weighted fallback: persistent movement and physical
button toggles, nearest live opponent in game-axis units, difficulty/time/range
based defence, incoming guard-height adjustment, confirmed-hit attack/cancel
weights, separate CMD-label admission and falling-hit recovery chords. Ordinary
CNS owns move eligibility, resources, guarding and CanRecover. Non-cheating timed
CMD plans and independent AI random streams remain explicit port policies.

One input snapshot serves the root and opposite-facing key-controlled helpers.
Human/disabled/KO ownership, absent opponents and changed CMD/level/policy discard
stale held input/plans. Seat random streams continue across rounds, ownership
changes and reload; reload copies them without sharing the frozen old match.
Training AI uses the same facing-aware adapter; cooperative/manual dummy input
remains separate. The existing connected command/combat and stock input paths
cover the AI contexts, helper ownership, actual attacks/contact, reset and reload.
Affected connected segments pass with race detection; the unchanged state-flow
segment retains its acceptance. Linux build and AI-versus-AI window smoke pass.
Owned runtime prefix, overlays, binaries, captures and scratch have been removed;
external data, user YAML, root module metadata and shared build cache are preserved.

Direct target handler review and one live combat observation establish an
additional planner layer inside K8: move tables populate during combat and feed
neutral/movement, pose-aware attack selection and predicted guard/counter paths.
Those learned metadata consumers are not reproduced. Next resolve table
population, command-index association, pose/range/startup prediction and planner
transitions as one bounded lifecycle, then implement and accept that consumer.
Do not create an AI-hints asset reader from a load message; the public sample
labels its hints file unused. Fallback acceptance is not complete native AI
acceptance. Exact random sequences are not promised by the port's separate streams.

U2 needs broader licensed font/content inputs; U5 needs a named precision
mismatch; H1 needs physical hardware exercise. All remain open alongside U4.
Unknown permutations are not new mandatory matrices. Preserve closed result,
Options/Training, contact/Pause/binding, turning, camera, rendering and audio
calibrations. Do not repair scripts that fail in the target; bundled runtimes
may differ despite familiar banners. Paintown C++ turning is not normative.

Validate affected connected segments at coherent batch boundaries; completed
unaffected segments stay skipped. Use the scoped module overlay/shared cache.
Native/application/display runs stay serial; await owned clients before edits or
another run. Clean only owned scratch and preserve external data/user files.
Physical CD, unavailable licensed/CJK payloads, other-platform runtime acceptance
and standalone music-file CLI playback remain deferred.
