# HitDef parameter completion audit

The bundled 1.1 beta manual contains 81 HitDef parameter entries. A source audit
at M4av finds 20 keys routed through compiled expression cues in
`engine/hitdef.go`. This count measures source routing, not complete behavioral
coverage: enums, defaults, coercion, timing, ownership, units and interactions
still require the native fixtures described in the checkpoint documents.
The remaining literal-parser fields must also receive typed parameter schemas.

The next bounded group is air.velocity, guard.velocity, airguard.velocity and
down.velocity, including omitted/inherited values, components, expressions,
bottom, receiver posture, actual HitVelSet motion and mixed coordinate profiles.
Ground.velocity is already compiled and covered by M4au; that does not establish
the other vectors. The current down-vector storage alone does not prove it is
selected by GetHitVar or the state machine.

These documented keys have no direct parser/compiler field and need explicit
implementation or measured target limitations. This is a source audit, not a
claim that the other keys are complete:

- Reaction: fall.animtype, air.type, fall.recovertime, down.bounce.
- Combat: air.juggle, chainID, nochainID, kill, guard.kill, fall.kill.
- Constraints: mindist, maxdist, snap.
- Push: down.cornerpush.veloff, guard.cornerpush.veloff, airguard.cornerpush.veloff.
- Palette: palfx.time, palfx.mul, palfx.add.
- Shake: envshake.time/freq/ampl/phase and fall.envshake.time/freq/ampl/phase.

Literal-only damage, pause, timing, power, fall and remaining physical fields
need dynamic expression/coercion/order coverage. Stored values such as power,
corner push and down time require a separate runtime-consumer audit. Existing
normal/guard/override/common-state fixtures are bounded evidence; they do not
close these parameter groups. Complete GetHitVar selectors, juggle/recovery/KO,
projectile/reversal/team interactions and whole-engine/asset/mode gates remain
open in COMPATIBILITY.md and PLAN.md.

The disposable audit is `temp/hitdef-schema-audit.txt`; its input is the local
reference `temp/mugen-1.1b1/docs/sctrls.html` HitDef definition list. No original
reference distribution files are modified or committed.
