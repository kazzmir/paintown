# MUGEN Go port review documents

Start with the [project README](../README.md) for requirements, building,
running, controls and dated coverage percentages with their exact scopes.

Current work is the remaining effects/controllers allocation in
[ITEM-3-PLAN.md](ITEM-3-PLAN.md). AfterImage/AfterImageTime has completed its bounded native, image and integration
checkpoint; see [AFTERIMAGE.md](AFTERIMAGE.md). The earlier 27-key
HitDef allocation is complete. Broader modes, assets and final platform
conformance remain a separate allocation.

Read the [coverage ledger](COVERAGE.md), [compatibility inventory](COMPATIBILITY.md)
and [current handoff](HANDOFF.md) for measured behavior, active validation and
remaining work. The [checkpoint history](CHECKPOINTS.md) and topic notes preserve
verification and limits. The original assessment documents the starting baseline.

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

24. [Helper retirement](RETIREMENT.md): recursive removal, retained parent snapshots, orphan effects and lifetime/binding cleanup.

25. [Helper binding phases](HELPER-BINDING.md): anchor completion, own/redirected flags, forced retirement fall and delayed victim facing.

26. [Helper combat](HELPER-COMBAT.md): defender candidates, birth activation, independent target identities and state-phase controls.

27. [HitDef selection](HIT-SELECTION.md): one-target consumption, throw defaults, friendly teams and root/helper contact order.

28. [Actor sprite order](ACTOR-ORDER.md): native priority/ties, global root/helper drawing and original RGB pixel calibration.

29. [Sprite alignment](SPRITE-ALIGNMENT.md): native facing/AIR offsets, effective flips and pixel phase across render widths and helper sizes.

30. [Combat priority](COMBAT-PRIORITY.md): reciprocal clashes, hit counters and interrupted target release.

31. [Contact drawing priority](CONTACT-DRAW.md): HitDef sprite priorities and destination entry.

32. [Hit flags](HIT-FLAGS.md): posture, falling eligibility, downed selection and fall continuation.

33. [Forced standing and velocity](FORCED-STANDING.md): posture forcing, compiled ground velocity and world/facing conversion.

34. [Contact facing](CONTACT-FACING.md): selected knockback direction, deferred physical turns and shared entry phases.

35. [HitDef completion audit](HITDEF-SCHEMA.md): remaining typed parameter groups and ignored runtime fields.

36. [Hit velocity defaults](HIT-VELOCITY.md): repeated inherited expressions, profile-aware vectors and consumed down-hit Y offsets.

37. [Physical corner push](CORNER-PUSH.md): common-pixel proximity, independent world impulses, precision and lifetime.

38. [Fractional sprite sampling](SPRITE-SAMPLING.md): original RGB/indexed patterns, pixel centers, clamped filtering and exact raster controls.

39. [Lethality](HIT-LETHALITY.md), [diagnostic stage fill](STAGE-CLEAR.md) and
    [fatal guard cues](FATAL-GUARD-CUES.md): recent behavior/evidence checkpoints
    and unwaived production raster failures.

40. [HitDef pause expressions](HIT-PAUSE.md): typed hit/guard pause pairs,
    native component/default/evaluation order and remaining timing obligations.

41. [Storyboard canvas](STORYBOARD-CANVAS.md): storyboard/game/window fitting and native placement controls.

42. [Ground/guard timing](HIT-GROUND-TIMING.md): expression defaults, coercion and native evaluation order.

43. [Air/down timing](HIT-AIR-DOWN-TIMING.md): launch/down selection, omission versus zero and bounded countdown controls.

Project instructions: [AGENTS.md](../AGENTS.md). All project changes for this
effort stay under `game/mugen/`.

- [HitDef literal schemas and reactions](HIT-LITERALS.md)

- [HitDef chains and juggle costs](HIT-CHAINS-JUGGLE.md)

- [HitDef distance/snap constraints](HIT-CONSTRAINTS.md)

- [HitDef palette effects](HIT-PALETTE.md)

- [AfterImage actor contexts](AFTERIMAGE-ACTORS.md): shared capacity, expression
  admission, lifetime, visibility, blank poses and preserved native provenance.

- [Shared shadows](SHADOWS.md): stage geometry/fade/flags, current-pose consumers,
  Explod creation schema and exact native/app results with unwaived pixel limits.

- [ForceFeedback target boundary](FORCE-FEEDBACK.md): native Null operation,
  ignored parameters, language profiles and no host/device dispatch.


- [Explod creation drawing](EXPLOD-CREATION-DRAWING.md): typed angles and blend
  defaults, AIR overrides, conditional evaluation and strict renderer limits.


- [Explod capacity and random displacement](EXPLOD-CAPACITY-RANDOM.md): global
  pool admission, exact RNG phases, reproducible matches and full acceptance.

- [ModifyExplod transparency tokens](MODIFY-EXPLOD-ENUMS.md): native quoted modes, skipped alpha compilation/evaluation and exact application controls.

- [AssertSpecial global visibility](ASSERT-SPECIAL.md): literal schemas, global pause/reset consumers, 40 exact world regions and 35 open strict HUD gates.

[Guard, walk and missing-state controls](ASSERT-SPECIAL-CONTROLS.md) records the
M4di keyboard/contact comparisons, corrected common-file inference and full
4,870-result integration checkpoint.

[NoKO and retained death state](NO-KO.md): entity-order/global assertions, frozen
owners, self-KO/corpse contacts and full4,918-result integration acceptance.

[NoKO voice and channel behavior](NO-KO-AUDIO.md): ten native PCM programs,
committed-death voice/echo gating and one unwaived production echo comparison.


[GameMakeAnim and MakeDust](SYSTEM-EFFECTS.md): deprecated common-resource scene
cues, native schema/RNG/layer/clock evidence,70/72 exact world observers, the
private-snapshot test-selection correction, and remaining system-effect studies.
