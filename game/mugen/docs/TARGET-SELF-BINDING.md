# BindToTarget — M4cm

BindToTarget now has a typed compiler and an actual combat consumer. Its name
hides a significant difference from BindToParent/Root: it moves the caller
immediately, then constrains the selected target to that moving caller. The
caller keeps its own movement. A later PosSet in the same CNS pass wins.

The frozen native corpus contains73 programs:66 accepted/792 numeric readings
and seven genuine load rejections.293 unique hashes pin292 CNS/CMD/AIR/stage
originals and the probe. Intent checks identify actual BindToTarget controllers
in state700, including the two-controller cancellation program. Baseline controls
confirm NumTarget=1 and the actual target's custom state810.

## Selection, schema and evaluation

ID defaults to-1; only-1 selects any ID. Other values match the exact acquired
HitDef ID, including negative values. ID evaluates even without targets. If no
target matches, time/position expressions do not evaluate. The selected-target
order is the existing acquisition order; broader multiple-target controls remain
a separate interaction gate. For a match, evaluation is ID, time, X, Y.

Time defaults to1, -1 persists, values below-1 become1, and0 cancels the selected
target constraint. Time0 still evaluates position and moves the caller. Direct
float integer literals and unknown expressions reject at load; runtime floats
truncate, and redirected bottom becomes zero. Unselected expressions still
undergo compile validation.

The optional third position field is a literal point. The beta uses its first
case-insensitive letter: f/h/m select foot/head/mid. `foobar`, `hampers` and a
`foot,0` suffix are accepted; a quoted `"head"` and an invalid initial letter
reject. This parsing behavior is deliberately different from validating only the
three documented complete words. The literal is never a numeric expression.

## Positions and retained target offsets

Explicit X/Y position the caller relative to the target's current axis or its
own CNS head.pos/mid.pos, with X oriented by target facing. The caller's facing
is not changed. Its own velocity is retained, including Turn's momentum rule.
After the caller moves, the reverse constraint moves the target and publishes
the caller's velocity in the target's facing/profile. Target GetHitVar(isbound)
uses the existing combat-binding policy; the caller does not become isbound.

The caller keeps fractional position. The reverse offset truncates to whole
game pixels and converts back to the target's storage profile. Head/mid points
also truncate after conversion to game pixels. Native1280/640 game widths and
320/480/1280 character profiles distinguish this from rounding in character
units or preserving a float offset everywhere. No comparison tolerance changed.

An omitted axis leaves the caller's corresponding position unchanged and reuses
the acquired target's stored offset. Those offsets survive the expiry of a prior
TargetBind. Initial offsets are the beta's -1025 sentinel in the acquiring
character's units. Thus an empty controller or a single-component pos can send
the target offscreen while leaving an axis of the caller unchanged. Fresh
native repetition reproduces the empty case; disabled ScreenBound controls
separate the sentinel from ordinary screen clipping. Explicit large Y controls
prove this is not a global Y clamp. Production code retains offsets with the
acquisition record instead of replacing omission with zero.

Finite binding lifetimes include the issuing pass. Step-velocity controls
separate1/2/3/-1 durations from the velocity inherited after release. TargetDrop
releases the constraint; a target leaving H loses NumTarget but its already
issued constraint can continue. The latter does not imply a new eligible target.

## Evidence and limits

Manifests and intent metadata are temp/m4cm-{native,extra,follow}-*.json; native
runs are temp/reference-runs/m4cm-*. The fresh empty repeat is m4cm2-fresh-empty.
The reference executable remains unchanged. Read-only disassembly corroborates
the measured mechanism: controller0x47 dispatches at0x44a64d; defaults at0x446b9d
use -MaxFloat32 for omitted axes, and runtime branches retain the previous
record offset for that sentinel. This is supporting evidence, not a replacement
for the authored native comparisons or a claim about unmeasured interactions.

Focused194-program Match races pass125.315s. Engine/System/CNS races pass
1.434s/9.260s/1.012s; Character passes11.183s. Its first run found an overly broad
legacy target*.json glob; selecting the original target-/targets- prefixes
preserves all49 existing cases and fixes discovery without changing observations.
Three actual-window white masks below the HUD match8,379/7,455/3,474 native
pixels exactly. Full-region RGB still differs; this is not framebuffer parity.
Stock win/rematch/win was visually inspected;82 balanced owned events,1,971,901
private PCM frames and source/config/binary/PNG/audio checks pass. All293 native
hashes and1,095 final focused hashes verify; only the legacy test-discovery file
changed after the focused Match run. All sessions awaited/no clients remain.
HANDOFF.md and temp/m4cm-final-evidence-audit.json record the final evidence.
The latest full integration remains M4cl (3,435 results/36 packages); it is not
rerun for every small checkpoint. Multiple acquisition/overlap, helper retirement,
foreign-state ownership, incoming-hit and wider pause/hitpause combinations stay
explicit interaction gates. Remaining Explod/shared effect/controller work is
still item3; modes/assets/final platform conformance remain item4.

The staged whitespace check reports trailing blank lines in three immutable CNS
originals: targetself-release, targetself-extra-step-release and
targetself-target-turn. They remain byte-for-byte intact. With blank-at-EOF
excluded, staged whitespace checks pass; all384 staged files are UTF-8 text/
source under game/mugen/, with no binaries.
