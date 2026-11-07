# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/;
requested upstream merges can bring in changes elsewhere. Local commits only,
no pushes/binaries/assets/generated banks. Runtime runs stay serial through the
private wrapper; await exit, guard, then edit/run. Reuse assets/cache and remove
disposable output after inspection.

Current integration includes origin/go-rewrite at172d3fb02 and the previously
pushed go-rewrite-mugen dependency update dc3423cf1. Both histories are retained.
Module conflicts are resolved with Go1.26/Ebitengine2.10.6/purego0.11.3 and
MUGEN's YAML/MP3/Vorbis dependencies retained. Root and scoped module graphs and
checksums are synchronized. Normal builds keep root metadata read-only; the
ignored post-merge SHA256 baseline records the intentional incoming update.
For private-wrapper commands use env GOTOOLCHAIN=go1.26.9 (already cached here).

All five MUGEN connected segments and incoming Paintown/parser/simulator tests
pass with race detection. Both applications build using the scoped overlay AND
resolved readonly root module. One owned-window stock KFM/KFM720 startup capture
was inspected and removed along with its overlay/log. No asset banks copied.

The original untracked root go.sum is preserved byte-for-byte at
`temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked`.
Other pre-existing untracked work remains untouched. The earlier pushed
dependency update is now reconciled; the combined graph retains its newer
Ebitengine2.10.6/purego0.11.3 versions plus upstream Paintown and MUGEN needs.

Latest implementation checkpoint3a45b0577 fixes immediate-parent background
links, animated parallax, width vs legacy xscale, infinite tiling, masks/blends,
BG disable/hide/action clocks and controller grouping through round reset.
Previous checkpoints cover mixed-team life/round budgets, Survival pre5900
carry/counters/RNG, Turns order, co-op input and Watch/Arcade menus.

The ledger remains32/34 implemented rows (94% of checklist items, not native
conformance or effort). Remaining: exact raster/zoom/crop/color/shadow/alpha,
KO echo, native AI policy, camera/push ordering, strict content compatibility,
physical CD playback and absent licensed fonts. Native pixel-exact parallax
and camera matching remain unproven. Linux is the target; other platforms are
deferred. Preserve user content and unrelated root/untracked work, including
initial_placement_render_probe.py. Do not regenerate historical matrices.
