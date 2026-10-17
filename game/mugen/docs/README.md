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

Project instructions: [AGENTS.md](../AGENTS.md). All project changes for this
effort stay under `game/mugen/`.
