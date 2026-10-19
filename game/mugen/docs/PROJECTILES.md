# Projectile spawn, motion and ownership (M4ce)

Predecessor/reversal: ded53de8. Target: the immutable Elecbyte M.U.G.E.N
1.1 beta 1 executable. This checkpoint implements typed projectile setup,
independent motion/animation/pause clocks, basic lifetime/bounds and root
ownership. Multi-hit/contact/removal-animation/cancellation/event consumers
continue in M4cf/M4cg; this is not full projectile conformance yet.

## Measured setup and behavior

The field-tagged native program proves this runtime order:

1. projid, projremove, projremovetime, projshadow, projmisstime, projhits,
   projpriority, projhitanim, projremanim, projcancelanim;
   velocity X/Y, velmul X/Y, remvelocity X/Y, accel X/Y, projscale X/Y.
2. The HitDef payload, including its existing source defaults/evaluation order.
3. offset X/Y, projsprpriority, projstagebound, projedgebound,
   projheightbound X/Y, projanim, pausemovetime, supermovetime, ownpal,
   remappal X/Y.

The compiler shares HitDef schemas through compileHitDefValues without wrapping
away evaluator/coordinate/redirect interfaces. Reverse source declaration order
preserves the same 44 evaluations. Both remappal expressions evaluate even with
ownpal=0. Omitted removal/cancellation animations reuse their predecessor's
source expression, evaluating it again, rather than copying its numeric result.
projhitanim side effects therefore execute three times when both successors
are omitted. Direct float literals reject in integer fields; runtime values
coerce through the existing typed parameter path. Defaults and field shapes
are in engine/projectile.go.

Creation is immediately counted. removetime 0 is absent at Time=1, 1 at Time=2,
and 2 at Time=3; -1 persists, while -2/-99 behave like 1. A finite active
animation loops. Missing/negative actions still leave objects counted. Single
velocity components supply Y=0; single velmul supplies Y=1. A single
projheightbound retains its profile-scaled high default. Extra velocity
components reject. postype=bad is accepted as a back prefix; xyz rejects.

Screen-edge removal also removes stationary/inward objects outside its range.
Stage and height bounds instead remove objects moving farther outside their
range; stationary objects and those moving back toward the range survive.
Height checks see the newly accelerated velocity. Stage travel limits derive
from camera bounds plus half the viewport, rather than PlayerInfo movement
limits. Frozen directional controls distinguish these rules.

All six postypes are accepted. Native sprites demonstrate a discrepancy with
the manual: **front/back/left/right all anchor at screen center**, while p1 and
p2 use the corresponding player axis. Both facings are calibrated independently.
The PNG-derived translations are retained in JSON, independent of Go's result.
Y offset remains issuer-relative. Projectiles keep the issuer's local geometry,
AIR/sound/effect resources and palette state. Own palettes clone and optional
remap uses the existing palette path; shared palettes follow their source.
Broader foreign-profile and palette interactions remain unverified.

A helper-created projectile immediately belongs to the root and survives helper
destruction. Helper NumProj/NumProjID return zero. ProjID -2 normalizes to 0.
NumProjID requires one expression: negative arguments query exact ID 0, float
arguments truncate, and bottom stays bottom (Cond distinguishes it from zero).
Bare, zero-argument and two-argument forms reject at load with source context.
Basic counts moved from planned M4cg into M4ce to measure lifetime/ownership.

## Evidence and checks

104 original programs were frozen before capture: 95 accepted, nine load
rejections and 416 CNS/CMD/AIR/stage hashes. They pin 1,192 distinct variable
slots / 1,212 capture readings; two tagged controls have ten overlapping slots
each across pages. Twelve isolated sprite controls add native position deltas
and exact white-body masks. Inputs and numeric goldens remain unchanged.

The initial 15 sprite/fallback drafts used an empty background list, which beta
rejects. Their 60 original inputs and raw captures are preserved unchanged in
ignored temp/m4ce-rejected-empty-bg and reference-runs/. They are preparation
diagnostics, excluded from feature acceptance. New projdraw-* replacements use
the known-valid stage. The first Go development replay found one implicit
height-bound default mismatch; its diagnostic log is retained and the corrected
source passes the original native observation.

Focused Match races PASS 135.338s, including all new programs, 57 historical
chain/juggle controls, legacy hit-velocity controls/counterexamples, Random and
HitPauseTime. Core races PASS: Character 11.450s, System 9.636s, engine 1.476s,
evaluator 1.023s, CNS 1.013s and combat 1.011s.

A fresh reverse-order native program reproduces all 48 capture readings.
Three actual projectile apps match native opaque body masks exactly:
4,473 / 4,293 / 4,473 pixels and the corresponding bounding rectangles for
front/right-facing, back/left-facing and p2 origin. This establishes bounded
placement/rendering, not general filtered/alpha parity. A separate four-PNG
stock win/rematch/win replay passes owned input, binary/config/source/PNG and
private PCM provenance checks (82 balanced events). All 819 frozen source/
probe/fixture/module hashes and root module guards pass. All sessions were
awaited and no native/app/test clients remain at the checkpoint.

Artifacts are ignored: m4ce-focused-match.log, m4ce-core.log,
m4ce-final-inputs.json, m4ce-final-validation.json, m4ce-native-audit.json,
m4ce-custom-app-audit.json and m4ce-app-evidence-audit.json in temp/.
The last entire scoped race suite is M4cb; final allocated integration will
repeat it with a deadline appropriate for the expanded native corpus.
Source routing is now 112/127 trigger families (88.2%), 15 unbound.

## Remaining allocated work

M4cf must implement multi-hit/miss clocks, hit/removal/cancel phases, contact
ownership and cancellation box/priority rules. Wire and measure the shared
PlayerProjectileMax pool: default 32, documented minimum 5. Its config value is
parsed but not yet passed into World. Pin helper-created projectile motion and
lifetime on its birth pass: root ownership is corrected, but creation after the
root advances existing children may leave late-born movement one pass behind.
Preserve measured per-root CNS/clock ordering when correcting that case.
Foreign profiles, source multipliers, owner state changes, priorities and
paused/retiring capacity need their own controls. M4cg adds contact/time query
families and legacy indexed/comparison forms; M4ch adds reversals.

Projectile afterimages and shared stage-shadow rendering depend on the deferred
effects/renderer allocation. Parsing/storing their fields does not establish
visual support. Broader mode/team/foreign/zoom and simultaneous combat variants
remain explicit conformance gates. No whole-engine completion percentage follows
from this checkpoint or its source-route percentage.
