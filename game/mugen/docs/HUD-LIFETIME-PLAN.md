M4dq/M4dr now record the bounded KO controls and implementation in

> Historical research plan. Follow [WORKFLOW.md](WORKFLOW.md) and the single
> [project checklist](PROGRESS.md); do not restart the matrices described below.
[HUD-TIMING.md](HUD-TIMING.md). Continue [phase-specific studies](HUD-PHASE-PLAN.md).
The original authoring plan below remains for provenance, not additional coverage.

# Next: timed fight-screen components

This is an experiment plan, not new native coverage or an implemented schema.
Finish the active M4dp integration and separate frozen-input/client guard before
creating or running these fixtures. Keep every native-used recorder unchanged.

## Current evidence and gap

Native held Round/Fight/KO components expire while Go keeps drawing them. M4dp's
positive fight-call placement control isolates the existing 36,030-pixel FIGHT
overlay difference after actor positions are correct. `FightHUD.Update` already
advances cue time during CNS Pause; the missing behavior is not simply an absent
clock. Drawing currently ignores component displaytime and start delays.

Update also returns early for `SimulationSkipped`, which includes KO slowdown.
Whether native HUD clocks continue on those skipped CNS frames needs a positive
control before changing that guard. Match-Finished and round-initialization skips
must remain distinct from KO slowdown. Do not infer these rules from the manual.

## Deterministic observer

Use a NEW separately named recorder derived from the calibrated seek recorder.
Preserve fixture-authored numeric clipboard output. After warmup, debug-Pause and
seek exactly to GameTime399, then step/capture400 onward. An authored P2 self-KO
at400 starts the timed component after the recorder is paused, removing wall-clock
capture uncertainty. The active-pause controls already establish that P2 completes
its issuing-frame CNS even when P1 requests Pause on that frame.

Use invisible actors and complete nonempty inert death states; retain the earlier
empty-state rejection finding. Set KO slowdown0 and a long over-wait for the base
controls. Show actual GameTime/RoundState/P2Life/StateNo in the ordinary observer;
paused/slow controls may retain the previous clipboard while engine frames step.
Preserve that distinction and verify the later resumed counter boundary.

Add a FULL `.fight.def` sidecar to the new recorder, copied only into the selected
`data/mugen1/fight.def`. The supplied config's motif is exactly
`data/mugen1/system.def`; verify that selection and the motif's fight path. Pin the
canonical fight definition, replacement bytes, prepared bytes, all original font/
SFF/SND files, config finalization and seed. The existing sprite200,0 is a positive
KO resource. No new binary asset is needed. Preserve the normal startup layout so
GameTime399 is active. Remove/replace existing KO attributes when constructing
its section; do not depend on duplicate-attribute interpretation.

## Proposed distinctions

- Sprite: omitted displaytime, zero, positive3 and negative1.
- Font: omitted, positive3 and negative1, using an existing stock font.
- Animation: finite two-frame default, explicit7, zero and negative1; explicit
  LoopStart; a final infinite element. The omitted marker sentinel is -1.
- Delay: positive KO.time with a finite display duration; capture both sides of
  the start and expiry boundaries.
- Clocks: the same finite animation during Pause, SuperPause and KO slowdown,
  with positive changes between its two frames to distinguish advancement from
  merely retaining a persistent sprite.

Use distinct frame offsets if the stock resource lacks a second suitable image.
Measure native first-pass duration versus repeated-loop duration rather than
assuming they are interchangeable. The parser/clock currently exposes elapsed
and AnimTime; its private total/loopTime fields do not themselves establish the
fight-screen default expiry rule.

## Consumer and acceptance bounds

Review every native-only frame, numeric row, full sidecar/controller intent,
resource and event before promotion or Go edits. Derive exact timing tests and
strict image regions from that evidence. Ordinary static bars/portraits/fonts
must remain separate from timed Round components; a default1 lifetime cannot be
applied blindly to continuously drawn static sprites.

Implement measured component start/expiry, animation clocks and timed text in the
production HUD. Keep audio cue timing and KO skipped-frame behavior under their
own observed controls. Recheck the original held overlays, M4dp placement worlds,
older renderer controls and stock input/private audio. Preserve all strict raster
and ordinary KO-echo failures; no tolerance or golden changes. Broader theme/font/
mode assets remain item4 unless their allocation is explicitly changed.
