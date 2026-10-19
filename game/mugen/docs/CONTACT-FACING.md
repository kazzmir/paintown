# Contact facing and entry phases

M4av uses 48 original `hitfacing-*` programs: 552 native numeric observations,
two load rejections and 192 authored CNS/AIR/CMD/stage hashes. The packet compares
both headings/states, receiver life, world get-hit X, initial/late local velocity,
position, activation/entry/phase heading and attacker contact/target counters.
Independent side-effect controls distinguish evaluation from application timing.

## Measured behavior

Negative p1facing flips the attacker; zero/positive values leave it unchanged.
Nonzero p1getp2facing takes precedence: positive copies the receiver's contact
heading, negative chooses the opposite. Values +/-2 use the same sign rule.
Both parameter expressions still evaluate, p1facing before p1getp2facing, even
when their declaration order is reversed. Literal floats reject in state 700;
computed +/-0.5 truncate to zero and bottom supplies zero.

A miss, guarded contact or HitOverride does not apply the attacker-facing change.
An ordinary hit chooses its direction from the heading at contact, after these
fields, rather than the heading at HitDef activation. Turns immediately before
and after activation independently pin that distinction. Receiver GetHitVar X
and HitVelSet use that selected direction. Combined P2Facing controls also use
the selected heading and retain the earlier deferred receiver-turn behavior.

The attacker's physical turn publishes after its own CNS/physics. Its contact
negative/positive passes and custom destination Time=0 body still see the old
heading. The later turn preserves world momentum. A custom state's FaceP2 can
turn independently during entry; the pending contact heading still wins at the
end of the update. A behind-opponent control distinguishes a fixed requested
heading from applying another relative flip after state entry.

Two shared entry corrections are also necessary. FaceP2 evaluates and turns
before the remaining Statedef expressions: poweradd sees the resulting heading.
Counter side effects in both written orders establish FaceP2-before-poweradd.
Ordinary ChangeState and HitDef destination controls agree. Standard get-hit
selection immediately publishes its number and H/no-control, but ordinary
receiver Statedef evaluation waits for its positive pass. A receiver entry
expression can therefore see the earlier attacker's completed contact turn.
Custom victim entry continues to supersede the pending standard entry.

## Implementation and evidence

Typed HitDef cues retain integer coercion/diagnostics and immutable source maps.
Contact snapshots the chosen heading for get-hit velocity and queues the actual
turn through the existing pending-facing publication. Guards/overrides skip the
P1 fields. The single Statedef entry path handles the earlier FaceP2 phase; the
ordinary standard-hit path uses the existing deferred external-entry adapter.

Native logs are `temp/m4av-native-first.log`, `m4av-extension-native.log`,
`m4av-phase-native.log`, `m4av-entry-native.log` and
`m4av-entry-order-native.log`. The initial 20-program comparison fails eight
cases (`m4av-before-facing.log`). The first 34-case implementation fails override/entry
controls (`m4av-first-fields.log`); the 40-case deferred version
retains two entry-expression failures (`m4av-deferred-facing.log`). These
counterexamples are preserved separately from corrected verification.

Full cross-field evaluation order, arbitrary simultaneous/foreign/paused/helper/
projectile contacts, standard/guard/override entry phases beyond these controls,
full velocity/HitDef schema, and engine/asset/mode compatibility remain open.
Reversal: dc90bb1e (M4au). No reference art or executable is committed.

The 48 corrected integrated programs pass in `temp/m4av-entry-facing.log`.
Pure CNS/engine/evaluator races pass in `temp/m4av-entry-pure-race.log`.
The full scoped race suite passes in `temp/m4av-full-race.log` (Match 349.165
seconds), including earlier Character/combat/throw/entry coverage. Six fresh
native replays pass in `temp/m4av-replays.log`: behind entry, receiver entry,
FaceP2 side-effect order, overridden copy, combined P2 turn and literal-float
rejection. Ten actual 1280x720 captures from one identical binary pass source/
definition/binary/config/PNG/dimension/runtime checks. All six stable white masks
match exactly: four at 16,563 pixels, guard 14,991 and override 11,181. Stock
throw recovery has two fighters without persistent dust. Results are
`temp/m4av-app-checks.json` and `temp/m4av-app-checks.log`. Background/debug/HUD
and full-frame renderer parity are not established by these masks.
