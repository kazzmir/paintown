# Current handoff

Use [PROGRESS.md](PROGRESS.md) as the single ledger. Work only in game/mugen/;
local source/text commits, no pushes/binaries/assets/capture banks. Follow
AGENTS.md's serial private-host wrapper and connected-scenario workflow.

Latest batch replaces provisional recovery with the supplied beta's calculation:
125-per-thousand base plus275-per-thousand time bonus over100 elapsed counts,
adjusted by the opponent's initial health (60% weight), truncated to a per-thousand
integer before scaling by maximum life. TimerFreeze and an unlimited displayed
timer do not stop elapsed recovery time. Healing precedes win-pose CNS, occurs
once, and survives round reset. Completed ordinary Turns rosters do not heal.
Single-player Survival carries the already recovered result without healing twice.

Original1.1b1 connected controls: Turns2-vs-4 has1000/500 maximum life;400→798 at
full displayed40,400→744 after20 elapsed counts,100/500→275 against the previously
damaged entrant, and900→1000 cap. Single-player Survival also gives400→798.
The corresponding connected team/mode scenarios pass with race detection; all
changed-source segments pass and the Linux executable is rebuilt. The final
coverage report initially caught a stale RecoveryLife symbol; catalog now names
the replacement consumer. No game-test rerun was required for that metadata fix.
Native overlays/captures/logs and segment scratch were deleted after each run.

Survival follow-up now advances one opponent per bout using the real default
match.wins setting; the scenario no longer forces it to1. Survival exposes a
P1 team menu. Turns reserves remain human-controlled. Campaign handoff retains
the original roster and selected reserve index, carries its recovered health/
power, and never re-enters eliminated members. The existing connected mode
scenario spans those transitions and final roster exhaustion. Affected segments
pass with race detection; Linux binary rebuilt. No generated assets are retained.

Next: implement documented1.1 stage startzoom/zoomout/zoomin and BG zoomdelta.
They are presently absent from stage parsing and rendering (Viewport.Zoom only
serves coordinate/boundary queries). This is a functional gap discovered during
this audit, beyond the remaining native color/shadow/alpha/KO-echo/AI precision.
Mixed-mode life balance, Survival team recovery and dead Simul partner carry
still need native confirmation. Physical CD and absent licensed fonts remain
limitations. Canonical local assets/music stay ignored and unchanged.
