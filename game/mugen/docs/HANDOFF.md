# Current handoff

Use PROGRESS.md as the single ledger; follow AGENTS.md. Author only game/mugen/;
local source/text commits, no pushes/binaries/assets/generated banks. Runtime
runs stay serial through the private host wrapper; await exit then guard before
editing. Reuse canonical assets/shared cache and delete run scratch on all exits.

Current batch adds documented1.1 startzoom/zoomout/zoomin, camera widening and
return, BG zoomdelta, world sprite/helper/projectile/trail/shadow and effect
projection, screen-bound/helper/Explod/projectile edges, mixed-coordinate queries,
and crop-aware floor anchoring. HUD and explicit screen-space effects stay fixed.
Unchanged zoom-one camera behavior retains the previous native control path.
Stage reflection intensity now draws colored floor reflections with actor palette,
visibility/shadow suppression, and zoom. Both intensity and documented reflect
flag are parsed. Native zoom easing and reflection blend precision are unverified.

One connected stage scenario spans participation/startzoom, separation/return,
CNS queries, constraints, BG/world/screen effects/HUD pixels, colored reflections/
suppression and actual KO→reset. Its initial arrangement was corrected to spread
fighters after fight-entry position reset and check opposite screen boundaries
without body-push overlap. All changed-source segments pass with race detection;
Linux binary rebuilt. No native captures or generated asset bank is retained.

Earlier local checkpoints calibrate Turns2-vs-4 and single-player Survival
recovery (400→798, timed400→744, weighted100/500→275, cap), and fix Survival's
single-opponent bout plus human Turns reserve position/life/power across bouts.

Next functional stage gaps: zoffsetlink moving floor (the user's cargo.def uses
it), stage autoturn, BG normal/animation scalestart/scaledelta and windowdelta.
Stage xscale/yscale are also unconsumed. Keep these visible in the asset partial.
Other remaining work: native color/shadow/alpha/KO-echo/AI policy precision,
mixed-mode life balance, Survival team recovery/dead Simul partner carry, strict
invalid-asset rejection. Physical CD/absent licensed fonts remain limitations.
The palette/legacy ACT and user's stage/tracker-music work remain intact.
