# HitDef contact drawing priority

M4as isolates the bound-release foreground mismatch discovered by M4ar. The
reference's `docs/sctrls.html` describes p1sprpriority/p2sprpriority as contact
drawing priorities, including guarding. Twenty-three original `hitdraw-*`
programs use the existing four-color marker SFF (no reference art) to identify
the visible entity independently of Go's actor sorting. Each program pins CNS,
AIR, CMD, stage and original marker-bank hashes. Accepted runs pin numeric
state/identity/life/target packets and the native foreground RGB/ID/rectangle.

## Measured behavior

Contact sets the attacker's priority to 1 and the receiver's to 0 by default.
Independent explicit ties bracket those exact integers; initial priorities -2
and 5 prevent an unchanged value from looking like the default. Explicit fields
can put the receiver in front. Contact applies the fields on normal hits,
guards and HitOverride, even when override entry to idle suppresses damage.
Helpers use their own priorities; helper attacks promote the helper above roots.

Assignment precedes get-hit, custom victim and custom attacker state entry.
A destination Statedef's explicit sprpriority can therefore replace a contact
value. A no-contact Dodge/Dodge tie changes neither actor's priority, despite
both HitDefs containing explicit drawing fields.

The measured -99/-98 and 98/99 pairs retain their ordering; do not clamp these
fields to the documented ordinary drawing range. Literal floats reject loading;
computed floats truncate. Bottom becomes zero while retaining a live attack.
The two field defaults remain distinct from an explicit zero.

`engine/hitdef.go` compiles both expressions and preserves the source map.
`cns.HitDef` stores optional values. `prepareContact` assigns defaults/overrides
before get-hit and custom-state entry. The existing global actor sorter and
sprite projection consume those priorities; no renderer offset is added.

## Validation

The corpus contains 23 programs, 252 numeric observations, two native load
rejections, 21 native foreground identities and 115 authored input hashes.
The pre-fix run in `temp/m4as-before-draw.log` fails 14 programs with ignored-
field/parser differences. All 23 corrected focused cases pass in
`temp/m4as-fields-draw.log`. The full scoped race suite passes in
`temp/m4as-full-race.log` (Match 247.463 seconds). Native source
captures are in `temp/m4as-native-first.log`. Pure races pass in
`temp/m4as-first-pure-race.log`.

Four fresh native replays pass: helper override, get-hit entry, computed P2 field
and literal-float rejection. The extended Go marker checker also passes the
previously verified M4aq reverse-helper capture and rejects Go record mode.
Twenty-two final 1280x720 app captures from one identical binary pass source/
binary/config/PNG/dimension/runtime/resource checks. Six accepted marker scenes
match exact native RGB/ID/count/bounds, including helper override and entry
precedence. All four real-art clothing masks match exactly: 11,181/11,181/19,515/
12,501 white pixels. The bound-release gap falls from 6,099 to zero with no
coordinate/sampling change. Stock throw recovery retains two fighters without
persistent dust clones. Evidence: `temp/m4as-app-checks.json` and
`temp/m4as-app-checks.log`. Native debug backgrounds/HUD and complete framebuffer
parity remain outside these actor-only comparisons.

`tests/actor_render_probe.py` now accepts hitdraw fixtures and can verify actual
Go app captures with `--go-run`. It checks original input hashes, prepared CNS/
CMD, bank, AIR, stage, profile, binary/config/PNG hashes and runtime/resource
errors, then requires exact native foreground color/ID/count/bounds. Go checking
cannot record or overwrite native goldens. Ordinary actororder fixtures retain
their existing native path.

Full projectile, effect, paused/multiple/foreign contact combinations, remaining
HitDef schema and complete rendering/asset/mode compatibility remain open.
Reversal: 8cae270d (M4ar).
