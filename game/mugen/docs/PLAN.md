# MUGEN 1.1 beta 1 implementation plan for review

Status: approved, 2026-10-16. Implementation and local checkpoint commits are
authorized. Current progress and validation are recorded in HANDOFF.md and
CHECKPOINTS.md; the assessment and initial inventory are dated baseline evidence.
After M4bd, implementation is paused at the user's request for progress and
token-budget review. See [PROGRESS-ASSESSMENT.md](PROGRESS-ASSESSMENT.md) before
resuming another bounded batch.

The goal is a working Go game with full coverage of the M.U.G.E.N 1.1 beta 1
state language and runtime assets. The stock KFM demo is an early delivery
milestone; completing it does not complete the compatibility target.

Read [ASSESSMENT.md](ASSESSMENT.md) for source evidence and baseline test results,
and [COMPATIBILITY.md](COMPATIBILITY.md) for all 89 controllers, 127 triggers,
and the asset-format inventory.

## Scope and working rules

All project edits, fixtures, copied working data, documentation, and handoffs
remain under `game/mugen/`. Preserve the supplied reference distribution.
Use `temp/` for build caches, disposable binaries, Wine prefixes, reference-run
copies, screenshots, and temporary asset extraction. Do not alter root
`go.mod`/`go.sum`, the C++ engine, or other data directories. Do not bundle the
reference executable into a release or commit the temporary distribution.

Keep the existing Go/Ebitengine direction. Reuse parsers/loaders/background/UI
code where it can meet the target; replace defective abstractions where needed.
Maintain one runtime implementation as code moves, rather than accumulating
separate demo and production engines.

Compatibility includes documented parameter variants, defaults, types, timing,
ownership, coordinate conversion, and version rules. Explicitly document target
features that are themselves disabled/unimplemented in the beta; investigate
them against the target instead of inventing behavior. Preserve documented
warnings/fallbacks, while surfacing unsupported behavior during validation.

The stock corpus is local test data, not an exhaustive specification. Add small
original fixtures for absent features and edge cases. Document any dependency
needed for formats such as TrueType/music before adopting it; dependency
management must remain within the approved directory boundary. If this proves
incompatible with the root module setup, bring that concrete constraint back
for review before making an out-of-scope change.

## Proposed architecture

| Responsibility | Proposed location / change | Reason |
| --- | --- | --- |
| Source and expression compilation | Evolve `parsers/`, `cns/`, `cmd/`, and `evaluator/` | Preserve source spans, compile expressions/parameters once, and validate against an explicit versioned schema. |
| Simulation | A display-independent `engine/` package, with Character state migrated from the current coupled implementation | Own fixed ticks, entities, match state, hit/guard resolution, pause, and event ordering in one place. Keep Ebitengine input/render/audio out of semantic tests. |
| Entities and ownership | Match-owned registry; evolve `character/` to use it | Separate world entity ID, player/input slot, helper ID, HitDef target ID, team, root/parent, state owner, and animation owner. |
| Assets | Shared resource layer under `game/mugen/`, reusing `sff/`, `snd/`, `air/`, and font parsing | Decode CPU data separately from GPU uploads/audio players; cache immutable assets without sharing runtime state. |
| Presentation | Ebitengine renderer/audio/input adapters and `system/` | Consume simulation snapshots/events and pass tick input into the core. Menus, test harnesses, and fights use the same behavior. |
| Verification | Deterministic tests plus authored reference fixtures under `tests/` and testdata directories | Record what a feature does, not merely whether its name parses. |

The expression runtime will represent int32, float32, and bottom distinctly.
An environment belongs to a specific evaluation, not a global callback. Parameter
schemas will distinguish expressions, enum/string fields, resource prefixes,
tuples, compile-time values such as persistent/ignorehitpause, and optional
values whose omission differs from zero.

The world will own entity creation/removal, stable iteration, target acquisition,
bindings, pause clocks, round clocks, deterministic random state, and events for
sound/effects. Tick ordering will be established from the reference and captured
in tests before behavior depends on it. Common CNS will drive scripted common
states; hardcoded fixes will be retained only where a documented engine rule
and reference observation justify them.

## Milestones and acceptance gates

### M0 — Reproducible baseline and reference harness

Fix the duplicate-main build problem and organize executable harnesses separately
from unit tests. Make test resources explicit and relocatable; required stock
coverage must not silently skip. Keep user configuration and the current working
directory from changing test outcomes. Add load/validation diagnostics with
file, section, state/controller, parameter, and expression context.

Establish a headless fixture runner with fixed input ticks/seeds and trace output.
Pin the reference corpus using a manifest/hash in documentation or test metadata.
Attempt the reference executable and sprite extraction tools with an isolated
Wine prefix and a disposable distribution copy under `temp/`. Preserve the
original distribution. If Wine cannot run the reference, report the exact issue
and prepare fixtures/capture instructions for Windows before claiming parity.

Gate: the entire Go tree builds/tests with one documented command; baseline
results and any reference-execution limitations are recorded. No unsupported
controller/trigger/load failure masquerades as completed compatibility.

### M1 — Correct language front end and typed expressions

Preserve expressions through DEF/CNS/CMD parsing, including operators, numeric
spelling, quotes, nested tuples, repeated attributes and source locations.
Handle encoding/BOM/newlines, comments, long lines, and paths deliberately.
Compile Statedef and controller arguments rather than parsing them each tick.

Implement the numeric model, all operators/precedence/associativity, assignment
side effects, interval restrictions, eager IfElse versus lazy Cond, bottom and
invalid operations. Implement math triggers, legacy condition syntax, enum/string
comparisons, component triggers, and legal redirection forms. Bind context-based
triggers as their underlying world features land; do not substitute successful
constant results for missing context.

Gate: table-driven language tests cover the reference grammar and types,
including `5/6` versus `5.0/6`, domain/zero-divisor cases, missing redirect
destinations, side-effect ordering, and the stock expression corpus. An
unsupported expression reports a source location and cannot silently remove
one condition from a controller.

### M2 — State scheduling, identity, ownership, and input

Implement a single entry path for every Statedef attribute, including omission
rules, expressions, `U`, ctrl/anim override priority, power/juggle/facing, and
hitdef/movehit/hitcount persistence. Implement contiguous trigger groups,
ordered repeated conditions, persistence counting, self-transitions, negative
state eligibility/order, and observable transition-limit diagnostics.

Define animation/physics/collision/timer order, hitpause/ignorehitpause,
Pause/SuperPause, move-time counters, and state/animation owner changes.
Separate input snapshots from keyboard polling. Implement CMD remaps and
sequence syntax, hold/release/charge, chords, strict modifiers, exact versus
four-way directions, command buffer timing, and facing changes. Correct the
KeyA/unset ambiguity and complete gamepad adapters.

Gate: deterministic tests exercise same-state re-entry, chained transitions,
paused boundaries, persistence with intermittent true triggers, custom-state
ownership, helper negative-state rules, and fixed input sequences. Probe
ambiguous negative-state transitions against Elecbyte; the C++ comment describing
different observed behavior is not enough to choose a rule.

### M3 — Complete asset decoding and resource loading

Implement SFF v1 plus v2/v2.01, including raw/RLE8/RLE5/LZ5 and PNG8/24/32,
palette/sprite links, transparency, ldata/tdata, and selected palettes. Harden
PCX/ACT/SND/FNT bounds and decoding. Implement FNT v2 bitmap/TrueType and bank
rules; use documented font lookup behavior with an explicit cross-platform
fallback policy. Add required animation metadata and 1.1 AIR offset/blend/
scale/angle interpolation.

Unify path lookup and required/optional resource errors for characters, state
files, motifs, stages, fight assets and storyboards. Honor every st/stN entry
and common/motif fallback with verified merge precedence. Decode CPU data
without requiring a window. Correctly distinguish a linked sprite from an
undecoded or invalid sprite.

Gate: every stock runtime asset loads completely, with declared counts checked
and decoded pixels/palettes/axes verified where applicable. Synthetic fixtures
cover formats not in the archive, links and malformed data. Cross-check sprite
extraction with the reference tool; rendering tests later cover placement/blend.

### M4 — First playable stock match

Connect title/selection/versus to a real Match/Fight state carrying players,
palettes, stage, mode and music overrides. Support stock KFM and KFM720,
common states, movement, attacks, guarding, recovery, round timer, KO/time-over,
round reset, and a complete result/rematch/exit path. Load and render the stock
fight definition, life/power bars, portraits, time/round announcements and common
effects. Connect character/common sound banks and basic channel playback.

Apply character, stage and motif localcoord transforms from the outset so a
320x240 character can fight a 1280x720 character. Give single-player modes a
basic opponent policy using the same simulation and establish AILevel plumbing;
two-player versus must accept distinct controllers.

Gate: a user can launch from the normal launcher, select characters/stage,
complete a stock match, and start another or return to the menu. A scripted
headless match exercises the same Match code through KO and time-over.
This milestone is explicitly playable, with the remaining compatibility gaps
still listed; it is not the final verification point.

### M5 — Full combat, targets, helpers, and projectiles

Complete HitDef schema and dynamic parameter evaluation, active-hit gating,
hit/guard flags, attributes, priority, teams, juggle, damage/power accounting,
corner push, fall/recovery/KO rules, contact/reversal/hit counters, and all
GetHitVar fields. Implement HitBy/NotHitBy/HitOverride/ReversalDef behavior.

Implement acquired target sets and HitDef IDs, target/bind operations, throw
custom states and ChangeAnim2/SelfState ownership. Complete helper parameters,
parent/root relationships, control, own palettes, limits and DestroySelf.
Implement projectiles as their own simulation/lifecycle rather than ordinary
state-0 Characters: hits, cancellations, removal animations, bounds, counts,
contact-time triggers and palette options. Back all entity/query triggers with
the shared world model.

Gate: two different characters can throw/release each other, multiple helpers
and projectiles coexist with unique identity, guarding uses its own damage and
timing, reversals/overrides work, and destruction leaves no stale bindings or
targets. Each behavior is exercised by focused fixtures and multi-entity traces.

### M6 — Remaining controllers/triggers, effects, audio, and 1.1 rendering

Close every remaining controller and trigger row in the compatibility inventory,
including the eight missing dispatcher entries and every stub. Cover their
entire documented parameter sets, not just defaults. Implement Explod/
ModifyExplod/ExplodBindTime lifetimes, motion, layers, binding, ownpal/remappal
and 1.1 screen/stage-space behavior.

Complete afterimages, palette effects/remapping, angle/scale/offset drawing,
transparency/blending, priority sorting, shake/color, width/screen bounds,
shadows/reflections, and background/controller reset/timing behavior. Implement
camera zoom limits, zoomdelta, coordinate conversion and edge/screen/camera
triggers consistently across simulation and rendering.

Complete PlaySnd/SndPan/StopSnd channel ownership, volume/pan/loop/resampling,
character/common/fight resources, BGM formats/looping/transitions, debug text
formatting and force-feedback events. Rendering/audio/haptics should expose
testable commands/state even when a physical device is unavailable.

Gate: all 89 controllers and 127 trigger entries have semantic tests, all
supported parameter variants are accounted for, and no behavior-bearing stub or
silent unsupported path remains. Visual/audio fixtures demonstrate the rendered
or emitted effects, including 1.1 zoom and mixed-coordinate scenes. This gate
precedes the final reference-conformance sweep; tests alone are not parity proof.

### M7 — Complete game modes, UI, and lifecycle

Implement mode-specific behavior behind the existing menus: arcade progression
and endings, versus, team/simul/turns and co-op, survival, training, and watch.
Complete team selection, AI/difficulty behavior, match ordering, win quotes,
continue/game-over/storyboards, options persistence and applicable configuration
rules. UI skins must use motif/fight assets rather than permanent placeholders.

Gate: every exposed mode has an executable end-to-end scenario, configuration
round-trips correctly within the allowed directory, and repeated matches reset
runtime state without corrupting cached assets or leaking entities/audio/GPU
resources. Any advertised target behavior that remains absent stays open.

### M8 — Reference conformance and final verification package

Run every fixture through the Go runner and, where observable, the isolated
Elecbyte reference. Compare state/animation transitions and elapsed timers,
variables, positions/velocities, facing/control, life/power, target ownership,
entity creation/removal, pauses, and events at specified ticks. Capture reference
values using authored diagnostic CNS and debugger/visible output; do not assume
access to the closed-source engine's internal state or RNG seed.

Use exact comparisons for discrete behavior and define any numeric/pixel/audio
tolerance per fixture with a reason. For random behavior, compare specified
ranges/distributions and downstream invariants unless equivalent seeds can
actually be established. Compare sprite pixels exactly when decoded RGBA values
permit it; separate font/platform-rendering differences from simulation defects.

Add malformed-input/fuzz cases for parsers, decoders, command matching and
zero-time/infinite animations. Run appropriate race checks and sustained
multi-entity/multi-round tests. Record reproducible failures and fix their
causes. Update the inventory with evidence instead of marking all names done.

Gate: no open required compatibility case, no unexplained reference divergence,
all documented tests pass, full stock content works through the production
launcher, and a handoff contains run commands, controls, evidence and any
explicitly agreed platform limitations. Then hand the working game to the user
for their final interactive verification.

## Dependencies and iteration

M0 comes first. M1 and the foundations of M2 establish reliable behavior;
M3 can be completed independently after M0. M4 joins those foundations into the
first playable game. M5/M6 close semantic and rendering gaps; M7 completes the
exposed game flows. Reference comparisons begin with M0 fixtures and continue
throughout, with M8 providing the final sweep.

For each feature: document the reference rule, add a focused failing fixture,
implement it, run the relevant checks, compare observable behavior, and update
the inventory/handoff. Use small coherent changes with green checks between
milestones. Do not pause for routine implementation choices after approval;
raise concrete scope/environment blockers and ambiguities that cannot be
resolved from available evidence.

## What completion means

Coverage is semantic and traceable: every documented controller/trigger, every
required/default/optional parameter behavior, language rule, supported asset
format, state/match lifecycle, and 1.1 coordinate/effect addition has a test or
explicit reference-backed applicability decision. Inventory omissions, no-op
stubs, skipped required fixtures, and unexplained divergence prevent a full
compatibility claim. Line coverage is supporting evidence only.

This is substantial engine completion work. A calendar estimate should follow
M0/M1, when the reference harness and expression backlog are measured. The
review decision now is whether to proceed with this scope and milestone order;
implementation proceeds under the user's approval of this plan.
