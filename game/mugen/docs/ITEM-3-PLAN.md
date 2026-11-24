# Remaining scope and implementation order

Use [PROGRESS.md](PROGRESS.md) as the single completion ledger. This plan defines
the assessment boundaries and implementation order; it does not maintain another
set of completion checkmarks. The bounded source/documentation assessment is now
recorded under `remaining_scope` in the feature catalog and rendered near the
top of the ledger. Use that ledger for current implementation/acceptance status.

## What the current numbers establish

The source report finds 89/89 controller families and 127/127 trigger routes.
The project ledger has 33 implemented categories and two broad partial ones.
These are availability/category counts, not an effort estimate or a percentage
of native compatibility. A registered route can still have a behavioral defect.
The two partial categories mix missing consumers, unverified behavior and
capabilities deliberately deferred from the current Linux work.

The baseline identified four implementation candidates. Direct target review
corrected the first: tested result-text layerno values 0/1/2 all compose in front,
so the absent field read was not a behavioral gap. K1 now covers the demonstrated
caption viewport difference and preserving that composition policy. K2 includes
fresh result-scene initialization as well as Survival pose thresholds. K3/K4
cover AI.Cheat command policy and legacy joystick configuration.
The U3 Options/Training comparison established K5/K6, and the U1 overlapping
runtime comparison established K7. These explicit scope expansions are now
implemented and accepted. Of the six separate acceptance/evidence followups,
U1/U2/U3 are passed; U4, U5 and H1 remain open. U4 handler review establishes
K8: the former fixed-cooldown heuristic differed from native input decisions.
Weighted fallback movement, defence, attacks/cancels, recovery and ownership are
now implemented and accepted through the connected scenarios. Handler review
and direct observation also establish live learned move tables; their separate
movement/attack/guard planner remains missing within K8. This explicit eighth
target therefore remains partial. U4 still needs that planner's bounded
implementation and reference acceptance. Other unresolved evidence questions
are not assumed code defects. This bounded review does not establish an exhaustive list
of every semantic difference; unsupported-path scans alone cannot do that.

The U2 licensed Unicode TTF/TTC content journey explicitly added K9 after the
same input selected different outfits. K9 now closes authored keymaps, held
Start, available-outfit reservation and palette ownership through fight/reload.
The supported font/content functional category is accepted. Small font hinting,
edge and odd-width centering differences give U5 a named precision question;
they do not reopen format loading or earlier camera/sprite/audio calibrations.

## Assessment method and limits

Target the Linux implementation of M.U.G.E.N 1.1 beta 1, including documented
state/runtime behavior, complete game flows, supported assets/audio and input.
Older content is a compatibility input, not a promise that every pack is valid.
Keep previously closed discrepancies closed unless new evidence reopens them.

The completed assessment used these rules:

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

The original planning checkpoint reused documentation/source and existing
scenario evidence; subsequent bounded comparisons and closures are recorded in
the ledger. If a behavior cannot be resolved from existing evidence, record the precise
question and the smallest connected reference comparison needed. Do not start
native experiments, fixture generation or exhaustive compatibility sampling as
part of the planning checkpoint. Review each area once; carry unresolved
questions forward instead of expanding the assessment indefinitely.

## Implementation contracts

K1–K9 contracts, sources, prerequisites, estimates and completion conditions are
recorded once in the catalog and displayed in [the ledger](PROGRESS.md#remaining-implementation-and-acceptance).
Confidence describes the effort estimate. Result and AI-admission contracts are
resolved for their scoped implementations. Joystick translation uses target
device-code review; physical acceptance remains separate. K5/K6 close the named
Options/Training contract through direct target operation/handler review and
connected Linux acceptance. Session-only persistence, startup-binding reset and
portable input editing remain explicit port policies. K7 closes deferred contact
entry, ordinary Pause owner allowance through hitpause and finite TargetBind
expiry during global Pause through the connected turn/recovery/reset path.

## Bounded review areas

These are reviewed boundaries, not five equal units of remaining effort. The
ledger records their findings and allocations to K1–K9 and U1–U5/H1. The two
parent categories stay partial until their scoped requirements and acceptance
have been resolved.

| ID | Review boundary and existing sources | Required assessment output / acceptance boundary |
| --- | --- | --- |
| A1 | State/runtime ordering: pause/hitpause, binding, facing and round/reset ownership. Use `engine/`, `character/`, `match/` and the existing state-flow, combat, entities and effects-pause segments. | Identify any documented lifecycle rule without a consumer or existing evidence. For a real gap, name one connected state path and its expected transitions/clocks. Do not enumerate every combination or reopen previously calibrated camera/turning behavior without evidence. |
| A2 | Asset/font/media contract: `resource/`, `sff/`, `air/`, `font/`, `snd/`, `sound/` and `storyboard/`. | Map each documented format family to its reader and production use, intentional rejection/fallback and existing acceptance. Distinguish missing licensed data from missing engine support. Name a complete load/use/release flow for any actual gap; arbitrary roster size is not a coverage metric. |
| A3 | Remaining rendering/audio fidelity: `render/`, `system/fight_state.go`, `character/ko_sound.go` and `sound/`. | Preserve closed camera, sprite/shadow, alpha and ordinary KO-echo findings. Identify a specific remaining timing, composition, filtering or playback mismatch before scheduling work. Explicitly distinguish functional playback from pixel/sample precision and list unresolved tolerances. |
| A4 | Options, training and native AI/debug policy: `system/options.go`, `system/input_config.go`, `system/fight_controls.go`, `system/runtime_debug.go` and `match/ai.go`. | Trace each scoped setting to its runtime effect, including session-only input edits. Separate documented behavior from current functional dummy/AI policy and undocumented reference decisions. Define observable acceptance for any mismatch; exact native random decision probabilities remain unverified unless evidence supports them. |
| A5 | Input compatibility: `config/config.go`, `config/gamepad.go`, `input/gamepad.go` and `system/input_config.go`. | Separate implemented portable/raw controls from native legacy joystick-number translation. Determine whether a translation consumer is missing and what platform/device evidence it requires; injected devices do not settle native numbering. Record hardware acceptance separately as H1. |

The format-family review mapped the following production boundaries. This is
an availability map, not a second acceptance checklist or a parameter matrix.

| Format/resource family | Production consumer / existing evidence |
| --- | --- |
| Text definitions, CNS/CMD and AIR | `parsers/`, `cns/`, `cmd/`, `air/`; state-flow and command/asset lifecycles. |
| SFF v1, ACT and indexed PCX | `sff/sff.go`, `sff/pcx.go`, `character/palette_load.go`; palette and character/asset lifecycles. |
| SFF v2 indexed/raw/PNG and links | `sff/v2.go`; encoded-format regressions and connected stage/asset lifecycle. |
| FNT v1 and v2 bitmap | `font/font.go`, `font/v2.go`; legacy maps, glyph/bank/precedence asset flow. Bitmap v2 printable ASCII limits match the format documentation. |
| TrueType and collections | `font/truetype.go` uses `ParseCollection` and face zero; licensed UTF-8/blend/height/collection/menu/fight/reload acceptance passes under U2. Font raster/layout precision stays U5. |
| SND and WAV | `snd/wav.go`, `sound/mixer.go`; shared PCM/float decoder, ownership and media lifecycles. |
| WAV/MP3/Ogg/MIDI/MOD/S3M/XM/IT BGM | `sound/music.go`, `sound/legacy_linux.go`; consumed-cursor, NoMusic, loop and scene lifecycle. Optional codecs require their libraries. |
| Resource paths and character ZIPs | `resource/path.go`, `resource/characters.go`; selective portraits, reload, error recovery and archive cleanup in the asset lifecycle. |

Format docs, the target readme/CFG and motif comments provide the requirement
basis. External inputs are not copied into the repository. Library availability,
missing font payloads and native-reproducing script defects remain distinct from
missing engine behavior.

## Measurement and scope changes

The catalog maps reviewed targets to the existing partial categories; the
coverage tool validates their source links and renders the single ledger.
Keep stable IDs and record:

| Field | Meaning |
| --- | --- |
| Contract / basis | Observable behavior, production consumer and target documentation or demonstrated mismatch. |
| Implementation | Missing, partial, or implemented for a confirmed target; uncertain behavior stays in the separate evidence list. A source route alone does not establish behavior. |
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

## Implementation order and stopping point

1. K1/K2 form the results batch. Reference evidence requires fresh restored
   human actors, no enemy/HUD context, state170/180 threshold selection and
   game-viewport caption fitting. Preserve observed front composition for
   layerno 0/1/2. See the ledger for implementation/acceptance status.
2. Resolve AI.Cheat admission with one bounded comparison, then implement K3.
   Its large/low-confidence estimate reflects a missing command-admission path;
   it does not include reproducing native random decision probabilities (U4).
3. Establish native joystick enable/code translation, then implement K4 with
   explicit override precedence. Its medium/low-confidence estimate depends on
   platform/device mapping. Schedule physical acceptance H1 when hardware exists.
4. U1 is closed by K7, U2 by the font/content lifecycle plus K9, and U3 by K5/K6.
   Address U4/U5 through their named evidence questions. Create new code targets
   only for demonstrated missing/incorrect behavior and record the scope change.
   Prioritize crashes, stalls and wrong state outcomes over precision findings.

K3 admission and K4 device-code prerequisites are resolved for the scoped
implementations. Preserve the distinction between software acceptance and
native probability/hardware followups. Larger findings receive their own
checkpoint. Validate affected connected segments at batch end, record
completion once and commit source/text locally. No per-field test matrices,
copied assets or per-run documentation are required.

The implementation milestone closes only when every in-scope code gap is closed
and its connected acceptance passes. Linux compatibility acceptance closes only
when all its in-scope contracts pass, including any required hardware checks.
Unknown or blocked work stays open; recording a reason does not complete it.
Explicitly deferred capabilities stay outside the milestone and remain visible.
Neither milestone alone establishes full 1.1 equivalence. Physical CD playback,
unavailable licensed/CJK font payloads, other-platform runtime acceptance and
standalone music-file CLI playback remain deferred; changing those exclusions
is a scope decision.
