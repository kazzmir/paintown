# HitDef parameter completion audit

The bundled 1.1 beta manual contains 81 HitDef parameter entries. A source audit
at M4av found20 compiled keys. M4aw extended that to29; M4ba added
Damage/GetPower/GivePower; M4bb adds kill/guard.kill/fall.kill/fall.damage;
M4be added pausetime/guard.pausetime; M4bg adds ground hit/slide times and four
dependent guard timing keys; M4bh adds air.hittime/down.hittime; M4bi adds
fall/air.fall/fall.recover/fall.recovertime; M4bk adds fall x/y velocity and
yaccel, for 53 expression keys across
engine/hitdef.go, hit_vectors.go, hit_power.go, hit_pause.go, hit_timing.go and
hit_fall.go and hit_fall_vectors.go. This count measures source routing, not complete behavioral
coverage: enums, defaults, coercion, timing, ownership, units and interactions
still require the native fixtures described in the checkpoint documents.
The remaining literal-parser fields must also receive typed parameter schemas.

M4aw's [velocity/default controls](HIT-VELOCITY.md) cover the measured air,
guard, air-guard and down vectors, component inheritance, repeated default
expressions, profile conversion and down-hit Y offsets. Five corner-push fields
compile/store their expressions. M4ax [physical corner-push controls](CORNER-PUSH.md)
now cover the measured root/own-state movement, proximity, decay, units and
lifetime; projectile-owner and wider world combinations remain open. These controls do not establish arbitrary
blank/malformed tuple grammar or all paused/foreign/helper/contact combinations.

These documented keys have no direct parser/compiler field and need explicit
implementation or measured target limitations. This is a source audit, not a
claim that the other keys are complete:

- Reaction: fall.animtype, down.bounce.
- Combat: air.juggle, chainID, nochainID.
- Constraints: mindist, maxdist, snap.
- Palette: palfx.time, palfx.mul, palfx.add.
- Shake: envshake.time/freq/ampl/phase and fall.envshake.time/freq/ampl/phase.

The M4ba [power controls](HIT-POWER.md) add typed damage/power, expression
defaults, all current typed-field order, root/helper pools, configured factors
and immediate contact transfer. Broad foreign/projectile/team/pause/multiple
contexts remain open. M4be's [contact pause controls](HIT-PAUSE.md) cover the two
pause keys. M4bg's [ground/guard timing controls](HIT-GROUND-TIMING.md) cover
six more keys and source-default chains. M4bh's [air/down controls](HIT-AIR-DOWN-TIMING.md)
cover both timers and the measured launch/down/countdown consumers. M4bi's
[fall/recovery controls](HIT-FALL-RECOVERY.md) cover the four new expressions,
fall selection and the eligibility clock. Remaining physical fields still
need expression/coercion/order coverage. Down
time still lacks complete runtime consumers; non-fall GetHitVar(recovertime),
bounce and the full recovery state paths remain open. Corner push has the bounded M4ax
consumer described above. Existing
normal/guard/override/common-state fixtures are bounded evidence; they do not
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

The refreshed audit is `temp/m4bk-hitdef-current-audit.json`; its input is the
local reference `temp/mugen-1.1b1/docs/sctrls.html` HitDef definition list.
It records all 81 documented keys, 53 expression keys and 28 keys that remain
literal or absent. Those 28 include compile-time enums/attributes as well as
numeric expressions; they need appropriate schemas rather than being treated
uniformly as numeric values. The earlier `temp/hitdef-schema-audit.txt` is a
historical M4aw snapshot, and m4bb-hitdef-current-audit.json records the prior36.
No original reference files are modified or committed.
