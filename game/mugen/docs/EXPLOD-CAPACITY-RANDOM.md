# Explod admission and random displacement — M4df study

Target: supplied M.U.G.E.N 1.1 beta1. This checkpoint implements the measured
admission and displacement consumers described below. M4de checkpoint: 3649165.

## First 28 native programs

All 28 complete state/controller/resource intents were verified before recording.
Original 118 source/recipe hashes and all earlier originals were guarded during
the native batch. Session 68967 completed with observed exit 0; a separate
1,875-original-input/client guard passed. Every native-only numeric/color/geometry/
seed row and all five full-body sheets were reviewed before any promotion/fix.
There are 25 accepted controls / 300 numeric values and three load rejections.
These observations were subsequently promoted once with the other groups.

Configured ExplodMax -1, 0, 1 and 8 all admit eight persistent objects from twelve
requests; 9 admits nine and 16 admits all twelve. Every accepted object has its
complete portrait visible, with 4,284 foreground pixels each. Animation, ownpal
and remappal expressions execute for all requests, including saturated requests;
late ID expressions execute only for admitted objects. Missing animations consume
no capacity but retain the earlier palette-expression phase. Changing SysExplodMax
to 0, 1 or 8 does not limit twelve character Explods under ExplodMax 16.

Valid animation creation consumes two draws from the logged initial Match RNG
stream even if random is absent, zero, negative or bottom. Missing animation
skips both draws. RemoveTime 0 still consumes them. Each control's three following
Random observations uniquely identify their stream positions, without a fitted
prefix. Some individual values collide later in the stream; the complete
three-value suffix disambiguates them.

All visible random controls are exact integer translations of the immutable
native angle-zero image. Negative ranges become zero. Bare float components,
excess components and unknown triggers reject; a scalar and a trailing comma
are accepted. Runtime fractional expressions truncate. The observed source
values/displacements support two independent draws with offset
`raw % (max(0, n) + 1) - (max(0, n) + 1) / 2`, using integer division. This differs
from relying on the manual's symmetric range example. Evaluation/draw ordering,
facing, helper/opponent sharing, lifetime and saturated draws need follow-ups.

## Dependency controls

Nineteen separately authored controls have fully verified state headers, all
controller kinds/triggers/params, helper/opponent state additions and unchanged
AIR/CMD/stage resources. The verifier completed with observed exit 0 and a
separate 431-input/client guard passed. Native session 98018 completed with observed exit 0. Frozen
original map: temp/m4df-extra-original-inputs.json (82 entries); combined guard:
temp/m4df-extra-inputs.json (1,955 entries). All observations were reviewed before promotion or changes.

Probe recipes, raw captures, seed logs and diagnostics remain ignored under
temp/. Native originals and comparison tolerances remain immutable. Broader
system/hitspark pool interactions and final shared renderer obligations remain
open after the first-look probes.

## Reviewed dependency observations

The 19-program dependency batch completed with observed exit 0 and a separate
1,955-original-input/client guard. All 19 rows/four complete-body sheets were
reviewed: 14 accepted / five rejections / 168 values. The combined 47 programs
were promoted once only after all native review and exact configuration checks.
Roots, helpers, opponents and common-resource objects share the same pool.
RemoveTime 0 occupies slots until the simulation advances; RemoveExplod releases
a slot immediately, including same-pass reuse under another ID.

Creation evaluates pos, random X, random Y, velocity, scale and alpha in the
measured relative order (123456). Each random component's expression is followed
immediately by its own draw: pos-expression draw 1, X-expression draw 2, automatic
X draw 3, Y-expression draw 4, automatic Y draw 5, alpha-expression draw 6.
A saturated ninth request skips its random fields/draws. Displacements stay in
world axes when the issuer turns; the 1.0 profile follows the same measured rule.
Leading/empty tuples, second bare floats, unknown second components and a comma
after both components reject. A single component with a trailing comma accepts.

The preserved baseline fails 37 individual native subcases (39 failures including
two enclosing tests). Four of six capacity images fail by 4,284–18,075 RGB pixels,
one application exits at the erroneous system limit of one, and same-pass removal/
reuse already matches. Both paired baseline sheets and the runtime diagnostic
were reviewed. The revision below corrects these recorded counterexamples.

Six further real-HitDef/Clsn controls now establish hitspark pool sharing in both
directions. The first three (session 47309) and three RNG/marker follow-ups
(session 90165) completed with observed exit 0, separate 1,971 / 1,987 input/client
guards, and complete numeric/color/geometry/image review. Every captured spark
is a separate complete 4,284-pixel portrait above the actors; owned Explods occupy
the lower row. Under capacity eight, eight owned objects suppress the spark;
a pre-existing spark leaves room for seven owned objects. Capacity nine shows
all eight plus the spark. The follow-ups pin MoveHit 1 / MoveGuarded 0 and show
that accepted or suppressed hitsparks consume **no** RNG draws. Do not apply
Explod's automatic two-draw rule to every animation cue.

The initial cue controls deliberately transition the receiver into idle state
701: late NumTarget is zero and Life remains 1000. These values are preserved;
they are not used alone to prove contact. The separate marker controls and the
unique full spark image establish the actual HitDef consumer. All six cue
programs were promoted once after this review.

## Implementation revision one

The compiler parses typed random components, always supplies both zero defaults,
and draws immediately after each component evaluation. The character stores the
chosen world-axis offsets independently of facing/binding. Capacity admission
sits after the established animation/palette phase and before later expressions.
World counts active objects across roots, helpers and retired owners together
with live/queued scene cues. Scene queue admission uses that same count and drops
saturated cues without terminating the game. Production now binds ExplodMax
(minimum eight) rather than using SysExplodMax for this pool.

An optional `--match-seed` reproduces the initial match seed; later matches use
the independent session sequence. Normal clock-based startup stays unchanged.
Every new match logs its seed. The application runner forwards/records the seed,
and strict RNG comparisons verify the actual logged value against the immutable
native log. The old runner and comparators remain archived with their baselines.

Focused races pass 869 core results across four packages and 236 selected native
results. All 12 seeded application crops match exactly. A separate 18,495-input/
client guard passes. Full shared-boundary integration and application acceptance are complete below.

## Completed checkpoint validation

The 53 original programs contain **45 accepted controls, eight load rejections
and 540 numeric values**. All complete controller/resource intents, native-only
numeric/color/geometry/seed rows and full-body images were reviewed before
promotion once. Four disjoint native and configuration ledgers preserve them.

The full scoped race suite passes **4,743 results across 36 packages**: 25 tested,
11 compiled-only, no individual skips or races. Match takes 2,211.607 seconds.
Fresh focused races pass 869 core / 236 selected native results. All **45/45
accepted application crops match exactly (100% of this corpus)** across
**22,896,000 RGB pixels**, and all 45 native/Go pairs were visually reviewed.
Every run uses and verifies the original native match seed.

Five older palette/order apps, four contact/fall masks and the older quarter-turn
app pass. The old 45-degree case still differs by 534 RGB pixels / maximum 148;
its strict comparator and the enclosing acceptance driver retain exit 1. All
previous shared renderer failures remain unwaived; no sampling code or tolerance
changed. Stock win → rematch → win is visually reviewed with 82 balanced input
events and 1,971,594 private stereo PCM frames.

The passive audit verifies 18,498 unique current frozen paths / 73,973 hash
checks; 2,036 original input paths including unpromoted direct controls; all 53
configuration finalizations; 41 canonical archives; 2,173 native resource copies;
and all 109 original manifest files. Validated executable SHA-256:
22c20d06d6da49c4779e8d193ee98dfc273f7c7790fa554dae6ebd647cba6577.
Runtime artifacts and executables remain ignored. Records:
temp/m4df-final-evidence-audit.json and temp/m4df-final-visual-review.json.

This is bounded Explod/hitspark evidence, not whole-engine compatibility. System/
motif SysExplodMax behavior, wider resource/profile/foreign-state combinations,
ModifyExplod quoted enums and previous raster obligations remain open. The
independent AssertSpecial, system animation/dust, clipboard and residual schema
work continues under item 3. Source routes remain 74 typed / 15 fallback.
