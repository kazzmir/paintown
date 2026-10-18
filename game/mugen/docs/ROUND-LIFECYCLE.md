# KO and round lifecycle allocation

Approved 2026-10-18 after bbba565e. All authored changes remain under game/mugen/;
local checkpoint commits only, no binaries/reference artwork or remote push.

| Checkpoint | Scope | Acceptance |
| --- | --- | --- |
| M4bu | KO/time-over clocks, slowdown, timer freezing, pauses, control and pose entry | Frozen native traces agree with production Match, focused regressions, rebuilt native/application checks. |
| M4bv | Draw/double KO, wins/match completion, reset/persistence/cleanup | Native result/reset traces and retained state/resource checks; stock multi-round scenarios. |
| M4bw | Complete production matches driven by inputs/damage, results/rematches and sustained resets | Repeatable end-to-end scenarios and final full scoped race suite, with artifact provenance. |

Baseline source findings: Match.Update has no KO slowdown scheduler; over phases
use coarse wait/win/over delays. finishRound publishes MatchOver immediately,
whereas the beta manual says it appears at win-pose entry. Existing KO tests
set life at phase boundaries and demonstrate lifecycle wiring, not natural
end-to-end combat or exact native clock parity. Reset reconstructs actors and
preserves power/configured variable ranges, but full reset/background/draw
rules still need evidence. These are the pre-M4bu baseline findings; the verified
clock changes are recorded below.

The target is one player per side. All previous controller/entity/recovery/
mode/asset/render gates remain unwaived. Typed HitDef coverage stays 81/81;
this work closes runtime obligations rather than increasing that key count.

## M4bu: verified KO/time-over clocks

Predecessor/reversal bbba565e. 29 frozen native programs pin 1,740 integer
observations and 116 pre-native CNS/AIR/CMD/recipe hashes. All source programs
and numeric goldens remain unchanged. Preparation manifests under ignored temp/:
m4bu-prepared-{stable,extra,clock-boundaries,integrated-clocks,readiness,single-hit}.json.
Two unpromoted held-hit prototypes are explicitly excluded; original files and
observations remain in m4bu-unpromoted-clock-drafts. Initial constant/capture
experiments remain diagnostic and do not count as acceptance.

The native uses an integer KO debt. Step/debt begin at 500 on the first post-KO
pass. Each outer tick adds the step if debt<=0, otherwise subtracts 100; simulation
runs when debt<=0. Before CNS at phase counts 5, slow.time/2, slow.time*7/10 and
slow.time*9/10, step/debt become 200/100/50/25. The slow duration decreases once
per simulation pass; zero duration, nokoslow or state4 disables it. GameTime
advances on skipped frames while state/animation/physics clocks remain tied to
simulation. Controls for durations0/6/12/60, shifted KO times, long/short waits,
late state/animation clocks and Pause/SuperPause verify the model. Local EXE
inspection corroborated these rules; disassembly/binaries remain ignored.

RoundState3 exposes KO results immediately. After over.waittime simulation
passes, ready actors lose control and enter RoundState4. Win/lose poses follow
over.wintime later; over.time counts from state4 rather than pose entry.
MatchOver is published at pose entry. Ready means a living controllable S/I
actor or state5150 with Time>0; the native eventually forces progress at 900
pre-over passes. Both readiness stalls and that timeout have bounded controls.
The next round has one initialization tick before its first negative-state
pass; power/persistence/reset details continue in M4bv.

The active timer counts through its displayed zero interval, with a measured
initial count offset 2. Time0/1/2, framespercount10/60, ctrl.time0/10/30 and
unlimited time have controls. Timerfreeze and Pause/SuperPause delay active
expiry; hitpause does not. Post-KO phase and slowdown clocks continue through
Pause/SuperPause while character execution is frozen. Time-over result flags
appear one CNS pass after RoundState3. Win/Lose, their KO/time suffixes,
WinPerfect and DrawGame now have typed root-result bindings; broader result
rules are M4bv work.

Legacy scalar fixtures count completed simulation passes, preserving their
original sampling intent across new KO skips. HUD animation/cue updates skip
suppressed simulation frames. The first flag design broke direct HUD use; an
explicit default-false SimulationSkipped flag fixes that, with existing cue/
native PCM regressions passing. Artificial lifecycle KO setups now supply a
valid dead pose. The old time-over test now observes the native one-pass result
delay. Full input-driven wins are reserved for M4bw, not inferred from those
isolated setups.

Final checks pass: 29 native programs plus the time-over regression36.064s,
previous intro controls 4.719s, Character 11.873s, System 4.773s and core races
(engine/evaluator/CNS/combat 1.528/1.022/1.015/1.012s). Older lethality/fatal-guard,
stock sweep/reset and pause cases passed in a run whose sole obsolete time-over
assertion was subsequently corrected; that failed package log is retained.
Two fresh native replays and three rebuilt applications/six captures pass
provenance. Stock and KO captures were visually inspected. Source manifest:
temp/m4bu-final3-production-inputs.json (274 Go/test/probe hashes). Audit:
temp/m4bu-final-evidence-audit.json. Binary SHA256:
5bd9bd3d5ba6527641645d23f4b44e01bb75952e21a52a4ff1a546115b98baef.

Native startup rewrites FirstRun and line endings. The audit reconstructs the
exact hashed prelaunch recipe and verifies that the preserved postlaunch file
has only that change. No gameplay setting or golden is relaxed. Fresh verifier
initially compared integer dictionary keys with JSON strings; the failed run's
60 values all matched independently, and corrected fresh reproductions pass.
The exact glyph matcher now scans the same bottom 80 rows without scanning the
unused full window; independent old/new captures retain every original value.

These controls establish one-on-one simulation clocks and pose/control entry.
Complete HUD component time/displaytime/fade and audiovisual synchronization,
all assertion/pause combinations and broader entity/mode rules remain open.
No whole-frame/font/audio timing parity is claimed from smoke captures. Full
scoped race integration follows M4bw; the previous full run belongs to M4bt.
Continue M4bv and M4bw automatically, with local commits and no push/binaries.

The staged whitespace audit retains trailing blank lines in the frozen native
CNS inputs and source snapshot rather than changing their verified hashes.
All other whitespace checks pass (blank-at-eof excluded for this checkpoint).

## M4bv correction and continuation

M4bu was committed as 7fae8ec4. Its nonterminal controls did not prove the
terminal MatchOver timing. M4bv ordinary Versus controls correct the implementation
to publish MatchOver at RoundState 4, before pose entry. Results/reset, late
contact and persistence now pass 26 native controls / 1,560 observations; see
[ROUND-RESULTS.md](ROUND-RESULTS.md). The final complete-match checkpoint M4bw
and full scoped integration remain authorized and next.
