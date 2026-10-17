# HitDef parameter completion audit

The bundled 1.1 beta manual contains 81 HitDef parameter entries. A source audit
at M4av found20 compiled keys. M4aw extended that to29; M4ba added
Damage/GetPower/GivePower; M4bb adds kill/guard.kill/fall.kill/fall.damage;
M4be adds pausetime/guard.pausetime for38 expression keys across engine/hitdef.go,
hit_vectors.go, hit_power.go and hit_pause.go. This count measures source routing, not complete behavioral
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

- Reaction: fall.animtype, air.type, fall.recovertime, down.bounce.
- Combat: air.juggle, chainID, nochainID.
- Constraints: mindist, maxdist, snap.
- Palette: palfx.time, palfx.mul, palfx.add.
- Shake: envshake.time/freq/ampl/phase and fall.envshake.time/freq/ampl/phase.

The M4ba [power controls](HIT-POWER.md) add typed damage/power, expression
defaults, all current typed-field order, root/helper pools, configured factors
and immediate contact transfer. Broad foreign/projectile/team/pause/multiple
contexts remain open. M4be's [contact pause controls](HIT-PAUSE.md) cover the two
pause keys; literal-only hit/slide/control timing, fall and remaining physical
fields still need expression/coercion/order coverage. Down
time still lacks complete runtime consumers; corner push has the bounded M4ax
consumer described above. Existing
normal/guard/override/common-state fixtures are bounded evidence; they do not
close these parameter groups. Complete GetHitVar selectors, juggle/recovery/KO,
projectile/reversal/team interactions and whole-engine/asset/mode gates remain
open in COMPATIBILITY.md and PLAN.md.

M4bb's [lethality controls](HIT-LETHALITY.md) add typed kill flags/fall damage,
pre-entry damage caps, fatal guard normalization, one-shot raw fall damage and
measured KO velocity adjustments. These do not establish absolute KO/round
timing, full fall recovery, or the broad contact combinations listed above.

The refreshed audit is `temp/m4be-hitdef-current-audit.json`; its input is the
local reference `temp/mugen-1.1b1/docs/sctrls.html` HitDef definition list.
It records all81 documented keys,38 expression keys and43 keys that remain
literal or absent. Those43 include compile-time enums/attributes as well as
numeric expressions; they need appropriate schemas rather than being treated
uniformly as numeric values. The earlier `temp/hitdef-schema-audit.txt` is a
historical M4aw snapshot, and m4bb-hitdef-current-audit.json records the prior36.
No original reference files are modified or committed.
