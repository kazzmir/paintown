# Process clock, tick rate and Versus match number

M4by, predecessor/reversal `e1299891`. The supplied beta 1 executable is the
primary reference; `docs/trigger.html#gametime`, `#tickspersecond`, `#matchno`
and the two GameSpeed settings in `data/mugen.cfg` describe the public surface.
Native observations resolve the rate formula and menu/reset ownership.

## Process ownership

The production Engine owns a signed 32-bit GameTime counter, starting at zero.
Each successful Engine.Update advances it exactly once, including storyboards,
menus, selection, versus, active combat, KO skips, finished-match waits and
returns to the title. Before a fight update, its timeline receives the current
process tick. After the update/transition, the current or newly constructed
fight timeline receives the next process tick. A rematch gets a fresh timeline
and world with the retained scalar counter; pause/shake/effect state is not
shared with the old match. First-round simulation remains immediate; the
existing blank initialization pass between later rounds is unchanged.

Characters, helpers and redirects continue using their match's shared Timeline.
Pause/super-pause, nested actors and KO simulation cadence retain their existing
owners. Standalone Match instances still start at GameTime zero and advance
their own timelines. Production Engine handoff supplies the process origin;
it does not turn every independent Match into a global singleton.

Native `engine-gametime` observations keep one process/window alive through
Versus, two Escape returns to the title, another selection and another match:

| Observation | First CNS in 5900 | Current CNS GameTime | State 0 Time |
| --- | ---: | ---: | ---: |
| First entry | 751 | 928 | 177 |
| Second entry | 1655 | 1834 | 179 |

Both snapshots show the enemy's identical process GameTime, RoundNo=1,
RoundState=2, MatchNo=1 and TicksPerSecond=60. Subtracting the entry timestamp
from current GameTime equals Time exactly. Absolute menu timestamps vary with
wall-clock capture startup; fresh reproduction verifies input hashes and the
same-process relationships rather than imposing a wall-clock timestamp golden.
The PNGs and menu-return view were inspected. Native exact menu/fade timing
is a separate presentation obligation.

`system/gametime_test.go` replays all 24 recorded integers by advancing real
Engine title updates to the recorded entry ticks, then using NewFightState and
the unchanged native CNS program. It never sets a Match timeline origin.
The production keyboard flow additionally samples a real CNS VarSet on every
executed pass across two full-life combat rounds, a 23-frame results wait,
rematch, 150 further updates and exit/menu waiting. Stock 5900 resets user
variables after its negative pass, so that overwritten witness is checked
from the following pass. Match timeline handoff is checked on every update,
including transitions and skips. A separate test crosses signed 32-bit wrap
using Engine updates. Earlier origin-aligned scalar Match fixtures remain
bounded round-trace tests; this checkpoint adds the production clock handoff.

## Tick rate and MatchNo

The new native clock program exposed two additional recognized but unbound
triggers. They are included here because they belong to the same process/match
clock context. Both return integers, demonstrated by integer division controls.

| Config.GameSpeed | Options.GameSpeed | Native TicksPerSecond | TicksPerSecond / 7 |
| ---: | ---: | ---: | ---: |
| 30 | 0 | 30 | 4 |
| 75 | 0 | 75 | 10 |
| 60 | 4 | 80 | 11 |
| 30 | 4 | 50 | 7 |
| 75 | -3 | 60 | 8 |

These controls support the formula `Config.GameSpeed + 5 * Options.GameSpeed`.
`Engine.ticksPerSecond` now supplies both Ebiten.SetTPS and the Match timeline's
TicksPerSecond context, fixing the previously ignored Options.GameSpeed.
Standalone contexts retain the default 60. The production rate helper uses
60 for an absent/nonpositive base and clamps its result to at least one;
malformed/extreme configuration parity is not established by these controls.

MatchNo returns one for the current one-on-one Versus lifecycle, including new
matches/rematches. Native also reports one in the Quick-VS rate controls.
Arcade/team progression, continue and restart rules remain mode gates; this
binding does not implement those modes.

## Evidence and limits

Six authored programs provide 84 native observations: two 12-integer process
snapshots and five 12-value rate controls. The 29 authored CNS/CMD/AIR/recipe/
stage inputs are frozen before native observation. `engine-gametime.json`
also pins the capture script, generated preparation and select list. Rate
goldens pin their exact config, executable, recipe and PNG. Native binary SHA:
`d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e`.
These preparations preserve the config's UTF-8 BOM. The provenance audit
retains it and reverses only native FirstRun startup persistence plus line
ending normalization; no numeric observations or prepared input bytes change.

The initial flow witness failed because stock 5900 clears user variables after
the negative pass. A later exact native-program replay exposed TicksPerSecond
and MatchNo as unbound and corrected its assumed initial blank tick; both runs
remain diagnostic. Neither changes native inputs or goldens. Final validation,
source manifests, reproduction and application provenance are in HANDOFF.md.

M4bx closes legacy HitVel; this checkpoint closes production process GameTime
handoff and the bounded rate/Versus-number bindings. It does not close every
remaining runtime trigger, all physical-coordinate cases, presentation clocks,
configuration lifecycle, broader modes or whole-engine compatibility. Static
trigger recognition remains 127/127 and typed HitDef schemas 81/81. The original
renderer/font/resource/combat/mode/BGM gates remain unwaived.
