# Remaining scope and measurement plan

Use [PROGRESS.md](PROGRESS.md) as the single completion ledger. This plan defines
the assessment and proposed work contracts; it does not maintain another set of
completion checkmarks. No further engine implementation is part of this planning
checkpoint.

## What the current numbers establish

The source report finds 89/89 controller families and 127/127 trigger routes.
The project ledger has 32 implemented categories and three broad partial ones.
These are availability/category counts, not an effort estimate or a percentage
of native compatibility. A registered route can still have a behavioral defect.
The three partial categories mix missing consumers, unverified behavior and
capabilities deliberately deferred from the current Linux work.

The current source review confirms two result-screen consumers still need work.
It also finds explicit functional policies for training distance/button jam and
AI command selection whose native equivalence has not been established. Those
policies need assessment; their existence alone does not prove a defect. A scan
for stubs or unsupported paths cannot establish that these are all remaining
gaps. For example, some rejected syntax/formats are intentional target behavior.

## Scope of the next assessment

Target the Linux implementation of M.U.G.E.N 1.1 beta 1, including documented
state/runtime behavior, complete game flows, supported assets/audio and input.
Older content is a compatibility input, not a promise that every pack is valid.
Keep previously closed discrepancies closed unless new evidence reopens them.

Perform one assessment checkpoint before starting the next implementation batch:

1. Review each area below against target documentation, production consumers and
   existing connected-scenario evidence. Reuse prior findings first.
2. Give each scoped behavior a disposition: implemented with supporting evidence,
   confirmed missing/incorrect, unverified, or explicitly deferred. Do not convert
   an unknown into a code task without a missing consumer or a demonstrated
   mismatch. Do not silently count an unknown as complete either.
3. For each confirmed gap, record its contract, source/reference basis, dependency,
   acceptance condition and rough size/confidence. Group related consumers into
   meaningful behavior changes rather than one task per parameter.
4. Publish a finite implementation backlog and a separate acceptance list in the
   existing feature catalog/ledger. Unresolved questions stay visible with their
   evidence or hardware dependency. Present the map before engine changes.

This checkpoint is a documentation/source assessment, not a new test campaign.
If a behavior cannot be resolved from existing evidence, record the precise
question and the smallest connected reference comparison needed. Do not start
native experiments, fixture generation or exhaustive compatibility sampling as
part of the planning checkpoint. Review each area once; carry unresolved
questions forward instead of expanding the assessment indefinitely.

## Confirmed implementation targets

These two targets belong to the current `raster-audio` partial category. Their
completion remains recorded in the ledger, not in this table.

| ID | Behavior and production boundary | Completion condition | Preliminary size / confidence |
| --- | --- | --- | --- |
| K1 | Win/Survival result text honors `wintext.layerno` / `winstext.layerno`. Currently `system/mode_screens.go` draws text after the completed fight; `system/fight_state.go` owns scene composition. | A completed-match result sequence places authored text at the target-defined layer, preserves fades/display duration and leaves score, round and timer committed. Resolve exact target layer semantics before implementation. | Medium: crosses result presentation and fight composition. High confidence that the consumer is missing; estimate remains provisional. |
| K2 | Survival results honor `roundstowin` when choosing the win/lose pose. `system/mode_screens.go`, `system/modes.go` and `match/result_pose.go` currently continue the completed fight without this threshold consumer. | Survival completion below and at the authored threshold chooses the target pose while preserving the final score, actor lifecycle, result duration and return to title. Resolve state-entry/dead-actor policy before implementation. | Medium: crosses campaign results and actor state entry. High confidence that the consumer is missing; estimate remains provisional. |

## Bounded review areas

These are assessment scopes, not five assumed defects or five equal units of
remaining effort. Each review produces supported dispositions or specific gap
contracts. The relevant three parent categories stay partial until their scoped
requirements and acceptance have been resolved.

| ID | Review boundary and existing sources | Required assessment output / acceptance boundary |
| --- | --- | --- |
| A1 | State/runtime ordering: pause/hitpause, binding, facing and round/reset ownership. Use `engine/`, `character/`, `match/` and the existing state-flow, combat, entities and effects-pause segments. | Identify any documented lifecycle rule without a consumer or existing evidence. For a real gap, name one connected state path and its expected transitions/clocks. Do not enumerate every combination or reopen previously calibrated camera/turning behavior without evidence. |
| A2 | Asset/font/media contract: `resource/`, `sff/`, `air/`, `font/`, `snd/`, `sound/` and `storyboard/`. | Map each documented format family to its reader and production use, intentional rejection/fallback and existing acceptance. Distinguish missing licensed data from missing engine support. Name a complete load/use/release flow for any actual gap; arbitrary roster size is not a coverage metric. |
| A3 | Remaining rendering/audio fidelity: `render/`, `system/fight_state.go`, `character/ko_sound.go` and `sound/`. | Preserve closed camera, sprite/shadow, alpha and ordinary KO-echo findings. Identify a specific remaining timing, composition, filtering or playback mismatch before scheduling work. Explicitly distinguish functional playback from pixel/sample precision and list unresolved tolerances. |
| A4 | Options, training and native AI/debug policy: `system/options.go`, `system/input_config.go`, `system/fight_controls.go`, `system/runtime_debug.go` and `match/ai.go`. | Trace each scoped setting to its runtime effect, including session-only input edits. Separate documented behavior from current functional dummy/AI policy and undocumented reference decisions. Define observable acceptance for any mismatch; exact native random decision probabilities remain unverified unless evidence supports them. |
| A5 | Input compatibility: `config/config.go`, `config/gamepad.go`, `input/gamepad.go` and `system/input_config.go`. | Separate implemented portable/raw controls from native legacy joystick-number translation. Determine whether a translation consumer is missing and what platform/device evidence it requires; injected devices do not settle native numbering. Record hardware acceptance separately as H1. |

H1 is physical Linux controller acceptance: menu/configuration → selection →
combat → unplug/reconnect, retaining player ownership and held/edge behavior.
It requires an available controller and a person able to exercise it. Software
implementation can progress independently; this acceptance cannot be marked
complete from injected input. Do not assume a device is currently connected.

## Measurement after the assessment

Add the reviewed targets under the existing three partial catalog entries and
render them in the single progress ledger. Keep stable IDs and record:

| Field | Meaning |
| --- | --- |
| Contract / basis | Observable behavior, production consumer and target documentation or demonstrated mismatch. |
| Implementation | Missing, partial, implemented, or needing evidence. A source route alone does not establish behavior. |
| Acceptance | Unverified, passed for the named connected scenario, or awaiting a stated prerequisite. Keep this separate from implementation. |
| Size / confidence | Small: one local consumer; medium: connected changes across components; large: scheduler/ownership/format redesign. Confidence is high/medium/low with the unresolved reason. These are planning classes, not hours, tokens or commit counts. |
| Dependency / completion | What must be settled first and the concrete observable end state that closes the target. |

Report implementation targets completed/remaining **within that frozen scope**,
acceptance passed/pending, and unresolved/deferred requirements separately. Do
not combine them into an overall percentage. Do not weight the old 35 categories
equally or estimate work from commits, test cases, assets loaded or lines changed.
If a count percentage is later useful, label it as scoped target count only;
it still does not measure remaining effort or whole-engine equivalence.

New discoveries enter an explicit scope-change list with their evidence. Do not
quietly grow the denominator or relabel native-reproducing content defects as
engine work. Completion of the implementation backlog means the scoped code
gaps are closed; target compatibility additionally requires the named acceptance
checks. Neither establishes universal compatibility with arbitrary content.

## Proposed implementation order and stopping point

After the assessment is reviewed, implement K1/K2 as one coherent results batch,
then schedule confirmed compatibility defects by impact and dependency: crashes,
stalls and wrong state outcomes first; supported content/settings next; precision
last. Larger findings receive their own checkpoint. Validate affected connected
segments at batch end, record completion once and commit source/text locally.
No per-field test matrices, copied assets or per-run documentation are required.

The implementation milestone closes only when every in-scope code gap is closed
and its connected acceptance passes. Linux compatibility acceptance closes only
when all its in-scope contracts pass, including any required hardware checks.
Unknown or blocked work stays open; recording a reason does not complete it.
Explicitly deferred capabilities stay outside the milestone and remain visible.
Neither milestone alone establishes full 1.1 equivalence. Physical CD playback,
unavailable licensed/CJK font payloads, other-platform runtime acceptance and
standalone music-file CLI playback remain deferred; changing those exclusions
is a scope decision.
