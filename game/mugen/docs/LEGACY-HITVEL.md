# Legacy HitVel X/Y

M4bx, predecessor/reversal `218e3106`. Target: the supplied Elecbyte
M.U.G.E.N 1.1 beta 1 executable, SHA256
`d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e`.
The primary documentation is `temp/mugen-1.1b1/docs/trigger.html#hitvel`;
native numeric controls resolve its ambiguous sign/unit description.

## Measured behavior

`HitVel X/Y` returns float, including zero before any contact. The values use
game-space units, independently of the observer's localcoord, an Enemy redirect
or a foreign positive-state owner. `GetHitVar(xvel/yvel)` uses the observer's
coordinate profile instead. X multiplies the stored world-oriented hit vector
by the receiver's **current** facing. Y retains the stored sign: the measured
upward values are negative. The trigger reads stored contact data rather than
current physical velocity. Turning, assigning another velocity and changing
MoveType to I preserve this data while changing X's facing interpretation.

| Control | HitVel X/Y | What it distinguishes |
| --- | --- | --- |
| Ground `(2,-1)`, 320-wide author, 1280-wide game | `(8,-4)` | Game units versus local units. |
| Same contact, 320-wide game | `(2,-1)` | Dependence on game width. |
| Ground `(-2,1)` | `(-8,4)` | Signed authored components. |
| Attacker turned left, receiver right | `(8,-4)` | Current receiver facing versus world-oriented GetHitVar X. |
| Receiver turned after contact; physical velocity overwritten | `(-8,-4)` | Facing changes X; stored data survives idle and new motion. |
| Air, down or air guard `(1.25,-0.5)` | `(5,-2)` | Selected reaction vector. |
| Standing guard `1.25` | `(5,0)` | Guard selection and vertical zero. |
| 1280-wide author | `(1.25,-0.5)` | Authored-to-game conversion. |
| Mixed receiver or foreign owner | Same canonical game values | Redirect/owner profiles do not rescale HitVel. |

Two fractional controls use authored `(1.234,-2.345)`, a 1024-wide game and
400/700-wide profiles in both orders. Native first divides the authored float32
by `(author_width/game_width)`, then multiplies by each observer's float32
`(observer_width/game_width)` for GetHitVar. Reconstructing HitVel from a
receiver-local vector or multiplying directly by the profiles' ratio loses
one float32 step. Canonical game-space hit storage and direct observation fix
that loss. HitVelSet transfers the canonical vector directly; redirected Vel
retains that precision until physics/CNS changes the applied axis. Existing
local HitDef metadata remains available to other consumers; KO offsets are
applied in canonical game units as well as to the local metadata.

## Frozen corpus and validation

Fifteen `tests/testdata/reference/legacyhitvel-*` programs contain 180 native
numeric observations and 60 frozen CNS/CMD/AIR/stage inputs. Goldens include
all input hashes, coordinate recipes, executable hash and capture PNG hash.
`match/legacy_hitvel_test.go` reuses the production Match executor and compares
float32 values exactly. It also replays the three unchanged original projectile
counterexamples in `known-divergences`, adding 180 original integer observations.
Their legacy Y is now -24/Var(22)=-2400, without editing their inputs or goldens.

Final validation results are recorded in HANDOFF.md and CHECKPOINTS.md.
Pre-native manifests are `temp/m4bx-{pre-native,extra-pre-native,
fraction-pre-native,valid-pre-native}.json`; final source/fixture manifest is
`temp/m4bx-final4-inputs.json`. Do not rerun the author scripts. Native PNGs,
prepared distributions, rejected inputs, logs and executables stay ignored.

The first preparation used an empty -1 state. Native's same-character cache
accepted nine programs but separate-opponent loads rejected two; Go correctly
rejected the empty state. Nine newly named `*-valid` programs add an inert
controller and have fresh native observations. The mixed-profile replacements
and a Turn replacement for the invalid FacingSet draft have distinct `*-v2`
names. Original inputs/observations remain unchanged in
`temp/m4bx-empty-negative-diagnostics` and `temp/m4bx-rejected-preparation`.
The first three Go runs failed and remain diagnostic: the first found empty -1
and precision issues; the second retained one redirected physical-velocity
rounding failure; the third showed that reconstructing the physical game vector
from local values cannot preserve its original precision. None is acceptance,
and no tolerance was introduced.

## Limits

These controls close the legacy HitVel binding gate, not all trigger runtime
bindings or full physical-coordinate conformance. Static recognition remains
127/127 and HitDef typed schemas 81/81. General physics, fractional KO profiles,
metadata-only contact sequences, helper/team/multiple-hit interactions and
full projectile/reversal lifecycle need their own controls. The preserved
projectile programs still exercise other incomplete queries such as fallcount;
matching their zero observations does not close those bindings. No overall
engine completion percentage follows from this checkpoint.
