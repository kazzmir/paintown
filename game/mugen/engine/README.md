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
and groups are ORed. Triggerall runs first. Reference probes show that persistent
N fires immediately, then suppresses the next N-1 eligible passes before testing
triggers again. A used one-shot skips trigger evaluation. Hosts clear positive
state counters on re-entry. The older success-count interpretation was incorrect.

RunStatePass is shared by Character and the headless tests. It executes -3/-2/-1
in order, then the positive transition chain. A transition aborts its controller
block while later negative levels still run, as observed in the reference.

The former ten-state cap is replaced by the beta's measured 2500-loop guard.
Its post-increment greater-than check allows 2502 positive state executions if
the last terminates; a further continuation fails and names the pending state.
Fifteen native programs cover self/alternating chains, terminal boundaries and
negative-state launches. Headless and production Character tests agree. M4t adds
an explicit StatePassBudget shared by CNS and engine/physics resumes in a tick.
It counts CNS transitions; negative/engine entries and terminal passes do not
consume it. Native landing budget/overflow and 2510 repeated engine entries back
that distinction. World/pause/ownership scheduling remains incomplete.

State entry now has a single explicit EnterState path. Entry expressions compile
once and evaluate on entry, with omission/U semantics and combat persistence.
Executing controllers at Time=0 does not reapply entry attributes. Native probes
show that transition ctrl/anim values apply before entry expressions, then
explicit Statedef ctrl/anim take precedence. The earlier override-wins assumption
was incorrect. Omitted entry values preserve the incoming transition values.

Core controller parameters compile to typed expressions with explicit omission
and integer conversion rules. Complex controllers still use legacy handlers.

Current limitations: many controller bodies remain incomplete, and the production Character still needs its
full world/ownership/tick extraction and reference timing comparisons.
World-level simulation and complete conformance are subsequent work. See the
checkpoint ledger and compatibility inventory; this extraction does not claim
full state-machine coverage.

RunPositivePass is also used after engine/contact transitions without repeating
negative states. Production root-player contact ordering now lives in
character.TickCharacters and has focused native trade probes; that adapter still
contains Ebitengine-coupled Character/assets and is not the complete world core.
