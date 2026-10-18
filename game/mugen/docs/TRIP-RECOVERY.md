# Trip contact entry and the KFM sweep loop

M4bj fixes the user-reported endless sweep. The recurring trip transition in
Character.HandleBuiltinTransitions was introduced in ea3599303 (2026-03-08),
before M4bi. While the receiver retained a falling Trip hit, it forced every
state outside 5070/5071 back to 5070. That included lying-down state 5110 and
custom/override states. The latest fall-expression checkpoint did not introduce
this rule.

Native chooses the Trip reaction at contact and allows the later common or
custom CNS to finish it. The Go receiver now selects 5070 at contact and has
no recurring trip-state heuristic. The stock sweep completes
5070 → 5071 → 5110 → 5120 → 0 and regains control.

## Measured scope

Thirteen original `triproute-*` programs pin 156 numeric observations and 52
pre-native source hashes. Grounded/crouching Trip enters 5070 even with fall=0.
Airborne contact uses air.type, which inherits ground.type when omitted:
Trip enters 5070, explicit High enters 5020. Downed contact keeps entry 5080.
Explicit custom states and HitOverride destinations complete their own paths.
No expected values come from Go; sources match temp/m4bj-prepared-inputs.json.
Do not rerun temp/author-m4bj-trips.py. Reference assets and captures stay ignored.

Native groundtype/airtype values are None=0, High=1, Low=2, Trip=3. The old Go
mapping merged Low with High and reported Trip as 2. The literal air.type
field and typed GetHitVar(airtype) binding are now present. GetHitVar(type)
returns 0 in all entry controls and the separate late Low-type control;
the compatibility binding preserves those measured beta results. That does
not establish every selector/context variant or a complete enum schema.

One program uses the stock common get-hit/get-up states and stock text AIR
actions with the sweep's 72 damage, 12/12 pause and -1.5/-2 velocity. Relative
to the first receiver observation, native sees 5070/5071/5110/5120/0 at
ticks 0/13/29/89/103. The Go replay matches every recorded value. Replay
metadata permits 180 ticks for this complete path; other existing controls
keep their default 90 ticks, with a checked 1..600 bound.

The new real-input Match regression presses down+B once, in both mixed-profile
directions (KFM and KFM720 attacking), and requires all recovery states, exactly
72 damage and restored idle/control within 240 ticks. Both directions fail
before the fix, retaining 928 life while alternating 5070/5071; the diagnostic
and stock cases pass after it. Baseline failure log: temp/m4bj-before-fix.log.

## Validation and checkpoint

Focused Match races pass 13 new controls, both stock-input sweep directions,
38 forced-standing and 30 fall/recovery programs (58.555s). Engine/evaluator/
CNS/combat races pass 1.464s/1.024s/1.014s/1.013s. Two fresh native
reproductions and two real apps/four captures pass provenance checks in
temp/m4bj-evidence-audit.json; the recovered 6s fixture image was inspected.
Its magenta diagnostic stage fill is intentional. Application binary SHA256:
58ce59cc00276489949717ea027b8a7ebec2cdd7da8d5dfa4aa11b9c3cfb4ad1.
The final pipeline temp/run-m4bj-validation.py exited 0; no GUI client remains.
The combined M4bk integration gate covers both user-authorized checkpoints
and now passes, including both actual sweep directions
and all trip controls (full Match 693.752s); see FALL-VECTORS.md and HANDOFF.md.

The frozen stock-source fixture retains one original trailing space at line
1242 (`x = 0 `). Git's whitespace check reports it; it is retained to preserve
the pre-native source hash. The remaining checkpoint diff passes that check.

Expression routing remains 50/81 (61.7%): air.type is literal enum storage,
not a new numeric-expression route. Legacy recovery-command heuristics,
full liedown/mashing clocks, bounce, HitFallSet variants, juggle/chains,
broad contact/pause/entity combinations and absolute KO/round timing remain
open. Six alpha and two spark raster differences remain unwaived.
