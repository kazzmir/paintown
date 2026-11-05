# Native Explod mutation and drawing projections — M4ck

M4cn correction: bindID-1 selects the caller, including player2/helpers. The
first-player-only M4ck controls did not distinguish that from selecting the
first root. Failed/nonselecting retarget IDs retain an existing anchor; the
shared creation/modification consumer now follows this policy. Native second-
player controls and actual windows verify it. Owned movement limits also use
native age, not an independently spent pause budget. Read EXPLOD-ATTACHMENT.md.
Alpha/rotation differences below remain open.

ModifyExplod now has a typed compiler and a production mutation consumer. All
89 manual controller names are registered, but registration is not behavioral
completion: 14 stubs and other partial consumers remain. This checkpoint is a
bounded mutation/schema slice; it deliberately remains a partial audit row.

## Evidence

The `modexplod-*` corpus contains 143 genuine programs, 97 accepted with 1,164
numeric slots and 46 load rejections. All 572 CNS/CMD/AIR/stage originals are
hashed. Two sequence controls pin the complete 17-expression execution order;
separate fixtures exercise misses, duplicate IDs, per-object assignments,
aliases, partial tuples, direct float errors, redirected bottom, finite/infinite
animations, retargeting and combined/negative/right-angle drawing transforms.

Three additional `alphamix-grid-*` programs create nine real Explods and nine
real ModifyExplods each. They pin 36 state slots and 27 source/destination alpha
pairings for the shared blend research below. These add twelve original hashes;
state replay does not imply that their colors match. Intent tests check actual
parsed controller kinds/counts, not controller labels.

The original three `*-bound` drafts contain duplicate bindtime parameters: the
first is zero, so these are unbound controls. They are preserved, including their
native observations. Separately named `supp-*-bound-unique` controls contain a
single -1 parameter and establish the actual bound cases. The parser already
implements the beta's first-ordinary-parameter rule. No observed source, PNG or
numeric golden was repaired in place.

## Measured schema and execution

ID evaluates once and defaults to -1, which selects all owned active objects.
Other IDs use exact matching: -2 matches none. All duplicate matches change.
Every selected object evaluates its own parameter expressions; a miss evaluates
only ID. The native order is ID, scale X/Y, angle/Y-angle/X-angle, bindtime,
bindID, ontop, removetime, supermovetime (or supermove), pausemovetime,
sprpriority unless ontop, shadow, removeongethit, then alpha X/Y.

Omitted fields preserve existing values; a one-component scale preserves Y.
Ontop can suppress priority evaluation, including an existing ontop effect.
Modern supermovetime suppresses the deprecated supermove at execution, while
both still undergo load validation. Direct float integer literals reject;
runtime floats truncate and redirected bottom becomes zero.

Anim, facing, vfacing, pos, vel, accel, random, ownpal, remappal and postype are
parsed/validated but not evaluated or applied in these native controls. Space
is ignored by the modification parser entirely. An unrecognized trans string
also ignores alpha, even invalid alpha syntax. Recognized trans values parse
and evaluate alpha, including default/none/sub; non-additive rendering retains
its fixed policy. This separation of parsing, evaluation and application is
required to reproduce the tagged side effects and native load errors.

Binding retains the original oriented offset. Actual self/enemy IDs retarget
the anchor; bindID -2 preserves the existing anchor during modification. Binding
cancellation uses bindtime zero. Removetime is compared to the existing age,
and modification values below -1 expire at the end of that tick. Creation's
special -2 policy is separate, as finite and infinite controls demonstrate.

## Production drawing and remaining precision

Mutation reaches the shared owned effect instance. Scale, angle, X/Y rotation,
transparency, binding and lifetime therefore affect actual application frames.
Perspective source coordinates use an inverse homography rather than affine UV
interpolation. The combined native control determines the rotation composition
(Z, then Y, then X). The measured camera distance is 500 units in the 320-wide
game profile. The ordinary fitted game surface remains a separate sampling pass.

Eighteen isolated application windows were captured. Nine nonrotated scale,
selection and binding masks match every native white pixel. Seven rotation
comparisons retain these explicit white-mask differences:

| Control | Native pixels | Go pixels | Symmetric difference |
| --- | ---: | ---: | ---: |
| Z=30 | 7,655 | 7,651 | 64 |
| X=30 | 6,203 | 6,200 | 3 |
| Y=30 | 6,499 | 6,478 | 27 |
| Z=20/Y=40/X=30 | 4,661 | 4,659 | 4 |
| Negative combined | 6,087 | 6,083 | 68 |
| X=90 | 559 | 569 | 36 |
| Y=90 | 139 | 117 | 52 |

Bounds match except the thin Y=90 white support ends three pixels early. A
combined-angle window was visually inspected. These are demonstrated consumers
with open sampling limits, not exact framebuffer conformance. Full RGB also
differs at interpolated edges; whole-frame identity is not claimed.

The two alpha windows expose a shared blend-precision issue: the half-alpha
white is (250,119,242) instead of native (251,120,243), and the source-only
half-alpha white is (123,119,115) instead of (124,120,116). Their geometry agrees.
The three nine-value grids preserve the wider rounding evidence for the shared
Trans/blend checkpoint. A uniform-tint experiment did not fix it and was reverted.
No tolerance or native observation was changed to mark those colors as passing.

Shared alpha precision, shadows/reflections, complete priority ordering,
pause/hitpause budgets, helper/foreign/mixed-coordinate ownership and broader
resource/zoom/space interactions remain in item 3's dependent work. Final platform
sampling conformance remains in item 4. Do not promote this row to full coverage.

## Verification artifacts

The 143-program mutation plus prior binding race replay passes in 85.278s.
Engine/Effect/Character/System races pass (1.430s/1.011s/11.129s/9.318s); Render
compiles. Alpha-grid state/intent races pass3.171s. Stock win/rematch/win images
were inspected;82 balanced owned events/private PCM and all585 unique native
input/probe hashes pass. Application runtime inputs still match the18-window
snapshot; only two test files subsequently added alpha-grid coverage. Details
are in HANDOFF.md. Full scoped races follow the Explod integration boundary.

Ignored manifests begin `m4ck-{native,supp,sequence,check,policy,alpha}-inputs`.
Native runs are `reference-runs/m4ck-*`; final comparison windows are
`app-runs/m4ck3-*`. `m4ck3-app-audit.json` reports all differences and deliberately
fails its strict alpha assertion. Earlier shader/compile attempts and m4ck4's
reverted experiment remain diagnostics. Preserve them. No binaries are added.
