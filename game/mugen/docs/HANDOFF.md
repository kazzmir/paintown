# Current handoff

Updated 2026-10-18. The user authorized completion of all 27 HitDef keys
remaining after24b3ebd0, with local checkpoint commits and continuous iteration
until complete or blocked. No two-checkpoint pause remains. All authored work
stays under game/mugen/. Never push or add binaries. Current completed batches:
M4bn guard.dist, M4bo nine literal schemas, M4bp chains/juggle, M4bq
constraints and M4br palette. Continue M4bs contact shake and M4bt fall shake automatically.
Read HITDEF-REMAINING.md, COVERAGE.md and the relevant topic documents.

## M4bq verified checkpoint

Predecessor/reversal b13c22d5.39 programs/588 observations/seven rejections/156
frozen hashes. Topic HIT-CONSTRAINTS.md describes strict schemas, the undocumented
third snap duration, repeated source/default order, deferred offsets, immediate
binding, profiles/facing and entry-write/contact-axis phase. Numeric routing
61/81 plus nine literals =70/81 typed keys (86.4%); eleven effect keys remain.

All39 Match replays pass23.971s. Related binding-retirement/contact-facing/
down-bounce/stock-sweep regressions pass68.873s. Full Character11.508s and core
engine/evaluator/CNS/combat1.457/1.021/1.013/1.014s pass. Two fresh native
reproductions and three apps/four captures pass source/config/binary/PNG/runtime
provenance in m4bq-evidence-audit.json. Stock and indefinite-snap captures were
visually inspected. The stock preview was losslessly reencoded because the
image viewer showed black for a valid repeated PNG; its decoded RGB pixels
were verified identical. Captured originals/goldens are untouched.
run-m4bq-validation.py exited0. Nine Go hashes are frozen in
m4bq-production-inputs.json. Application SHA256:
125738f2800b6207e00036e0814ff47749f97df1471a273d897caefac32866a6.
Manifests: m4bq-prepared-{inputs,extra,routing,offset-profile,entry-routing}.json.
Native values/tolerances never changed; do not rerun authors. No full suite at
this batch: final scoped integration follows M4bt. Stage only M4bq families;
continue automatically, no push or binaries.

## M4br verified checkpoint

Predecessor/reversal572f2eb6.64 numeric + nine literals =73/81 typed keys
(90.1%); eight shake keys remain.24 hitpalette-* programs pin324 observations,
three native rejections and96 pre-native source hashes. Manifests:
m4br-prepared-inputs.json, m4br-prepared-lifetime.json and
m4br-prepared-negative.json.19 static accepted programs have12 native-only
color samples and a zero portrait-delta measurement each. Numeric values,
source inputs, existing body points/colors and tolerances were never changed.
Original body metrics remain in m4br-body-color-expectations-before-portrait.json.

HIT-PALETTE.md describes strict triples/coercion/order/defaults, signed
add-then-multiply byte rounding, time0/<-1 preservation and time-1 indefinite
behavior, guard/None exclusion, shared palette clocks, image transforms,
revision/cache/clone behavior and separate body/portrait rendering. Standalone
PalFX/AllPalFX/BGPalFX remain stubs, not promoted by these HitDef consumers.

Numeric Match24 races pass16.085s; engine/evaluator/CNS/combat/palette races
1.469/1.020/1.012/1.011/8.570s pass. The initial shared-clock test missed its
idle Null controller; test-only correction passed full Character11.422s.
First20 apps matched all body colors, but visual review discovered that Go
also tinted the HUD portrait while native did not. That earlier body-only run
is NOT final proof. Its artifacts/audit are retained under original *-m4br
names. The corrected State.MappedImage/Player HUD path and separate cache roles
are covered by a regression and nineteen native-relative portrait comparisons.

Final full Character races pass11.454s; palette recheck8.602s passes. Two fresh
native reproductions and20 rebuilt apps/21 captures pass provenance. All19
programs match228 exact body colors and retain unchanged portraits. Stock and
representative palette captures were visually inspected (lossless PNG previews
work around the image-viewer repeated-PNG display bug; original pixels/goldens
are unchanged). Final source/config/binary/PNG/scalar audit:
temp/m4br-final-evidence-audit.json.14 source hashes (13 Go plus Python) are
frozen in m4br-final-production-inputs.json. Application SHA256:
dc3b2b1da89c8d6b42a9f51f76a397f15f87c144aae3518f418376b81b859e92.
run-m4br-portrait-validation.py exited0. New apps/reports use *-m4br-final;
original buggy captures remain diagnostics. Root module/executable checksums
pass. Final full scoped races follow M4bt. Commit only M4br family; continue.

## Active contact/fall shake work

M4bs native21 hitshake-* programs are CAPTURED. Native exec28980 was awaited.
Manifests m4bs-prepared-inputs.json and m4bs-prepared-lifetime.json. No M4bs Go
changes yet. NEXT implement its four schemas/global clock/render consumer and
native replay, then focused/core/Character/apps/pixel gates and local commit.
Do not start M4bt Go changes until M4bs checkpoint is concrete.

M4bt native runner temp/run-m4bt-native-final.py is ACTIVE, exec session25580,
log m4bt-native-batch.log, on the private display/audio server.21 fallshake
source sets are frozen in m4bt-prepared-inputs.json and
m4bt-prepared-lifetime.json. Await it and separately check processes before
ANY native/app/GUI tests. Pure no-DISPLAY engine/evaluator/CNS/combat/palette
checks may run alongside it. All M4br GUI sessions are completed (29194 awaited).
No M4bt production source changes. Never rerun authors or derive native values
from Go. Final full integration follows M4bt; source must be frozen throughout.

Measured contact semantics: ScreenPosY includes shake NEXT receiver pass;
CameraPosY stays0. Baseline200; explicitfreq0/phase90/ampl4 gives Time0=200 then
196; negative amplitude gives204. freq90/phase0/time8 gives
200,200,196,200,204,200,196,200. Default phase atfreq90 gives
200,196,200,204,200,196,200,204. Omitted phase does NOT reevaluate frequency
source (side-effect counter1). Thus defer newly requested shake until end of
world frame, advance old clock before installing pending new request, start
at supplied phase, and advance phase by freq each later frame. Logical getter
uses current offset; render uses end-of-frame offset. Default amplitude is RAW
-4 in author units, not scaled by source localcoord. Mixed author640/receiver320
source default gives receiver+2 and caller redirected404, not receiver+4.
Use normal profile conversion once. Second default contact preserves an active
shake. Source21 also pins strict integer time/ampl, float freq/phase, bottom,
negative duration, guard, order and fractional frequency behavior; inspect all
JSON data before finalizing. Incoming None likely ignores, like palette, but
explicit None shake probe is not yet present. Ordinary EnvShake controller is
still a stub; decide its dependency scope honestly rather than promoting it.

The shared tests/hit_effect_probe.py now supports palettes and static rigid
shake displacement. Native baselines and provenance are pinned; Go cannot
record goldens. For shake, record static active positive/negative/zero/default/
mixed/guard cases from native before Go comparisons. It measures the entire
pure garment mask in a bounded ROI and searches a unique integer Y displacement.
No reference art is stored. ScreenPosY numeric programs already supply real
frequency/phase/clock traces. Final field order is in native hitshake order JSON;
compile four keys with correct types, defaults and side effects. Existing
engine.Timeline owns the shared frame; system/FightState.Draw composites game
canvas then separate HUD. Shake must move the world and ScreenPosY without
moving the HUD or changing logical CameraPosY. Reset its state at round reset.
Keep GetHitVar fall-shake metadata/controller work for M4bt after measuring it.

## M4bp verified checkpoint

Predecessor/reversal ff7a3296. Numeric routing58/81 (71.6%), plus nine literal
schemas =67/81 typed keys (82.7%). Fourteen documented keys remain. This is not
whole-engine completion. HIT-CHAINS-JUGGLE.md describes the precise scope.

Adds strict integer chainID/nochainID/air.juggle schemas, measured order and
coercion, prior-contact ownership/ID, guard/None/idle context, per-move and
per-projectile juggle costs, non-fall budget preservation and the typed chainid
query. The beta rejects GetHitVar(hitid), despite its manual's argument-summary
typo. GetHitVar now validates literal selector names before state effects.
An authored idle contact destination clears fall; later StateTypeSet retains
an existing flag. A narrow single-contact projectile adapter supplies the
necessary air.juggle consumer: native names, active animation/HitDef, removal
time, owner target credit and movement without another CNS/world update.
Full projectile schemas, cancellation, removal animations, queries and broader
entity/foreign/pause combinations remain open.

57 hitcombo-* programs pin660 observations, eight load rejections and228
pre-native source hashes. Manifests under ignored temp/:
- m4bp-prepared-inputs.json: use34 non-order rows.
- m4bp-prepared-extra.json: use six non-states rows.
- m4bp-prepared-final-states.json: three clean state-juggle controls.
- m4bp-prepared-final-order.json: two independent corrected order controls.
- m4bp-prepared-nochain-eval.json: two component-order controls.
- m4bp-prepared-nofall.json: four lifetime controls.
- m4bp-prepared-hitid.json: three native selector rejections.
- m4bp-prepared-zero-id.json: one valid zero-ID chain.
- m4bp-prepared-chainid-query.json: two query controls.

Do not rerun author scripts or derive expected values from Go. Two misplaced
snapshot-marker drafts remain archived/unpromoted in temp/m4bp-draft-order.
Three duplicate-state drafts were archived BEFORE native execution under
temp/m4bp-draft-states. Native JSON values/tolerances were never changed.

The enum/combat full scoped race suite PASSED before the final lifetime/query
extension: Match805.949s, Character11.753s, System4.790s, storyboard1.834s,
stock/resources6.734s; engine/evaluator/CNS/combat1.449/1.021/1.012/1.011s.
Log temp/m4bp-full-race.log; runner run-m4bp-validation.py exited0. This run
also covers the earlier M4bm idle-override correction. Eight source hashes
are retained in m4bp-production-inputs.json; this is the earlier source snapshot,
not the final tree.

Final lifetime/query correction gates PASS:144 Match replays90.507s (57 chain,
65 literal,22 HitFallSet), full Character11.481s and core races
1.445/1.021/1.013/1.012s. The first lifetime replay caught the missing typed
chainid dispatch; a later core test used a host without CharacterEnvironment
and was moved to a proper evaluator fixture. Diagnostic logs remain
m4bp-lifetime-first-* and m4bp-testhost-*. No production change followed the
passing Match/Character checks; only that test fixture changed. Completion
runner run-m4bp-complete-validation.py exited0, including core tests, two fresh
native reproductions and two rebuilt apps/three captures. Eleven final Go
hashes are pinned in m4bp-final-production-inputs.json. Final audit:
m4bp-final-evidence-audit.json. Application SHA256:
a97c3fd5eb5abc5e9b7405e767f314b632dda17a077e050116c732b22638085f.
The final stock7s capture was visually inspected and is byte-identical to the
previously inspected capture979e93044acd31accda05ba0439be525bb20ea39f8c9b4709e0e40ed399e1049.
Root module and immutable native executable checksums pass. Final full scoped
races follow M4bt, covering the lifetime/query changes and remaining fields.
Do not relabel the earlier full run as a final-tree run.

## Remaining frozen inputs

Unexecuted future source sets are already frozen:
- M4br:22 hitpalette-* programs; m4br-prepared-inputs.json plus
  m4br-prepared-lifetime.json; runner run-m4br-native-final.py.
- M4bs:21 hitshake-* programs; m4bs-prepared-inputs.json plus
  m4bs-prepared-lifetime.json; runner run-m4bs-native-final.py.
- M4bt:21 fallshake-* programs; m4bt-prepared-inputs.json plus
  m4bt-prepared-lifetime.json; runner run-m4bt-native-final.py.

These include positive/negative/zero/default/runtime/bottom/error, tagged order,
guard/profile and effect-lifetime controls. Source author scripts must not be
rerun. Dynamic allow_reject records the actual target's classification; reject
only when native says so. Stage explicit families at each checkpoint; keep
future fixtures out of earlier commits.

The promoted tests/hit_effect_probe.py covers palette colors/portrait isolation
and has a rigid-shake measurement mode awaiting calibration. Native/Go images
remain ignored. Hit palette rendering is implemented; global shake remains
pending. Character.EnvShake and PalFX controller methods remain stubs;
FallEnvShake currently delegates to empty EnvShake. Distinguish standalone
controller work from the HitDef consumers.

## Earlier checkpoints and remaining gates

M4bn7d30ae85: guard.dist strict integer; omitted/negative range preservation,
zero, facing/edge/height/profile and state-reset consumers.15 native programs,
204 observations/four rejections/60 hashes. Focused Match41.153s, full Character
12.156s, core/native/apps passed. GUARD-DISTANCE.md.

M4bo ff7a3296: nine literal schemas, stored contact animation, None contacts,
HitDefAttr and trip defence entry.65 programs/684 observations/eight rejections/
260 hashes. Focused Match55.619s; broader selection/flags/forced-standing/stock
sweeps97.325s, Character11.772s, core/native/apps passed. HIT-LITERALS.md.

Prior checkpoints24b3ebd0 downed-hit bounce and8779869a HitFallSet are recorded
in DOWN-BOUNCE.md and HITFALLSET.md. M4bm's full run had one diagnosed idle-
override failure; its targeted correction passed and M4bp's later full suite
now passes that integration. Preserve historical failed logs as diagnostics.

Unwaived: six M4az RGB alpha precision cases (maximum1), two M4bd spark raster
failures (276 native core pixels versus288 Go, maximum8), absolute KO/round
clocks, complete recovery/liedown/mashing/input routes, broad helpers/foreign/
projectile/reversal/team/pause combinations, modes/AI/BGM and full assets/fonts/
camera/effects. Absent Windows fonts still use a visible portable fallback;
no native glyph/metric parity is claimed. The full engine is not complete.

## Scope and environment

Branch go-rewrite-mugen; local commits only, preserve unrelated root files.
No agents, skills or goal tool active. Sandbox bwrap startup is broken; scoped
require_escalated commands pass automatic review. No approval blocker.
Do not touch the host display/audio/keyboard/focus.

Private display DISPLAY=:97,
XAUTHORITY=/home/juvinious/projects/paintown/game/mugen/temp/virtual-display/Xauthority,
LIBGL_ALWAYS_SOFTWARE=1. Xvfb PID2146490. Private Pulse PID1393643:
PULSE_SERVER=unix:/home/juvinious/projects/paintown/game/mugen/temp/audio-reference/native;
PULSE_COOKIE points to that directory's cookie; ALSA_CONFIG_PATH to its alsa.conf.
Never print cookies. Native/app/audio/GUI tests run sequentially. Await the prior
session, then separately check ps -eo pid,stat,comm for mugen.exe/mugen-go and
match/character/system/tests/storyboard test clients. Pure engine/evaluator/
CNS/combat tests without DISPLAY may run alongside native captures.

Build caches: GOCACHE=.../game/mugen/temp/go-build,
GOTMPDIR=.../game/mugen/temp/tmp, GOPROXY=off,
GOFLAGS='-modfile=/home/juvinious/projects/paintown/game/mugen/mugen.mod -mod=readonly'.
Full suite: game/mugen/mugen.sh --check -race -p1 -count=1 -timeout=20m.
Use non-Go suffixes for disposable source snapshots: recursive tests discover
ignored .go files too. Root checksum baseline:
temp/upstream-post-merge-module.sha256. Native executable SHA256:
d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e.
