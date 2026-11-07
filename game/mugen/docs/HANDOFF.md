# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Present each checkpoint's complete change list and obtain user approval BEFORE
committing. No pushes, binaries, assets or generated banks. Use the private
wrapper with env GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard,
then edit/run. Reuse assets/cache; remove disposable output.

Latest batch fixes horizontal zoom: fit actor span PLUS both unzoomed tension
margins. Camera bounds describe the fully zoomed-out view; narrower views can
travel farther within fixed stage edges. Projectile stage bounds share those
extents. Bounded beta observations and the connected camera scenario cover
fitting, tension return, zoom limits and stage edges. Native values are retained
in the scenario and feature catalog, without generated captures or fixture banks.

Legacy CameraPos queries remain fractional, including on a 640x480 stage with
omitted mugenversion. Do not quantize CNS queries to implement the documented
whole-stage-unit rendering rule; exact legacy raster rounding remains open.

The existing connected scenario also retains vertical edge tension, height-based
zoom and post-zoom CameraPos Y, real stage loading, mixed 320p/1280p actors,
participation, pause and reset. Begin scripted poses AFTER Active: fight-start
placement resets positions. All four changed-source segments pass with race
detection; state-flow remains complete without rerunning. Native helpers,
captures, logs and symlink overlays were inspected/removed.

Configuration checkpoint 8eda46f84 untracks/ignores game/mugen/mugen.yaml while
preserving the local file. Existing clones must back up their tracked YAML before
pulling and restore it afterward; the user will notify the contributor. README
retains the scalar/list keyboard override example, already user tested.

Both upstream histories remain integrated, with Go 1.26/Ebitengine 2.10.6/
purego 0.11.3 synchronized in root/scoped metadata. Root files stay read-only.
Original untracked go.sum backup:
`temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked`.

Ledger remains 32/34 checklist items, not native conformance or effort. Remaining:
camera easing/crop/legacy raster, color/shadow/alpha, KO echo, AI policy,
intermediate camera/push ordering, strict asset compatibility, physical CD and
absent licensed fonts. Linux target; other platforms deferred. Preserve
unrelated work and local YAML; no historical matrices.
