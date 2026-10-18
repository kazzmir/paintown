# HitFallSet in 1.1 beta 1

M4bl compares the supplied executable with 22 original text programs. The
manual's velocity description differs from the executable. This checkpoint
matches the beta and records the discrepancy rather than enabling behavior
that the target ignores.

## Measured behavior

- Omitted `value` and explicit `-1` preserve the current fall value.
- Other int32 values are stored exactly: `0` clears, `1` enables, and `2`/`-2`
  remain visible through `GetHitVar(fall)`. Boolean consumers use nonzero.
- Floating literals for `value` fail loading; runtime floats truncate toward
  zero. Bottom becomes zero. A two-component value fails loading. Unknown
  value triggers fail compilation with file/state/controller/parameter context.
- The fall clock is retained by clear, enable and preserve operations.
  `GetHitVar(fall)` remains observable after `StateTypeSet` changes MoveType
  to I. The controller still changes that stored value in this context.
- The documented `xvel`/`yvel` fields are ignored: no velocity mutation, no
  expression side effects and no load failure for tuples or unknown triggers.
  `xvelocity`/`yvelocity` spelling controls also leave velocities unchanged.
  Both declaration orders, an omitted-X sentinel and a 1280-wide profile agree.
- `HitFallVel` consumes the retained velocities even after fall is cleared or
  MoveType becomes I in these controls. It preserves X when X was omitted.
  These existing consumers are retained; the manual's falling-state wording
  does not describe these measured restrictions accurately.

The Go controller now precompiles the supported value before any state effects,
keeps omitted/default handling out of the shared source map, preserves raw
integer flags and leaves ignored fields unevaluated. Contact still normalizes
HitDef fall selection into a boolean. No HitDef expression count is added:
53/81 (65.4%) remains the current routing metric.

## Evidence and limits

`tests/testdata/reference/fallset-*` contains 22 CNS/AIR/CMD/stage/JSON sets:
20 accepted programs, 240 numeric observations, two native load rejections and
88 source hashes. Original inputs were frozen before native execution in
`temp/m4bl-prepared-inputs.json` and `temp/m4bl-prepared-extra.json`. The two
additional controls distinguish alternate key spellings and unknown ignored
expressions. The initial runner expected velocity tuples to fail; the native
executable accepted them. This changed the acceptance classification, not the
frozen input or native observation. Do not rerun the author script or derive
native expectations from Go.

`TestNativeHitFallSetPrograms` checks the exact corpus count, input hashes,
native load errors and recorded float32 observations in the production Match
runtime. Core tests check shared-source reuse and rejection before earlier
controller effects. Focused races pass (Match 17.114s, Character 1.041s);
core engine/evaluator/CNS/combat races pass 1.435s/1.021s/1.012s/1.011s.
The broader initial replay matched all numeric observations but two rejection
checks named the wrong state in metadata (700 rather than native state 5000).
That metadata is corrected; native inputs and observations are unchanged.
Two fresh native reproductions and two apps/three captures pass source,
config, binary, PNG and runtime provenance checks. The stock 7s image was
visually inspected. Logs/audit are temp/m4bl-validation.log and
temp/m4bl-evidence-audit.json. The full scoped race run is deliberately deferred
to M4bm, the second checkpoint's integration boundary.

This is bounded own-state/root evidence. Foreign-state coordinate execution,
helpers/projectiles/teams, repeated contacts and broad Pause/SuperPause
combinations remain open. M4bm adds bounded [downed-hit bounce](DOWN-BOUNCE.md) coverage and corrects
contact initialization for an idle HitOverride: it has no fall flag, whereas
StateTypeSet changing an existing hit state to I retains the stored flag.
The earlier native hitkill-fall-override-idle control pins that distinction. Full
recovery-command routing, liedown/mashing clocks, juggle/chains and absolute
round clocks remain separate obligations. Existing six alpha and two spark raster failures remain unwaived.
