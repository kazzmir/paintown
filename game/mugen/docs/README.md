# MUGEN Go port review documents

Current status: implementation approved. See [checkpoint history](CHECKPOINTS.md)
and the handoff for current progress; the assessment describes the original baseline.

1. [Assessment](ASSESSMENT.md): source-confirmed gaps, baseline tests, and comparison with the C++ engine and bundled M.U.G.E.N 1.1 beta 1.
2. [Implementation plan](PLAN.md): architecture, scope, milestones, dependencies, and acceptance gates.
3. [Compatibility inventory](COMPATIBILITY.md): all 89 documented controllers, 127 triggers, and the stock asset census.
4. [Handoff](HANDOFF.md): authorization, completed work, environment, and next action.
5. [Build and fonts](BUILD.md): scoped dependency metadata and portable font lookup.
6. [Reference evidence](REFERENCE.md): executable probes and pixel/timing comparisons.

7. [Match runtime](MATCH.md): production flow, controls, tests and current fight limitations.

8. [Sound runtime](AUDIO.md): native PCM evidence, command/mixer behavior and remaining audio work.

9. [Contact effects](EFFECTS.md): spark geometry, resource/clock/render wiring and viewport findings.

10. [Pause scheduling](PAUSE.md) and [SuperPause](SUPERPAUSE.md): shared clocks, native contacts, defence, cues and remaining ownership limits.

11. [Entity identity](ENTITIES.md) and [bottom boundaries](BOTTOM.md): native IDs, helper families, runtime redirection and unresolved VM error behavior.

12. [Targets and redirection](TARGETS.md): acquisition, typed target effects, caller units and conditional scope.

13. [Custom state and animation ownership](OWNERSHIP.md): native external entry, negative eligibility, borrowed coordinates/AIR and remaining throw integration.

14. [Root world phases](WORLD.md): native per-root CNS/damage/physics/clocks, shared late push and remaining entity scheduling.

15. [Target binding](BINDING.md): native constraint timing/lifetime, units, cleanup, parameter grammar and hurtbox push gating.

16. [HitDef custom states](CUSTOMHIT.md): contact transitions, override exclusions, binding flags and infinite AIR clock evidence.

17. [Stock throws](THROWS.md): original CNS/AIR composition, grab timing, contact poses and fall-velocity omission.

18. [Explod state and rendering](EXPLODS.md): native lifecycle/counts, common resources, calibrated placement and remaining effect coverage.

19. [Coordinate spaces](COORDINATES.md): profiles, viewport fitting, fixed-camera script positions and remaining camera/rendering gaps.

20. [Horizontal camera](CAMERA.md): native tension, retained goals, easing, speed floors and root update points.

21. [Screen boundaries](SCREENBOUND.md): native root/helper defaults, stage margins, typed ScreenBound and camera participation.

22. [Width and edge queries](WIDTH.md): native temporary offsets, base body distance, ground push and edge-trigger semantics.

23. [Helper configuration](HELPERS.md): native birth/order, typed parameters, sizes, pause allowances, camera precision and intro timing.

Project instructions: [AGENTS.md](../AGENTS.md). All project changes for this
effort stay under `game/mugen/`.
