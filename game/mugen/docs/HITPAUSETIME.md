# HitPauseTime binding (M4cc)

Seven frozen native programs pin 84 scalar observations and 28 authored
CNS/CMD/AIR/stage hashes. The integer trigger reads the attacker's own hitpause
counter, clamped to zero for negative stored pause values. It does not read the
defender's separate hit-shake counter. For pausetime=5,8, ignorehitpause CNS
observes 5,4,3,2,1,0; a guarded contact with guard.pausetime=3,6 gives
3,2,1,0,0,0. Normal controllers resume at zero. Explicit zero/negative durations,
Enemy redirection, helper versus owner and a global Pause interaction are pinned.
A helper's attack pause is observable through Helper(9) but does not pause its
root. Existing pause scheduling is preserved; this checkpoint adds the missing
binding and distinguishes the two clocks.

Tests: TestNativeHitPauseTimeBinding replays all seven immutable programs;
existing hit-pause expression/fatal-guard controls and the new controls pass
focused Match races (19.970s). Full Character/evaluator races pass
11.615/1.021s, including integer type and negative-boundary checks. A fresh helper
capture reproduces all 12 values. All 321 frozen source/probe/fixture/module
hashes and authored source/PNG/glyph audits pass. Artifacts start temp/m4cc-*;
native originals and fresh run are under temp/reference-runs/m4cc-*.
No numeric golden or tolerance changes; binaries/captures remain ignored.

Runtime family routing is now 110/127 (86.6%), with 17 unbound. This is source
routing rather than complete variant conformance. Broader third-party/custom-
state/Pause/SuperPause interactions remain part of their interaction matrices.
The scalar/projectile/reversal allocation continues with Random/VarRandom.
Final full scoped integration follows the allocated integration boundaries;
M4cb remains the last full scoped suite at this checkpoint.
