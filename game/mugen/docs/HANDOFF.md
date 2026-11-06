# Current handoff — M4cz validated, shared shadows next

Continue ALL item3 effects/controllers with documented local commits. Only author
under game/mugen; no pushes, binaries, agents or goals. Item4 remains unallocated.
Read ../AGENTS.md. Do not stop automatically at a checkpoint.

## Runtime status and next action

No active runtime. Final integration10305 completed/awaited1; SEPARATE16,814
frozen-input/clientguard PASS. The complete154-app matrix has152exact crops,
77,337,600RGBpixels. ONLY missing-shadow1/-1 remain294RGB/max32 each; strict exit1
preserved. All625full native test results pass; all843engine/character results
pass this exact source in focused4. Four old masks pass. Final stock21win,
26FIGHT rematch and45secondwin screenshots visually inspected;82balanced owned
inputs and1,918,419private PCMframes, no run error. Modules/inventory pass.

Final evidence audit35042 completed0. temp/m4cz-final-evidence-audit.json checks
67,882 frozen-source entries/16,814unique paths,646 original native source/probe
hashes and all final native/config/binary/capture/audio/module/inventory evidence.
Additional temp/m4cz-reference-resource-audit.json verifies109 original manifest
files and6601 native sprite/sound/font/palette copies. Final visual record is
 temp/m4cz-final-stock-visual-review.json. No native original/golden changed.
Final binarySHA256:2ee879965eedf7ec4230c72c13b8fce8af8103d49bf746611b79bd90e0a69f94.

NEXT scoped staging audit(UTF8/noNUL source/text, only game/mugen) and local M4cz
commit; record hash/path count in temp/m4cz-commit-record.json. Check whether it
already exists before committing again. No push. Continue ALLitem3 immediately
with shared shadows. README, COVERAGE, COMPATIBILITY, CHECKPOINTS, test docs,
ITEM-3-PLAN, inventory and PROJECTILE-TRAILS have current evidence and limits.

## Current implementation

Previous checkpoint M4cy3270a56ee8be00d80742c0df9eeceb795c96509d. M4cz production files:
engine/after_image.go, engine/projectile.go, character/after_image.go,
character/projectile.go, character/hit_chain.go. New match/projectile_trail_test.go
and tests/projectile_trail_render_probe.py plus native source/text corpus.
Controller inventory sources/findings regenerated after deliberate review;
74typed/15fallback/3stubs/9partial/17bounded/60route-only, no overall percentage.

Nested Projectile AfterImage uses an explicit time-presence compilation gate.
Missing time ignores siblings entirely; explicit zero still validates them.
Ordered nested expressions run on issuer variables after admission to both
projectile and GLOBAL root/helper/projectile history pools. A new projectile
needs its own slot even if its issuer already owns one. Core args still evaluate
when either pool denies admission; ordinary projectile creation survives an
AfterImage-only denial. Owned palette creation clears initial localFX, keeps maps;
recorded mappings are immutable snapshots. Initial red brightness is validated
but never evaluated. Defaults/palette/blend policy share the actor compiler.

Actors retain slots after stop/drain. Finite projectile contexts release after
configured duration+length*timegap active passes (negative time<-1 uses1 for
expiry but shows no old poses). Removal/cancellation frees history immediately;
helper-created projectiles become root-owned and outlive their creator. Owner
freeze gates projectile motion/history. Positive Pause/SuperPause allowance is
inclusive at p.Time==allowance; zero freezes even a newborn during existing pause.

Projectile history captures completed AIR AFTER advancement; actors record
before it. Hitpause has THREE clocks: samples are forced regardless of timegap,
and sample phase advances. Duration uses precontact pause state: new-contact
pass consumes duration, subsequent paused passes retain it. Allocation expiry
uses POSTcontact state and freezes on all paused passes. Do not reuse disproved
early inference that the gap phase freezes or that duration always advances.

Revision4 fixes two final boundaries: inclusive positive allowance and removal
cleanup at nonnegative AnimTime. Infinite AIR gives positive(time+1), so ==0 kept
an extra removal sprite. Finite and infinite target controls remain immutable.

## Native corpus and preserved revisions

161 original native programs:154accepted, five target schema rejections, TWO
empty-state AUTHORING rejections.1848captured values/646 unique original source-
probe hashes. ALL161rows and11review sheets inspected before promotion ONCE.
Base49, additional106accepted and boundary4 expectations are in separate ledgers;
two authoring errors stay in the additional ledger, without behavioral JSONs.
All161prepared/final configs exactly reconstruct FirstRun1->0 and LF->CRLF only.
Original recorders/sources/resources/captures/goldens are immutable.

Groups and ledgers under tests/testdata/reference:
- trailprojectile49; projectile-trail-native-ledger.json and
  projectile-trail-config-finalization.json.
- trailpool58/trailphase48 originally; additional native/config-finalization
  ledgers hold108programs,106accepted plus two authoring diagnostics.
- Four new trailphase-boundary cases; separate boundary native/config ledgers.
  Current replay counts:trailpool58,trailphase52,base49; additional intent110.

All frozen temp inputs groups: m4cz-{projectile,extra,timing,followup,finalcontrols,
boundary}-inputs.json. Native final sessions completed/awaited and separate
646-input/client guard PASS. Boundary four budgets5/6 at valid-owner pauseTime4
show6/7 moving poses, bboxright559/613, in both pause kinds.

Initial baselines preserved:13base numeric failures/eight app cases;
99additional numeric failures/four app cases; boundary four real app failures
8019RGBpixels/max214 each, session34357 completed/awaited1, separate16808input/
clientguardPASS. Previous strict probe versions archived in temp/m4cz-baseline-
render-probe.py and m4cz-r3-render-probe.py before disjoint ledger extensions.
No pixel tolerance changed; final captures4seconds versus earlier7 compare the
SAME original crops, with the fixture world frozen at Time8/10/16.

Revision1:162focused results pass;20apps17exact/3failures(animated21612,
finite-hitpause21420, hitpause-gap2 4284RGB). Revision2:8apps7exact; finite
palette12852/max50 remained. Revision3:allthreecounterexamples exact/1526400px.
These diagnostics are preserved, including native age-candidate analyses that
did not uniquely distinguish the clocks.

Integration1 session72254 completed/awaited1; separate16789input/clientguardPASS:
843engine/character results pass;616native pass/5failed results(three subcases
plus two wrapper failures),397.936s. Subcases projspawn-pause-move,
projspawn-super-move, projclock-birth-paused-lifetime-1 exposed allowance edge.
Full150apps144exact/6UNWAIVED: removal-visible and timing-removal-visible3/4/5
4359RGB/max214 each; shadow1/-1 294/max32 each. Native LEFT has no removal sprite,
Go RIGHT extra (see temp/m4cz-matrix1-removal-visible-counterexample.png).
Four old masks pass; stock21win/26FIGHT rematch/45secondwin visually inspected;
82events/1919886PCMframes; modules pass. Inventory failed stale source hashes;
it was deliberately refreshed after final fixes, not to waive behavioral errors.

Focused4 session12039 completed/awaited0, separate16811input/clientguardPASS.
All843unit results,117selected native results and12targeted actual apps pass,
6105600RGBpixels exact. This resolves the old three native subcases and allfour
visible-removal failures. All baseline/failing artifacts remain unchanged.
Final full validation10305 is complete as recorded above.

## Subsequent allocated work and independent preparation

Shared shadow rendering next. temp/m4da-shadow-study-plan.md lists required
geometry/alpha/fade/palette/flags/layer controls. The original294pixel presence
controls clip most of the shadow; new stages must bring the WHOLE shadow into
view. Do not infer shadow math from Trans or from actor defaults.

Latest independent preparation is temp/m4da-shadow-draft3,62UNOBSERVED/
UNPROMOTED drafts. It fixes additive AIR field placement and adds explicit AIR
intent, root/helper/projectile trail-shadow separation, helper shadowoffset+/-10
and shadow/root overlap at three priorities. FULL CNS/AIR/STAGE intent parser
verification is required before freezing public inputs or native observation.
Old drafts remain unchanged, including the first count-assert authoring error.
Foreground/background layer overlap still needs calibrated controls. No shadow
runtime or implementation yet. See temp/item3-residual-research-queue.md.

Independent static ForceFeedback preparation:
 temp/item3-forcefeedback-static-findings.json and
 temp/item3-controller-static-xrefs.json. Exact supplied PE hash records successful
ForceFeedback and Null compiler-name matches storing the same ESI value at
controller kind+0x10 then jumping to common0x4472c2, without parameter parsing.
Store sites0x447f5a/0x4485c9. This is stronger than absent /dev/input or -nojoy,
but runtime schema/evaluation controls and dispatch review are STILL REQUIRED
before a demonstrated native no-op/complete claim. No implementation yet.
Do not invent gamepad vibration behavior ahead of the supplied beta target.

Continue remaining Explod creation/ownership/remap/random/angle/trans/shadow/
capacity, all AssertSpecial consumers, GameMakeAnim/MakeDust, clipboard formatting,
haptics/device/target boundary, residual StateTypeSet/VarSet/FVar schemas/aliases,
HitAddHUD. Native VarSetliteral7.9 rejects; later fix conflicting old unit inputs
to7.9+0.0 expressions without modifying native goldens. All item3 remains allocated.
Item4(modes/teams/AI/BGM/assets, remaining triggers, final font/renderer/platform)
remains unallocated. No source-route percentage proves engine completion.

## Preserved previous checkpoint limits

M4cy committed269 scoped UTF8source/text paths, no binaries/push. Full scoped race
4243results/36packages(25tested/11compiled-only), four masks, stock82inputs and
1920802PCMframes pass. Actor apps46/48exact: current-only45degree534RGB/17maskXOR,
trail45degree2285/65,max148, remain UNWAIVED with old12M4cq/sevenM4cv. Quarter-turn
trail exact. Read AFTERIMAGE-ACTORS.md. Integration never waives strict failures.
