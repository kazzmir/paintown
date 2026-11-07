# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/;
local commits, no pushes/binaries/assets/generated banks. Use the private
wrapper with env GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard,
then edit/run. Reuse assets/cache; remove disposable output.

Latest batch implements the 1.1 vertical camera selected by BOTH tensionhigh
and tensionlow. The stage loader retains explicit zero vs absent parameters.
The pair overrides verticalfollow/floortension. Highest-player visibility wins
when margins conflict; the goal returns toward the stage baseline. Vertical
zoom fits the eligible actor span PLUS both unzoomed stage margins, respecting
zoomout/zoomin. Internal camera coordinates remain in world space; CameraPos Y
publishes post-zoom translation, with static crop outside that scale.

Bounded original-beta jump/return flows establish high80/low40 cameras
0/-80/-80/0/-180/0 and stage bounds. With high70/low30, y=-200/0 yields zoom.8,
CameraPosY-16 and ScreenPosY70; both y=-200 return zoom1/camera-70. Actor axes,
not sprite head offsets, drive those observations. No parameter matrices or
asset copies were made. Native setup also confirmed required BG sprite and
command declarations; broader strict content validation remains open.

The existing connected camera scenario loads a small stage DEF, runs these
poses with 320p/1280p actors, checks CNS viewport identities, pause, movecamera
participation, legacy follow fallback and round reset. Begin scripted jumps
AFTER Active: fight-start placement otherwise resets their initial positions.
Changed-source segments pass with race detection. Linux rebuilt; root module
hashes unchanged. Native helpers/overlays/captures/logs and segment scratch
were inspected/removed. The user's local mugen.yaml edit is preserved and
excluded from the checkpoint.

Prior work covers alternative YAML keyboard keys (user tested), linked/animated
backgrounds, teams/recovery, Survival carry/counters/RNG and co-op/Watch menus.
Both upstream histories remain integrated, with Go1.26/Ebitengine2.10.6/
purego0.11.3 synchronized in root/scoped metadata. Root files stay read-only.
The original untracked go.sum backup remains at
`temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked`.

Ledger remains32/34 checklist items, not native conformance or effort. Remaining:
horizontal zoom fitting, legacy integer-stage camera publication, easing/crop/
raster/color/shadow/alpha, KO echo, AI policy, intermediate camera/push ordering,
strict asset compatibility, physical CD and absent licensed fonts. Linux target;
other platforms deferred. Preserve unrelated work; no historical matrices.
