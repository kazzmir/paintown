# Projectile AfterImage — M4cz

Projectile nested AfterImage fields now have typed schemas, ordered expression
evaluation, global context admission, history/palette drawing and production
lifecycle consumers. This checkpoint leaves shared stage shadows for the next
implementation. The two original shadow-presence comparisons remain failures;
this is bounded compatibility evidence, not full engine completion.

## Measured behavior

`afterimage.time` is a compilation presence gate. Without it, sibling fields are
ignored, including an unknown multiplier trigger. Explicit zero still validates
siblings. Literal float time/length and unknown brightness/multiply triggers
reject with source context. Five original rejection fixtures remain unchanged.

Nested expression order follows the ordinary Projectile/HitDef/ownpal arguments.
Forward and reversed declarations produce the same tags: core projid1, velocity2,
damage7, projanim8, ownpal9; then time10, length11, timegap12, framegap13, color14,
invert15, green/blue brightness16/17, contrast18–20, postbright21–23, additions
24–26, multipliers27–29. Red brightness is parsed/validated but not evaluated;
its tag stays zero. Postbright is evaluated without affecting the observed draw.
Defaults, palette aging and blend policy share the actor compiler.

Admission to both the root projectile pool and GLOBAL root/helper/projectile
AfterImage pool precedes nested evaluation. Core arguments still evaluate when
admission fails. A full history pool still permits an ordinary projectile; a
full projectile pool skips nested side effects and creates no extra projectile.
An issuer's existing history slot does not exempt its new projectile from capacity.

Actor slots survive stopped/drained recording. Projectile slots instead retire
after their configured drain interval or removal/cancellation. Time0/length8
releases at8; time2/length4 at6; time2/length4/timegap2 at10. The observed time-2
case records no visible history and uses1 for expiry, releasing at9 for length8.
Length1 and disabled contexts still reserve during creation. A helper-created
projectile becomes root-owned, outlives DestroySelf and retains its own context.

Ownpal1 clears initial local PalFX while retaining inherited remaps. Shared
projectiles observe later owner changes; owned ones isolate them. Recorded poses
retain immutable palette mappings, including older mappings after RemapPal.
Local effects are replaced by the history palette policy; global composition
uses the shared measured palette renderer. Blank/disabled history slots and
selected-frame palette aging retain the actor behavior directly tested here.

## Distinct motion, recording and expiry clocks

Projectiles record after movement and AIR advancement; actors record before
AIR advancement. The animated two-frame portrait counterexample establishes
this difference, rather than assuming the actor phase applies to projectiles.

During selfhitpause, a projectile forces a stored pose on every pass regardless
of timegap. The sampling phase continues advancing. Duration observes the pause
state before new contacts: the contact pass consumes duration, then subsequent
paused passes retain it. Allocation expiry observes the postcontact pause state
and freezes on every paused pass. These three clocks cannot be collapsed into
one paused check. Native time 6 finite recording and nonneutral gap1/gap2 palettes
separate repeated samples, duration expiry and the first resumed moving pose.

Pause/SuperPause motion/history also requires the root owner to update. A
positive allowance includes the pass at the completed-age boundary; zero stays
frozen even for a newborn during an existing pause. With ownerMoveTime4 and pause
issued atTime4, budget4 permits no paused movement, budget5 permits one pass and
budget6 permits two. Budgets5/6 give six/seven total moving poses, right bounds
559/613, for both pause kinds. Unchanged older lifetime controls also pin the
birth boundary. Four earlier owner-motion labels used out-of-range MoveTime12
for time4; they are preserved as out-of-range controls, not owner-active evidence.

Removal clears history/context immediately. The native infinite removal action
has positive AnimTime(time+1), so removal completes at nonnegative AnimTime.
Using only equality to zero kept an extra Go portrait in four original controls;
the corrected cleanup passes them without altering their captures or goldens.

## Corpus and validation

There are 161 original programs:154 accepted, five target schema rejections, and
two preserved authoring rejections from empty state5000. The latter are explicit
diagnostics, not AfterImage rejection/hitpause behavior. All 1,848 captured values
and646 original source/probe hashes are preserved. Full CNS/AIR parameter/kind/
state intent was checked before native observation; All 161 native-only rows and
11 review sheets were inspected before expectations were promoted once.

Base, additional and boundary native/config-finalization ledgers are separate.
All 161 prepared/final configurations reconstruct exactly: FirstRun1 becomes0 and
LF becomesCRLF, with no other byte change. Original recorders, inputs, resources,
observations, captures, failed diagnostics and expectations remain immutable.

Fresh validation on the final source:

- All843 engine/character test/subtest results pass under the race detector.
- All 625 old/new native Projectile/actor AfterImage results pass under the race
  detector, including159 new behavioral programs and full source-intent checks.
- All 154 actual applications run successfully.152 crops match exactly across
  77,337,600RGBpixels. The strict matrix RETAINS EXIT1 for shadow1/-1, each294RGB
  differences/max32. This matrix is not described as wholly exact.
- Four old hit/fall masks pass. Stock win at 21s, FIGHT after rematch at 26s and second win at 45s screenshots
  are visually inspected;82 balanced owned input events and 1,918,419 private PCM
  frames pass. Module and regenerated source-inventory guards pass.
- Final audit verifies16,814 unique frozen paths,646 original native inputs,
  native/config/binary/capture/audio provenance. An additional resource audit
  verifies109 original distribution files and6601 native resource copies.

## Retained counterexamples and remaining work

Initial baselines keep13 base numeric failures/eight app cases and99 additional
numeric failures/four app cases. Revision1's20 apps had17exact/3failures;
revision2's8 had7exact; revision3 fixed the three recording cases. Integration1
then exposed three older allowance/lifetime regressions and four extra removal
sprites. Four separately authored boundary baselines each differed8019RGBpixels.
Revision4 fixes those boundaries:117 targeted native results and12 targeted
apps all pass, followed by the complete final validation above. No comparator
was weakened; earlier strict versions and all failure statuses remain available.

The shadow controls show only294pixels because most of their shadow lies below
the crop. They prove nonzero flag presence, not full projection, alpha, fading,
color or ordering. New independently named stages must bring the entire shadow
into view before implementing the shared renderer. Do not infer it from Trans.

Broader transform/AIR-blend/resource/foreign-profile interactions remain bounded.
Earlier12M4cq, sevenM4cv and twoM4cy nonquarter failures remain UNWAIVED. Continue
shared shadows, remaining Explod consumers, AssertSpecial, system animations/
dust, clipboard, haptics/target boundary and residual schemas. All item3 remains
allocated; broader modes/assets/final conformance remain item4, unallocated.
