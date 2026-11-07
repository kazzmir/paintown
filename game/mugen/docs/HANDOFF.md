# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/;
local commits, no pushes/binaries/assets/generated banks. Use the private
wrapper with env GOTOOLCHAIN=go1.26.9. Runtime runs stay serial: await exit,
guard, then edit/run. Reuse assets/cache; remove disposable output.

Latest checkpoint adds alternative keyboard keys to mugen.yaml. Each action
accepts a scalar or YAML list; either key activates it in menus/gameplay.
Lists replace that action, omitted actions inherit, none/disabled/[] disable.
Duplicate key aliases are removed. An unknown key rejects that entire binding
with a warning while keeping its inherited value. Malformed override files
now report their load error before the existing fallback search continues.
Startup output lists all configured alternatives. README and the tracked
mugen.yaml template document syntax, supported actions and lookup order.

Keys retains its primary-key API and legacy scalar fields. Immutable alternate
slices and replacement setters preserve value-copy isolation; Bindings returns
an independent snapshot. Runtime matching avoids snapshot allocation, polls
valid physical alternatives once per query until a match, and preserves
explicit KeyA(0), disabled bindings and player-specific confirmation.

The existing connected title/selection/fight scenario loads a tiny YAML file,
checks ownership/partial release/disabled/invalid overrides, uses alternatives
for menus, movement and stock CNS attacks, then primary input in the next round,
rematches and exits through an alternative Escape. Affected combat/match-flow
segments and existing config/input checks pass with race detection. Linux
rebuilt; root module hashes unchanged and scenario scratch removed.

Both upstream histories are integrated: go-rewrite172d3fb02 and the earlier
MUGEN dependency update dc3423cf1. Go1.26/Ebitengine2.10.6/purego0.11.3 remain
synchronized in root/scoped metadata. Root metadata stays read-only. The original
untracked go.sum backup remains at
`temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked`.

Prior implementations cover linked/animated backgrounds and BG clocks, mixed
teams/recovery, Survival carry/counters/RNG, Turns order and co-op/Watch menus.
Ledger remains32/34 checklist items, not native conformance or effort. Remaining
precision/assets: raster/zoom/crop/color/shadow/alpha, KO echo, AI policy,
camera/push ordering, strict content compatibility, physical CD and absent
licensed fonts. Linux target; other platforms deferred. Preserve user content
and unrelated untracked work; do not regenerate historical matrices.
