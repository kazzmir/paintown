# Headless execution core

The state executor now lives here. `character` retains public compatibility
aliases so the production Character and the headless tests use the same code.
This package must not import Ebitengine or the input/render/audio adapters.

State definitions and controller sources are immutable after compilation. A
StateMachine caches immutable trigger programs with synchronized compilation;
each host owns its variables, persistence counters, and other runtime state.
Compiled programs may be used by distinct hosts concurrently.

Trigger compilation fails before state side effects on unknown controller types,
malformed expressions, invalid universal literals, or absent trigger1. Numbered
groups run in numeric order through the first gap; repeated conditions are ANDed
and groups are ORed. Triggerall runs first. Persistence counts successful trigger
evaluations, while hosts clear positive-state counters on state re-entry.

State entry now has a single explicit EnterState path. Entry expressions compile
once and evaluate on entry, with omission/U semantics and combat persistence.
Executing controllers at Time=0 does not reapply entry attributes.

Core controller parameters compile to typed expressions with explicit omission
and integer conversion rules. Complex controllers still use legacy handlers.

Current limitations: many controller bodies remain incomplete, and the production Character still needs its
full world/ownership/tick extraction and reference timing comparisons.
World-level simulation and complete conformance are subsequent work. See the
checkpoint ledger and compatibility inventory; this extraction does not claim
full state-machine coverage.
