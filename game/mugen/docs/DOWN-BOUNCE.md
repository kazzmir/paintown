# Downed-hit bounce

M4bm adds `HitDef.down.bounce` expression routing and the native downed-hit
consumer. This is bounded own-state/root evidence, including the stock common
reaction path, rather than complete fall/recovery physics compatibility.

## Native rules and implementation

`down.bounce` defaults to zero. Integer literals/expressions accept any nonzero
value as enabled; runtime floats truncate toward zero and bottom becomes zero.
Floating literals and multi-component tuples fail loading. Unknown triggers
fail Go compilation before earlier controller effects. Full forward/reversed
source programs evaluate this key after `air.cornerpush.veloff` and before
`down.hittime`; declaration order does not determine expression side effects.

For a downed receiver with nonzero down Y velocity and bounce disabled, contact
replaces the stored fall X with the preserve-X sentinel and fall Y with zero.
Consequently HitFallVel preserves current X and sets Y to zero. With bounce
enabled, stored fall velocities remain available. This rule applies with both
`fall=0` and `fall=1`. Zero down Y leaves fall velocities unchanged for either
bounce setting. Standing/airborne controls retain their fall vectors. Explicit
mixed-profile vectors convert once into receiver units. The contact copy does
not mutate the attacker's HitDef or velocity slices.

The stock bounce control exposed a separate tick-latch defect: Go cleared
PosFreeze on ChangeState. Native state 5100 asserts PosFreeze before changing
to 5101; its Time=0 velocities are visible but movement remains frozen for that
tick. Go now preserves that latch across transitions and clears it at the next
active tick. Common idle/crouch/walk turning also waits until after the entry
tick, preserving the heading seen by the first idle negative-state pass. The
additional native stock trace pins these positions, velocities and heading
observations without expanding the claim to all auto-turn eligibility.

The stock sensor records first observations in negative state -2:

| State | Bounce 0 | Bounce 1 |
| --- | ---: | ---: |
| 5080 | 0 | 0 |
| 5030 | 1 | 1 |
| 5050 | 8 | 8 |
| 5100 | unobserved | 15 |
| 5101 | unobserved | 18 |
| 5110 | 15 | 34 |
| 5120 | 75 | 94 |
| 0, control restored | 89 | 108 |

Times are relative to the first observed get-hit state. A state can enter and
exit within one positive-state chain; “unobserved” does not prove no entry.
The native trace observes equal Time=0/1 bounce positions because PosFreeze
survives entry, then subsequent movement resumes.

## Evidence, checks and limits

Twenty-one original text CNS/AIR/CMD/stage/JSON sets under
`tests/testdata/reference/downbounce-*` pin 324 numeric observations, two native
load rejections and 84 pre-native source hashes. Manifests are ignored
`temp/m4bm-prepared-inputs.json`, `m4bm-prepared-extra.json`,
`m4bm-prepared-last.json` and `m4bm-prepared-trace.json`. Additional zero-launch,
nonfall and phase-trace controls were frozen independently before native runs.
Do not rerun the author script or derive native expected values from Go.

`TestNativeDownBouncePrograms` verifies the exact fixture count, source hashes,
load errors and float32 observations through production Match. A Go source
overlay using predecessor 8779869a confirms both the default-vector and stock
no-bounce regressions before the consumer fix (temp/m4bm-before-fix.log).
The initial focused run exposed bottom handling and the stock bounce timing;
the refined replay passes all 21 programs and both stock sweep directions
(Match 16.775s). Separate physics/landing/recovery/SuperPause races pass
Character 1.450s and Match 1.871s. Core engine/evaluator/CNS/combat races pass
1.465s/1.025s/1.013s/1.013s. Four fresh native reproductions (including the legacy idle override) match.
Three final rebuilt apps/six captures pass provenance checks in
temp/m4bm-final-evidence-audit.json. The stock
bounce 7s capture was visually inspected: recovered fighters and intact HUD;
magenta stage diagnostic fill is intentional. Application checks are smoke,
not complete raster or interactive gameplay parity. See HANDOFF.md for the
combined integration result. The full run exposed one earlier idle-HitOverride
fall-query regression from M4bl; the final contact initialization fix preserves
both that native zero and M4bl's retained StateTypeSet flag. All other full-run
checks passed. The initial full log retains the failure; affected Match and
full Character replays plus rebuilt application checks verify the correction.
Final affected Match races pass 136.768s (196 native programs plus both stock
sweeps), full Character races pass 11.907s. The full suite was not repeated
after this isolated correction. Its initial failure and the final passing
checks are recorded separately; no full-run success is claimed for that log.

A fresh manual/source audit records **54/81 HitDef expression keys (66.7%)**;
27 remain literal or absent. Other README metrics are unchanged. This is not a
whole-engine or test coverage percentage. Full recovery-command routing,
liedown/mashing clocks, juggle/chains, broad helper/projectile/team/foreign/
contact/pause combinations, complete auto-turn/PosFreeze edge cases and absolute
KO/round clocks remain open. Six alpha and two spark raster failures remain
unwaived. This work adds no assets or binary files to Git.
