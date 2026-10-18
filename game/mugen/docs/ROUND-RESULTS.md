# One-on-one results and reset (M4bv)

Verified after clock checkpoint 7fae8ec4. All authored changes remain under
`game/mugen/`; local commits only. The native beta distribution is immutable.
The promoted corpus contains 26 controls, 1,560 integer observations and 104
pre-native CNS/AIR/CMD/recipe hashes. Source programs and goldens are unchanged.
Three additional counterexamples preserve an open legacy HitVel trigger gate;
they are excluded from the passing count, not assigned a numeric tolerance.

## Results and clocks

Native command-line Quick-VS runs indefinitely by default. Its Win/Lose flags
and reset clocks cannot establish normal best-of-three completion. Ordinary
Versus controls instead drive the spawned window through title, both character
selections and stage confirmation. Events target that window without desktop
focus changes. Preparation pins select.def, fight/config files and character
inputs. GameTime includes menus: Match replays start at the recorded first-CNS
timestamp and compare all raw integers unchanged. Fresh native verification
allows only that external origin difference in the explicitly enumerated event
timestamp slots; missing events, durations, life, states and flags stay exact.

Straight/alternating wins, one-/two-/three-win limits, KO/time draws and time wins
have native controls. MatchOver is visible at RoundState 4, before over.wintime.
This corrects M4bu's pose-entry implementation: its nonterminal controls did not
prove normal-mode match-point timing. Win/Lose publication and committing the
tally are separate operations. The win.time 0/60 controls retain the same tally
boundary. Earlier short-interval observations were a Quick-VS mode difference,
not evidence that short over.time prevents native wins from being recorded.

At match.maxdrawgames=1 the first draw awards neither side a win; later draws
award both sides wins. Three consecutive draws finish a two-win match. Native
-1 and 0 both award wins on every draw; this beta discrepancy with the manual's
"unlimited" description remains explicit. Dead fighters keep their death states
on a draw; living time-over draws enter 175. Healing to full life before a win
allows WinPerfect; life loss before result commitment clears it.

AssertSpecial roundnotover maintains a rolling deadline 19 passes ahead, rather
than simply blocking a test of over.time. Holds of 5/30 passes with over.time
10/90 pin both extending and shortening that configured deadline. Local EXE
inspection corroborates the addition at 41530b..415314; disassembly stays in
temp/. Final result holds keep native captures reviewable.

## Late contact and self-KO

Seven recovering projectile controls locate both sides of over.hittime. The
initial KO pass counts toward this clock. A later fatal contact can replace a
provisional win with a draw before state 4 commits the tally. After the damage
cutoff, contact still enters get-hit state and exposes the raw hit damage; life
loss is suppressed at the damage consumer. Clearing HitDefs or suppressing
collision would give the wrong state and query behavior. Every control records
a successful projectile spawn and contact timestamp. Its authored get-hit state
recovers, so blocked damage does not leave an accidental 900-pass readiness
stall. Earlier missing-spawn and held-hit drafts remain preserved diagnostics.

Two self-KO controls establish another boundary: LifeAdd immediately exposes
Life=0 while Alive stays 1 for that CNS pass. The engine then enters 5030;
next-pass Alive is false. An authored ChangeState can therefore interrupt the
following negative-state list. Exact before/after values and eight subsequent
state/time/phase samples pin the implementation. Contact lethality retains its
existing hit-state path. An initial unnecessary fall flag broke the legacy idle
HitOverride control; removing it restores that original golden without edits.

Three further text controls in
[known-divergences](../tests/testdata/reference/known-divergences/README.md)
preserve native HitVel Y=-24 versus Go bottom/0. The legacy trigger is recognized
but unbound. The other 59 integers agree in each control. These observations do
not establish a physical-knockback or stored GetHitVar-vector difference. The
remaining trigger/projectile gates are unwaived.

## Reset and resources

roundreset-persistence-v2 seeds power, integer variables around persist index
30 and float variables around index 20, plus a helper, projectile and Explod.
Round two restores life, retains power 1234 and configured variable ranges,
clears lower ranges and all three entity/effect types, preserves root identity,
and rejects the retired helper ID. The first state is 5900; RoundNo and
RoundsExisted are 2/1. Bare NumProj now reads the actor's active projectile list;
this bounded consumer does not complete the projectile trigger family.

Stage resetBG reconstructs authored element/controller state between rounds,
retaining sprite archives and loaded image caches. resetBG=0 continues clocks.
Tests cover velocity/controller changes, links and retired-element isolation.
The bundled background manual supplies the specification; these state checks
make no claim of full stage-render parity. Existing pause/shake/effect/sound
reset checks also pass.

## Validation and provenance

All 26 promoted controls, all 29 preceding clock controls, the restored idle
HitOverride regression and stock KO/time/pause/sweep/reset checks pass together
under race detection (Match 71.953s). Full Character/System/Stage races pass
11.504/4.694/1.039s. Core engine/evaluator/combat/CNS races pass
1.452/1.020/1.011/1.012s. The earlier integrated run failed only the subsequently
corrected idle fall-flag assertion; it remains diagnostic, not a package pass.

Three fresh native reproductions, three rebuilt applications/six captures and
private stock PCM pass provenance. All three final application captures were
visually inspected. PCM peak/RMS are 32768/2129.41 over 300,249 stereo frames;
this proves device output, not native waveform/timing parity. Source manifest:
temp/m4bv-final-production-inputs.json (277 Go/test/probe hashes). Final audit:
temp/m4bv-final-evidence-audit.json. Application SHA256:
f12ca932c50bdc7e4a2b845a26f21b47f0d5038fc1dc74e6e25b9d14a31418a7.

The capture decoder now scans each image once for all rotating pages. All 1,740
old clock observations and 420 initial Versus observations retain exact decoded
values. A preparation bug consumed a first attribute's header separator; the
fix retains byte-identical preparation hashes for all 43 earlier promoted
controls. Rejected first-field preparations use separate archived filenames;
valid one-/three-win programs were authored and frozen independently. Native
startup changes only FirstRun and line endings, checked against reconstructed
prelaunch hashes and preserved postlaunch files.

Original rejected range/empty-helper inputs, nine Quick-VS result programs,
missing-spawn/held-hit probes and the malformed first-field runs remain in
ignored temp/ with their preparation manifests and logs. Never rerun authors or
promote these diagnostic observations into passing evidence.

M4bw complete input-driven matches/rematches and the final full scoped race
suite remain next. Exact HUD/fade/audio timing, all pause/assertion interactions,
other modes and broad recovery/controller/entity/asset/font/render gates remain
open. Typed HitDef coverage remains 81/81, not whole-engine completion.

Frozen CNS files retain trailing blank lines to preserve their verified hashes;
other staged whitespace checks pass with only blank-at-eof excluded.
