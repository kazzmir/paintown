# Projectile event queries — M4cg

The four typed elapsed-time functions and indexed legacy ProjHit/ProjGuarded/
ProjContact comparisons have production routes. This increases the source-route
inventory to119/127 (93.7%); it does not establish whole-engine parity.

Native controls establish a single latest event per root across state changes.
Hit/guard events retain their authored projectile ID. ID0 and negative IDs are
wildcards; floats truncate and bottom propagates. A new contact replaces older
IDs and kinds. The first CNS observation is elapsed0. The owner's active clock
ages history at completion of its CNS/physics pass, freezing with Pause/SuperPause unless that owner can move. Helpers
have their own empty history; an explicit Root query sees the credited root.

Cancellation overwrites hit/guard history. Only wildcard ID0 matches the beta's
cancellation record, even when its cancelled projectile had an authored nonzero
ID. ProjContactTime and legacy ProjContact exclude cancellations. Higher-priority
survivors do not publish cancellation. Separate count controls prove that the
cancellation actually happened. The hit-then-cancel control independently pins
history replacement after real damage.

Legacy forms pulse on elapsed0; the optional elapsed comparison extends the
window. Boolean0 inverts that result, including absent history. Other accepted
positive literals are true. The main != spelling behaves like = in the measured
beta; the optional elapsed != comparison works normally. Bare/call forms,
dynamic/float/negative boolean literals, invalid elapsed-function arity and
recursive helper/root redirects reject at load time. The raw rejected captures
are retained. Initial guard-switch drafts stayed in hit state because the
controller was placed in5020 or a later negative state; corrected isolated
kind-switch controls measure the actual guard transition without editing them.

The first49-program replay found two helper Root samples one tick behind native.
Two additional owner-type controls prove that Projectile does not change issuer
MoveType. History ages at owner completion, so a later helper sees the newly
published age while an earlier root sees its previous helper variable. The
correction preserves the originals. All51 controls pass after correction.
Broad simultaneous/team contacts, extreme counters and final mode/render gates
remain separate. Original inputs and numerical observations are immutable.

Final checks PASS:51 native programs/456 values/13 rejections/204 authored
hashes. Core races: Character11.445s, engine1.445s, evaluator1.019s, System9.768s.
Selected query/cancellation/coordinate/clock/phase/Random/HitPauseTime Match races
pass88.019s. Fresh hit-then-cancel reproduces the immutable observation. Four-PNG
stock win/rematch/win/82 balanced owned events/private PCM, all556 final frozen
hashes and module guards pass. Sessions39895/59309 were awaited; separate client
check is clear. Artifacts: temp/m4cg-{focused,corrected-focused,final}.log,
m4cg-final-inputs.json, m4cg-final-validation.json, m4cg-native-audit.json and
m4cg-app-evidence-audit.json. Full scoped races follow final reversal integration.
