# AfterImage actor contexts — M4cy

This extends [AfterImage history and schemas](AFTERIMAGE.md). The reference is
Elecbyte M.U.G.E.N 1.1 beta 1. Native observations precede implementation. The
46 original CNS/AIR/CMD/stage programs, observations, captures and expectations
are immutable. They contain 552 captured numeric values and 186 distinct
source/recorder hashes. All native table rows and three review sheets were
inspected before promotion. The public ledger is
`tests/testdata/reference/afterimage-actor-native-ledger.json`.

## Capacity and expression evaluation

`AfterImageMax` limits contexts globally across both roots and all helpers.
Limits 1, 2 and 4 admit that many of the three simultaneous root/helper trails.
Two-root controls independently show shared admission. A context retains its
reservation after recording stops, expires or begins disabled, with length 1,
and after all recorded poses have drained. Do not free slots when a trail is
no longer drawable. Reissuing the owner's AfterImage works at full capacity.
DestroySelf removes that helper's trail and frees its reservation; another
helper can acquire it on a later controller execution.

Fully drained and initially inactive contexts resume through AfterImageTime.
A missing context skips evaluation of its duration expression. An AfterImage
denied by the full pool also skips all parameter expressions. Native admitted
and denied controls have the same later marker 77, but their variable side
effect is respectively 1 and 0. Compilation still validates their schemas.
Stopping recording does not turn AfterImageTime into a missing-context call.

Production wiring now carries the setting from config through Match into World.
Default capacity is 16; the supplied configuration documents minimum 1. The
current native capacity controls use positive limits only. Reservations clear
on round replacement, and helper retirement releases its context immediately.
Projectile context ownership remains a separate next study.

## Drawing and clocks

An invisible actor still draws its history, including a recorded pose at its
current location. Only its ordinary current sprite is hidden. Blank AIR frames
consume history slots while older poses drain; the blank poses do not increment
the palette age of older drawable entries. Saved AIR animation, facing, offset,
angle and scale belong to each recorded pose. Broader missing-resource cases
and angle raster precision remain explicit gates.

The study includes live local/global palette changes, remapping, current Trans,
helper shared/own palette, Pause and SuperPause with movement budgets 0, 2 and 5.
All 46 native observations are retained. The complete Go matrix matches these cases exactly. Nonquarter rotation remains
a separate explicit failure, reproduced without AfterImage. Do not infer all
other combinations from the accepted cases.

## Baseline and provenance

The first baseline has two genuine numeric failures: denied AfterImage and
missing AfterImageTime incorrectly execute their side-effecting expressions.
A new intent-test key-case bug was also fixed without changing native goldens.

Seven original visual checks stopped before RGB measurement because the native
engine rewrites its copied configuration after launch. Exact reconstruction of
all 46 configurations proves that the prepared SHA matches the original
recorder and that the final bytes differ only by FirstRun 1 becoming 0 and LF
becoming CRLF. The separate `afterimage-actor-config-finalization.json` records
prepared and final SHA values; the original recorder, evidence and expected JSON
remain unchanged. Comparisons require the exact final hash, not a normalized or
partial configuration match.

Fresh read-only comparisons of the same baseline captures measure:

| Case | Differing RGB pixels out of 508,800 |
| --- | ---: |
| Shared capacity 1 | 51,300 |
| Invisible current actor | 4,284 |
| Blank animation with default palette | 8,559 |
| Live AllPalFX | 3,681 (maximum channel error 1) |
| Missing AfterImageTime context | 0 |
| Helper shared owner PalFX | 0 |
| Helper SuperPause movement budget 2 | 0 |

These failures and exit statuses are retained. Revision 1 adds capacity and
pre-evaluation gates, current-only visibility, blank selection aging, and native
integer coefficient quantization after merging the global multiplier. Focused
races pass 194 test/subtest results, and all seven rechecks match exactly
(3,561,600 RGB pixels). Complete 46-case actual-app validation followed. No comparison tolerance was relaxed.

The older 12 M4cq angle/raster and seven M4cv EnvShake precision failures remain
unwaived. This study does not establish an overall engine coverage percentage.

## Complete first matrix

All 46 cases were collected. 43 crops are exact. Shared helper remapping and
live root remapping each differ in 6,228 pixels (maximum channel error 52);
only older poses differ, while newer poses already agree. Live 45-degree
rotation differs in 2,285 pixels (maximum channel error 148). Original failed
measurements and exit status 1 are retained. New independent current-only
45-degree and trail 90-degree native controls are in progress; they distinguish
recorded state from the existing shared sampler's nonquarter rotation gate.

Two separately authored, parsed and frozen native diagnostics add 24 values.
Their native tables and review images were inspected before promotion. Current
45-degree rotation contains 4,495 opaque pixels; quarter-turn trail contains
29,151. They use separate native/config-finalization ledgers and never replace
old expectations. Revision 2 saves immutable palette mapping snapshots in each
recorded pose and retains those snapshots during sprite cache pruning. Focused
actual-app validation is in progress. The study now contains 48 programs and
194 unique source/recorder hashes.

Revision 2 focused validation passes 53 race-checked results, both remapping
counterexamples, and the two earlier M4cx reissue/default-aging regressions.
The new 90-degree trail matches exactly. Current-only 45-degree drawing, with
no AfterImage controller at all, differs in 534 RGB pixels and 17 silhouette
pixels; the original 45-degree trail differs in 2,285 RGB pixels and 65
silhouette pixels. Both have maximum channel error 148 and remain **unwaived**.
The independent current-only reproduction identifies a shared renderer gate.
The complete 48-case strict matrix and full integration followed; their results
are reported separately below. Integration does not waive image failures.

## Production ownership and cache lifetime

World owns global reservations; config supplies the limit through Match.
DestroySelf releases helper contexts, and new rounds clear old reservations.
The compiler checks admission before evaluating AfterImage expressions and
checks context presence before evaluating AfterImageTime. Compiled schemas
remain strict even when an execution would be denied.

History owns immutable palette snapshots. Poses recorded before RemapPal keep
their earlier mapping; later poses acquire the new mapping. A snapshot is reused
until its source pointer or revision changes. Sprite-cache pruning retains
palette states referenced by active history slots and disposes them once those
slots leave the history. Local PalFX is replaced by the trail's palette policy;
current global AllPalFX still applies to historical draws.

## Final checkpoint acceptance

The fresh complete matrix matches 46 of 48 crops exactly, totaling 23,404,800
RGB pixels. Both 45-degree failures remain unwaived and preserve exit 1; the
integrity audit does not convert them into passes. All other cases, including
both remapping counterexamples, are exact. The nine M4cx load rejections and all
189 combined native programs pass in the full Go replay suite.

Full scoped races pass 4,243 results across 36 packages (25 tested and 11
compiled-only), without failures, individual skips or races. Match takes
1858.841 seconds; summed package time is 1943.381 seconds. Four contact/fall
rendering regressions pass. Stock win at 21 seconds, rematch at 26 seconds and
another win at 45 seconds were visually inspected. All 82 owned input events
balance; private PCM contains 1,920,802 frames. Frozen source/fixture/native/
driver, binary/resource/config/PNG/audio, module and source-inventory guards
pass. Audit: `temp/m4cy-final2-evidence-audit.json`. Build SHA:
`6548ec120ce7aa468440210d091189ea9844e50c4021ca6409716fcfe6ff9104`.

This checkpoint closes the measured context/lifetime/remapping failures. It
retains the first failed 43/46 matrix, both initial numeric counterexamples,
configuration-finalization diagnostics, rotation isolation and every original
native capture/expectation. Broader palette ranges/tuples, foreign/resource/
ordering interactions, projectile trails and shared shadows remain separate
gates. Item3 continues; no full-engine or family-completion claim is made.
