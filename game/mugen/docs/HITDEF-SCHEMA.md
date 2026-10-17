# HitDef parameter completion audit

The bundled 1.1 beta manual contains 81 HitDef parameter entries. A source audit
at M4av found 20 compiled keys. M4aw extends that to 29 keys across
`engine/hitdef.go` and `engine/hit_vectors.go`. This count measures source routing, not complete behavioral
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
- Combat: air.juggle, chainID, nochainID, kill, guard.kill, fall.kill.
- Constraints: mindist, maxdist, snap.
- Palette: palfx.time, palfx.mul, palfx.add.
- Shake: envshake.time/freq/ampl/phase and fall.envshake.time/freq/ampl/phase.

Literal-only damage, pause, timing, power, fall and remaining physical fields
need dynamic expression/coercion/order coverage. Power and down
time still lack complete runtime consumers; corner push has the bounded M4ax
consumer described above. Existing
normal/guard/override/common-state fixtures are bounded evidence; they do not
close these parameter groups. Complete GetHitVar selectors, juggle/recovery/KO,
projectile/reversal/team interactions and whole-engine/asset/mode gates remain
open in COMPATIBILITY.md and PLAN.md.

The disposable audit is `temp/hitdef-schema-audit.txt`; its input is the local
reference `temp/mugen-1.1b1/docs/sctrls.html` HitDef definition list. No original
reference distribution files are modified or committed.
