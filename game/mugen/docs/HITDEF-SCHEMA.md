# HitDef parameter completion audit

The bundled 1.1 beta manual contains 81 HitDef parameter entries. A source audit
at M4av found20 compiled keys. M4aw extended that to29; M4ba added
Damage/GetPower/GivePower; M4bb adds kill/guard.kill/fall.kill/fall.damage;
M4be added pausetime/guard.pausetime; M4bg adds ground hit/slide times and four
dependent guard timing keys; M4bh adds air.hittime/down.hittime; M4bi adds
fall/air.fall/fall.recover/fall.recovertime; M4bk adds fall x/y velocity and
yaccel; M4bm adds down.bounce and M4bn adds guard.dist; M4bp adds chainID/nochainID/air.juggle, for 58
expression keys; M4bq adds mindist/maxdist/snap for 61, across
engine/hitdef.go, hit_vectors.go, hit_power.go, hit_pause.go, hit_timing.go and
hit_fall.go and hit_fall_vectors.go. This count measures source routing, not complete behavioral
coverage: enums, defaults, coercion, timing, ownership, units and interactions
still require the native fixtures described in the checkpoint documents.
M4bo adds nine literal mask/enum schemas in hit_literals.go, including
fall.animtype: 64/81 typed keys, of which 55 use numeric expressions.
See [literal schema evidence](HIT-LITERALS.md).

M4aw's [velocity/default controls](HIT-VELOCITY.md) cover the measured air,
guard, air-guard and down vectors, component inheritance, repeated default
expressions, profile conversion and down-hit Y offsets. Five corner-push fields
compile/store their expressions. M4ax [physical corner-push controls](CORNER-PUSH.md)
now cover the measured root/own-state movement, proximity, decay, units and
lifetime; projectile-owner and wider world combinations remain open. These controls do not establish arbitrary
blank/malformed tuple grammar or all paused/foreign/helper/contact combinations.

The final M4bt source audit accounts for all 81 keys: 72 numeric schemas and
nine literal schemas. [HITDEF-KEYS.json](HITDEF-KEYS.json) records the complete
inventory and pinned manual hash. Contact and stored-fall shake now have shared
clock/render consumers; see [HIT-SHAKE.md](HIT-SHAKE.md) and
[FALL-SHAKE.md](FALL-SHAKE.md). Final scoped races and native/application checks pass. Typed routing
and bounded native controls do not establish every interaction combination.

The M4ba [power controls](HIT-POWER.md) add typed damage/power, expression
defaults, all current typed-field order, root/helper pools, configured factors
and immediate contact transfer. Broad foreign/projectile/team/pause/multiple
contexts remain open. M4be's [contact pause controls](HIT-PAUSE.md) cover the two
pause keys. M4bg's [ground/guard timing controls](HIT-GROUND-TIMING.md) cover
six more keys and source-default chains. M4bh's [air/down controls](HIT-AIR-DOWN-TIMING.md)
cover both timers and the measured launch/down/countdown consumers. M4bi's
[fall/recovery controls](HIT-FALL-RECOVERY.md) cover the four new expressions,
fall selection and the eligibility clock. Remaining physical fields still
need expression/coercion/order coverage. Non-fall GetHitVar(recovertime),
liedown/mashing clocks and full recovery state paths remain open; M4bm adds
bounded downed-hit bounce consumers. Corner push has the bounded M4ax consumer
described above. Existing normal/guard/override/common-state fixtures are
bounded evidence; they do not
close these parameter groups. Complete GetHitVar selectors, juggle/recovery/KO,
projectile/reversal/team interactions and whole-engine/asset/mode gates remain
open in COMPATIBILITY.md and PLAN.md.

M4bk's [fall vectors](FALL-VECTORS.md) add the three expression schemas,
per-host profile defaults, omission/zero distinction, type/error/order rules,
mixed contact conversion and measured velocity consumers. Complete recovery
and bounce behavior remains open.

M4bj adds literal air.type storage and native trip contact selection/type
queries; see [TRIP-RECOVERY.md](TRIP-RECOVERY.md). Numeric expression routing
at M4bj remained 50/81. Enum/attribute validation still requires its own schema work.

M4bb's [lethality controls](HIT-LETHALITY.md) add typed kill flags/fall damage,
pre-entry damage caps, fatal guard normalization, one-shot raw fall damage and
measured KO velocity adjustments. These do not establish absolute KO/round
timing, full fall recovery, or the broad contact combinations listed above.

The refreshed audit is `temp/m4bn-hitdef-current-audit.json`; its input is the
local reference `temp/mugen-1.1b1/docs/sctrls.html` HitDef definition list.
It records all 81 documented keys, 55 expression keys and 26 keys that remain
literal or absent. Those 26 include compile-time enums/attributes as well as
numeric expressions; they need appropriate schemas rather than being treated
uniformly as numeric values. The earlier `temp/hitdef-schema-audit.txt` is a
historical M4aw snapshot, and m4bb-hitdef-current-audit.json records the prior36.
No original reference files are modified or committed.

M4bm's [downed-hit bounce](DOWN-BOUNCE.md) adds expression/coercion/order
coverage, contact fall-vector selection and stock bounce/get-up phase controls.
M4bl's [HitFallSet findings](HITFALLSET.md) close the bounded value/default
consumer gap and record the beta's ignored documented velocity parameters.

M4bn's [guard distance](GUARD-DISTANCE.md) closes the bounded integer/range
schema and consumers. [HITDEF-REMAINING.md](HITDEF-REMAINING.md) tracks the
user-authorized 27-key allocation, including the nine literal schemas.

M4bo closes the nine literal schemas with 65 native programs, 684 observations,
eight load rejections and 260 frozen input hashes. The remaining 17 keys are
numeric fields in the combat, constraint, palette and shake groups above.

M4bp closes the three integer chain/juggle keys and bounded consumers with57
native programs,660 observations/eight load rejections/228 frozen input hashes.
Numeric routing58/81 plus nine literals gives67/81 typed schemas; fourteen keys
remain. See [chain/juggle evidence and limits](HIT-CHAINS-JUGGLE.md).

M4bq adds three constraint schemas and position/offset/timed-binding consumers:
39 programs,588 observations/seven load rejections/156 frozen source hashes.
Numeric routing61/81 plus nine literal schemas gives70/81 typed keys; eleven
palette/shake keys remain. See [distance/snap evidence](HIT-CONSTRAINTS.md).

M4br adds three palette schemas and actual color/clock/cache consumers:
24 programs/324 observations/three rejections/96 frozen source hashes.19 static
production programs match228 native color samples and preserve HUD portraits.
Numeric routing64/81 plus nine literals gives73/81 typed keys; eight shake keys
remain. See [hit palette evidence and limits](HIT-PALETTE.md).

M4br adds three palette keys; M4bs adds four contact-shake keys. Numeric routing
is68/81 plus nine literal schemas =77/81 typed keys (95.1%). Four stored-fall
keys remain. Palette evidence: [HIT-PALETTE.md](HIT-PALETTE.md).

M4bt adds the final four stored-fall fields, bringing typed key coverage to
81/81 (72 numeric plus nine literal schemas). The authorized 27-key allocation
is complete, including final scoped races and native/application gates. Earlier counts above are dated
checkpoint history, not the present remaining-key count.
