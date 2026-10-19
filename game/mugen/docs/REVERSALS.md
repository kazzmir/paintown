# ReversalDef and MoveReversed — M4ch

ReversalDef now has a typed compiler and a character-contact consumer. It
requires the reversal.attr literal mask and reuses the full HitDef argument
reader, including unused damage/ID expressions and repeated guard-pause source
defaults. Tagged normal/reversed declarations produce the same ten evaluations.
Missing/invalid postures and literal float pause/state values reject. Empty,
partial, quoted or unknown attack-class masks have measured empty-mask behavior.

Definitions persist until consumed, replaced by HitDef, or cleared by state
entry without hitdefpersist. Clsn1 overlaps Clsn1; neither party needs Clsn2.
Masks use the authored attack posture, unlike HitBy. Normal protection,
HitOverride and HitDef priority do not replace that selection. Idle/hit reversers
retain their move type. Repeated attacks need a renewed reversal. Pausetime
comes from the reversal, not the incoming attack.

Success disables the incoming character HitDef, increments MoveHit/MoveContact,
HitCount/UniqHitCount, and publishes MoveReversed to that character. Counters
follow owner execution order and pauses. MoveHitReset and state entry without
movehitpersist clear MoveReversed; retained counters remain visible in idle.
Reversal targets survive that counter/state reset, using the existing hit-
generation boundary. Reversal grants GetPower but ignores ordinary damage and
GivePower in measured controls. P1/P2 custom states/persistence have separate
frozen controls.

The beta does not reverse Projectile controllers in the measured NA/NP/SP,
idle/attacking-owner controls. The projectile remains active and visible; there
is no MoveHit/MoveReversed, target, damage, pause or projectile-query event from
the reversal. Character HitDefs with projectile attack attributes are still
reversible. The implementation therefore excludes actual projectiles rather
than treating an attack attribute as an entity kind. Ten genuine controls have
an explicit Go assertion: state701 must contain one Projectile and no HitDef.
Three isolated paused images retain the active projectile:7935 pixels at
[569,357,688,555], regardless of configured hit/removal/cancel animation.

Reversal sparks use incoming character geometry and reverser resources. Both
sparkxy offsets add. Native base/enemy/own/combined masks each have7935 pixels
at [617,357,736,555], [647,297,766,495], [632,327,751,525], [662,267,781,465]. The
shared effect path now uses the existing measured indexed sampler, flip bias
and pixel centers while retaining blend options. The first sampler mismatch
(8379 vs7935 pixels) is preserved. Broader renderer conformance remains open.

The promoted corpus has90 programs, four genuine load rejections,948 numeric
replay readings and84 additional early clipboard readings in seven renderer
probes.404 original hashes cover101 authored programs, including11 rejected
drafts (empty destinations/oversized debug message). Those originals remain in
place, locally ignored; corrected -valid controls are separate. Approval review
rejected moving/deleting originals; the safe correction did neither. Original
sources and observations are immutable.

A later audit found that initial reversal-projectile* and reversal-proj-tail*
fixtures still invoked HitDef: the author searched for label701, while a valid
label700 occurred inside state701. Those captures only establish ordinary
character behavior. Their projectile-specific conclusions were withdrawn. The
first full race run was stopped and retained as an interrupted diagnostic, not
acceptance. Ten newly named reversal-ps-* controls use genuine Projectile
controllers, save observations before Pause/clipboard, and establish the beta
exclusion above. The parser-level controller assertion prevents this mistake
from recurring. Earlier seven render probes publish clipboard before later
VarSets; render_only metadata keeps their84 readings distinct from final-variable
assertions. Their images remain evidence for their actual ordinary behavior.

The87-program corrected development replay passed60.057s; the subsequent ten
projectile variants plus controller-kind assertion pass8.223s. Prior core races
passed Character11.513s, engine1.451s, evaluator1.021s, System9.663s. All seven corrected actual apps match native exactly (three surviving
projectiles and four sparks), with797 frozen application hashes. The final full scoped integration passes against the corrected sources. Source routes
are120/127 (94.5%), seven mode/stage/name families unbound; no whole-engine
percentage is claimed. Mutual/multiple-contact and multi-team arbitration, foreign/resource/
size/zoom extremes, other HitDef-derived reversal fields and broad effects remain explicit variant gates. Items3–4
remain unallocated.

The second full attempt found an interrupted CGO build tree as an extra package.
It was stopped and preserved, not counted as acceptance. Build trees are now
kept under underscore-prefixed scratch directories; final full3 preflights all36
intended packages. No authored source or observed native input was moved.

Final full scoped race suite PASSED:3,169 test/subtest results,24 tested and12
compiled-only packages (36 full package identities), no failures or individual
skips. Match1228.212s; total1298.980s. Command:
`bash game/mugen/mugen.sh --check -race -p 1 -count=1 -timeout=30m -json`.
All10,831 frozen source/test/probe/fixture/module hashes pass; stderr is empty.
Session36806 was awaited and a separate process check found no clients. Only
full3 is acceptance; the two interrupted attempts remain diagnostics. Artifacts:
temp/m4ch3-final-inputs.json, m4ch3-full-scoped-race{.jsonl,.stderr.log,-result.json}
and m4ch3-full.log. Final native/stock/provenance checks also pass.

All final gates pass. Fresh genuine projectile and combined-spark captures
reproduce all original numbers and exact 7,935-pixel masks. Stock captures show
win/rematch/win at 21/26/45 seconds, visually inspected. All 82 owned key events
balance; private PCM, source/config/binary/PNG/input/audio provenance, all 10,831
full-suite hashes, 797 application hashes and root module guards pass. Native
prepared definitions/config/commands/eight resources pass all 101 authored runs.
Sessions 36806 and 12654 were awaited; a separate check finds no clients.
Final audit: temp/m4ch3-final-evidence-audit.json; post logs/results begin
m4ch3-post; final stock app is app-runs/m4ch2-stock-input. No originals, numerical
goldens or tolerances changed. No binaries or changes outside game/mugen/ are
committed. The allocated items 1–2 are complete. Stop before items 3–4.
