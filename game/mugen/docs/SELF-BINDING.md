# Parent/root actor constraints — M4cl

BindToParent and BindToRoot now have typed compilers and production constraints.
The native corpus has84 programs:78 accepted/936 numeric slots and six load
rejections, with336 pinned CNS/CMD/AIR/stage originals. Fourteen programs are
dependent friction traces. Intent metadata/tests identify the actual controller,
state (700/710/720) and count, including two-command cancellation cases.

## Measured controller rules

Only helpers bind; root callers skip parameter evaluation. Parameters still
undergo load validation. Time defaults to1, facing to0 and position to0,0. One
position component supplies Y=0. Direct float time/facing literals and unknown
expressions reject; runtime floats truncate and redirected bottom becomes zero.
The evaluation order is time, facing, X, Y. Time0 cancels; -1 persists; values
below -1 behave as one tick. Facing uses the sign of nonzero values.

Binding uses the current parent/root facing for the offset. Facing0 leaves the
helper's facing alone; positive values synchronize it and negative values face
opposite. A turn preserves absolute momentum by negating the local X velocity.
The binding retains the actor's own velocity rather than inheriting the anchor's.

CNS reads the previous completed pose. Issuing a bind does not immediately
change Pos; an ensuing PosSet remains visible to later controllers in that pass.
After physics forces, the constraint replaces that position. Movement is
suppressed while bound, but gravity and friction continue. The issuing pass
counts toward the duration. Cancellation immediately permits movement again.
GetHitVar(isbound) remains zero for these self constraints.

Bindings survive ChangeState. Destroying a parent ends parent binding and
resumes the child's motion; a root-bound grandchild continues after its parent
retires. Pause and SuperPause freeze the tested duration/clock sequence. Root
binding and parent binding share the exclusive constraint slot with combat
TargetBind, while retaining separate phase/velocity/flag policies. The
implementation restricts TargetDrop cleanup to combat constraints; overlapping
controller interactions still need their dedicated native controls.

## Dependent friction correction

The binding physics controls exposed old friction differences. Native standing
physics multiplies velocity, then sets magnitudes strictly below1 to zero;
equality1 persists. Crouch physics has no corresponding cutoff, including a
0.01 velocity and traces below0.05. The documented threshold fields are ignored
in the measured beta: omission and values1/2/100 do not change these results.
This differs from interpreting the comments in the stock CNS as implemented.

Matching the complete traces requires importing the coefficient at float32
precision while retaining the stored velocity precision during multiplication:
`velocity *= float64(float32(coefficient))`. Rounding the velocity to float32
after every multiplication is a different recurrence and fails the originals.
This describes the reproducing model, not a claim about the closed binary's
internal types. No numerical tolerance or native golden was changed.

## Verification and limits

Initial failed implementations/logs are retained. Both actual-window masks match
all7,935 native white pixels. Sessions were awaited and a separate client check is
empty. See HANDOFF.md and ignored final evidence audit. This sequencing was not
repeated.

BindToTarget's combat selection and target head/mid points follow in a separate
checkpoint. Incoming-hit/overlapping-constraint interactions, mixed/foreign
profiles, disabled roots and wider pause/hitpause variants remain bounded gates;
do not infer exhaustive family or engine conformance. The remaining Explod,
palette, alpha/shadow/draw and other item3 consumers remain active. Item4 is
still unallocated.

The standard staged whitespace check reports four authored originals with a
trailing blank line (`parent/root-extra-empty` and `parent/root-state-change`).
They are preserved byte-for-byte because native evidence pins those inputs.
The staged check with only blank-at-EOF excluded passes; all staged files are
UTF-8 text/source under game/mugen/, with no binaries.
