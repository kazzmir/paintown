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

Continue with Survival bout/roster persistence: the existing mode scenario forces
match.wins=1 and may hide Survival's ordinary two-round default; carry currently
stores only active members. Mixed Single/Simul/Turns life balancing and Survival
team recovery still need native confirmation. Remaining renderer/KO-echo/AI
precision and licensed-font/CD limitations remain partial, not certified parity.
The earlier ACT outfit/private-portrait and stage/tracker-music work is retained.
Canonical local assets remain under temp/mugen-1.1b1; music links to the user's
existing data-new/mugen/music. No originals outside scope were changed.
