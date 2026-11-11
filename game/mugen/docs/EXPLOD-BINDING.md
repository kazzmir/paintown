# Explod binding-time mutation — M4cj

The production dispatcher now compiles and consumes ExplodBindTime. Twenty-eight
promoted `explodbind-*` programs contain the real controller in state 700: 24
accepted programs pin 288 numeric slots and 28 isolated effect positions; four
reject on load. A separate test asserts parsed controller kinds rather than
trusting labels.

## Measured behavior

ID defaults to -1. Any negative ID selects all active effects owned by the
executing character; nonnegative IDs select exact matches. Time defaults to one
tick. Zero cancels binding, positive values count down, and negative values bind
indefinitely in these controls. Binding reuses the original anchor and offset
after expiry; effects without anchors stay unbound. No objects are created.

ID evaluates before the chosen time expression. `time` takes precedence over
`value`: the ignored alias is neither evaluated nor type checked. Direct float
literals for ID/time/value reject; runtime floats truncate; redirected bottom
becomes zero. Unknown triggers reject with source context. Tagged assignment
controls distinguish these rules.

P1/P2 binding updates after player physics. The horizontal offset is oriented at
creation and retained when the anchor turns, as is the effect's facing. The old
implementation incorrectly reapplied the current anchor facing each tick. Counts,
lifetime and rendering continue to share the same owned effect instance.

The fixed camera is at stage X=80. Native 1280x720 images have game scale 3.
Stock KFM sprite 0,0 has a right-facing white support of 8,379 pixels or a
left-facing support of 7,935. JSONs record color/crop/support/bounds and calibrated
stage positions. Two-object cases use nonoverlapping supports. These are exact
color masks, not whole-frame equivalence claims.

## Preserved diagnostics and environment

Unpromoted `explodbind-runtime.*` fails native loading at an integer VarSet with a
direct float literal, before binding runs. It remains unchanged in place, with
an exact Git ignore pattern. Separately named `explodbind-supp-runtime-expr`
measures runtime truncation without that setup error. The newly found VarSet
schema gap is in the item 3 audit. No observed source or capture was repaired.

The upgraded host required task-local 32-bit Wine launcher shims and development
headers under ignored temp storage. A warm existing calibration reproduced all
twelve prior values. An initial cold startup capture and failed launcher/build
attempts are diagnostics. System packages and the reference distribution were not
changed. Final apps use fresh `m4cj3-*` names.

## Validation and limits

Helper/root ownership, explicit bindID, destroyed anchors, mixed localcoords,
dynamic cameras/zoom, hitpause and world pause budgets still need controls in
the following Explod checkpoints. Complete Explod mutation and the remaining
fields are in active item 3; item 4 remains unallocated. This checkpoint is
bounded controller evidence, not full Explod or engine compatibility.
