# HitDef literal schemas and reaction metadata (M4bo)

Target: the immutable local M.U.G.E.N 1.1 beta 1 distribution. This checkpoint
adds schemas for nine literal keys: attr, hitflag, guardflag, affectteam,
animtype, air.animtype, fall.animtype, ground.type and air.type. These are
literal masks/enums, not numeric expressions. Numeric routing stays 55/81;
combined typed coverage becomes 55 numeric + nine literal = 64/81 keys.
Seventeen numeric keys remain. This is not whole-engine completion.

## Measured rules

Animation enums accept their first letter, case insensitive: Light, Medium,
Hard, Back, Up and DiagUp. Attack types similarly accept High, Low, Trip and
None; team selection accepts E/F/B. Suffix text is ignored. An unknown or
empty enum rejects loading before any state controller runs. Air animation
inherits ground animation. Fall animation defaults to Back, except Up and
DiagUp air animations retain that type. Air attack type inherits ground type.

Hit flags default MAF; guard flags default empty. Unknown characters are
ignored, including noise between recognized letters. Empty hit flags and
simultaneous +/- cannot connect. Existing posture, falling, guard, priority
and team controls remain separate evidence for those consumers.

Missing attributes, a quoted attribute string or an unusable first attack
class produces a non-hitting definition. Attack classes must be exact two
letter NSH × ATP tokens. Parsing stops at the first invalid class and retains
earlier valid classes: S,NA,XX still hits; S,XX,NA and S,NAjunk do not. An
invalid posture with a comma (X,NA) rejects loading. Protection filters retain
their strict grammar. HitDefAttr now consumes the active definition's classes
and current attacker posture, including multiple classes.

GetHitVar(animtype) stores the selected type at contact. Falling contacts use
fall.animtype, otherwise airborne contacts use air.animtype. Ground Back
without fall reports Hard. Later HitFallSet does not recompute the selected
type. A new contact selects again. Guard contacts report groundtype/airtype
zero; an untouched receiver also reports zero.

Unguarded None updates hit metadata and consumes that swing's receiver slot,
but causes no damage, power, hit/contact counts, target acquisition, state
transition or hit pause. Explicit pause 5,7 and pause 0,0 give identical
45-tick snapshots. Changing the receiver to airborne under the same definition
does not enable another contact. Guarded None follows ordinary guard behavior.
This differs from the manual's description of hit pause for None; the target
executable is the behavioral reference.

Entry to trip state 5070 installs the character's fall.defence_up adjustment,
even for an authored state, non-falling Trip or an explicit custom destination.
The adjustment persists into a subsequent ordinary contact. Three additional
native controls isolate this from the Trip literal and fall flag. This bounded
entry rule does not establish all fall-defence/state-number behavior.

## Evidence and limits

Sixty-five hitliteral-* programs freeze 260 CNS/AIR/CMD/stage source hashes
before native execution. The manifests are retained under ignored temp/ as
m4bo-prepared-inputs.json, m4bo-prepared-extra.json, m4bo-prepared-last.json,
m4bo-prepared-clocks.json, m4bo-prepared-none-air.json and
m4bo-prepared-trip.json. Recorded JSON observations are replayed through the
production Match/Character runtime. Native expectations and tolerances are
unchanged. Source, captures and original-executable provenance are checked.

Native records contain684 numeric observations and eight load rejections.
Focused Match races pass55.619s; broader selection/flag/forced-standing/stock
sweeps pass97.325s, full Character11.772s and pure core races pass. Three fresh
native replays and two apps/three captures pass provenance; the stock capture
was visually inspected. Details and hashes are recorded in HANDOFF.md. Earlier failed
focused logs remain diagnostic evidence of corrected defaults, untouched/guard
metadata and trip defence. Full scoped races follow M4bp's combat integration.

Broad team/entity/foreign/projectile/reversal/pause combinations, all recovery,
complete HitDefAttr grammar and other fall-defence/state-number rules remain
open. Existing alpha and spark raster failures are unwaived.
