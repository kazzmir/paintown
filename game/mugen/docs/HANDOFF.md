# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Present the complete change list and obtain approval BEFORE each local commit.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Last reviewed checkpoint 9645e9b27 commits horizontal tension/zoom fitting and
zoom-dependent camera bounds, sharing fixed stage extents with projectiles.

Latest batch fixes camera startup and stage rendering. Integer startx/starty
initialize the visible camera in raw game units; disabled participants retain
it while follow endpoints remain stage-scaled. Modern backgrounds/foregrounds
project directly to the common game raster and use the existing triangle
sampler. Pre-1.0/date/omitted stage versions truncate only BG camera values to
stage units. CNS queries, fighters and effects retain fractional coordinates.

One cropped/zoomed HD original-beta scene establishes startup, CameraPos/edges
and exact BG/fighter/effect bounds for modern/legacy declarations. Values live
in the existing connected scenario and feature catalog. Acceptance continues
through foregrounds, projected mask windows and KO/round reset, using mixed
320p/1280p actors. In-memory marker substitutions must also replace palette/AIR
bindings and disable BOTH roots before the first update. Four changed-source
segments pass with race detection; state-flow remains checked without rerunning.
Native helpers, small generated markers, overlays, captures and logs were removed.
Linux rebuilt; a stock KFM/KFM720 window was inspected and its scratch removed.

Configuration checkpoint 8eda46f84 untracks/ignores local mugen.yaml, preserving
its contents. Existing clones need a backup before pulling; the user will notify
the contributor. README retains the tested scalar/list keyboard override example.
Both upstream histories remain integrated with Go 1.26/Ebitengine 2.10.6/
purego 0.11.3. Root module files stay read-only; the original go.sum backup stays
at temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked.

Ledger remains 32/34 checklist items, not native conformance or effort. Remaining:
intermediate camera easing/push ordering, other crop/raster/color/shadow/alpha
edges, KO echo, AI policy, strict asset compatibility, physical CD and absent
licensed fonts. Linux target; other platforms deferred. No historical matrices.
