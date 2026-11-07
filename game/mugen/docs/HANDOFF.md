# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Scope only game/mugen/;
local source/text commits, no pushes/binaries/assets/generated banks. Runtime
runs are serial through the private wrapper; await exit, guard, then edit/run.
Reuse canonical assets/shared cache; remove all per-run scratch after inspection.

Latest batch restores Survival as a continuing match: MatchNo stays1, RoundNo
continues across opponent loads and survivor RoundsExisted advances. RoundCarry
snapshots life/power and variables; match initialization applies only declared
persistent variables BEFORE state5900 attributes and initial-health snapshots.
Enemies start fresh. Zero-life carry stays zero; positive recovered partner life
becomes alive on the next round/bout. Carry applies only to the first loaded
round, so a later reserve entry cannot replay an obsolete snapshot.

The shared CNS RNG is copied across opponent loads without reseeding/advancing
the session match-seed stream. GameTime/TicksPerSecond are available during5900
entry. Successful Survival bouts expose MatchOver0 to CNS while internally
routing to the next opponent; full home-team defeat exposes MatchOver1.

The connected mode scenario uses a tiny CNS/DEF text overlay with existing
KFM assets in place. Its5900 anim attribute requires restored persistent vars,
RoundNo/RoundsExisted and GameTime; lower vars clear, declared higher vars/RNG
survive. Mixed-coordinate Turns reserve and Simul667/664/264→667/528 flows still
pass, as do changed-source segments with race detection. Linux rebuilt; root
module hashes unchanged; no generated assets/captures/fixture bank retained.

Previous local team checkpoint fixed mixed Single/Simul round budgets against
Turns and normalized maximum life, validated against original1.1b1 flows.

Next (remaining teams partial): implement documented held-direction Turns
rotation at initial match loading and after losing a round: forward/back rotate
only remaining members; up rotates four by2. Sample input once before loading,
keep palette/selection/player pointers aligned and never rotate KO members or
continuing Survival winners. If a Survival roster is reordered, synchronize the
campaign template before capturing the active member's state.
Team Co-op also needs P1's opponent-mode menu after both human selections.
Survival Co-op already schedules a Single opponent; cover its input ownership
alongside that menu. No native parameter matrices are needed: the beta readme
has the roster-order chart and co-op selection flow.

Other partial work remains rendering/audio/AI precision, stage/link/window and
camera/push interactions, strict validation, physical CD and absent licensed
fonts. User's stage/tracker content and unrelated root work stay untouched.
