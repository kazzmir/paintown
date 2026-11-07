# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Scope only game/mugen/,
local source/text commits, no pushes/binaries/assets/generated banks. Runtime
runs stay serial through the private wrapper; await exit, guard, then edit/run.
Reuse canonical assets/cache and remove scratch on success/failure.

Latest team batch fixes mixed Single/Simul/Turns round allowances and health.
Single/Simul keep match.wins rounds against Turns; draws consume a round/member.
Starting life compares concurrent fighters per required opponent defeat with
Team.1VS2Life weighting Simul, reduces only the stronger team, then applies
Life% and native nearest-integer rounding. Original roster sizes remain fixed.
Defeated teams replenish; winners carry recovered health, including a defeated
partner whose positive result life revives only on the next round/bout.

Two bounded original1.1b1 flows: Simul-vs4Turns has1000/750 max life,400→797
carry and ends on Simul's second defeat in round4. Survival Simul has667 max:
400→664 and defeated partner0→264 (Alive0 during result, Alive1 next bout),
then667/528 after another win, and one whole-team defeat ends the streak.
The underlying native initializer also separates Turns team-size health and
recovery adjustments from Simul's ordinary1-vs-2 balance. Survival Co-op now
schedules a Single opponent. Connected team/mode scenarios cover these paths;
changed-source segments pass with race detection. Linux binary rebuilt.
All native overlays/captures/logs and Go scratch were removed; no fixture bank.

Newly explicit functional gaps (reopened modes/teams rows):
- Survival still creates a fresh match per scheduled opponent, resetting
  RoundNo/RoundsExisted and declared persistent CNS variables. Carry these
  BEFORE state5900 entry and initial-opponent health snapshots; avoid merely
  restoring after NewFightState. Native control showed rounds1/2/3 and survivor
  RoundsExisted0/1/2. Preserve the existing original Turns roster/start index.
- Implement documented held-direction Turns rotation at initial loading and
  reserve entry (forward/back rotate remaining members; up rotates4 by2).
- Team Co-op must let P1 choose opposing team mode after both humans choose.

Current recovery carry lives in modeRun.carryLife/carryPower and restore(),
called after match.New from NewFightState. This is the natural next refactor.
Native helpers were removed after use; do not regenerate old matrices.
Remaining precision/assets stay partial (render/audio/AI, position-link/camera/
push ordering, strict validation, physical CD and absent licensed fonts).
User's stage/tracker content and unrelated root work remain untouched.
