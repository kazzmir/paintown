# M.U.G.E.N 1.1 beta 1 compatibility inventory

Assessment snapshot: 2026-10-16. The tables below describe the original semantic
baseline. M0a now fixes the build and pins the reference test corpus; no semantic
row is promoted to Verified. See [CHECKPOINTS.md](CHECKPOINTS.md),
[ASSESSMENT.md](ASSESSMENT.md), and [PLAN.md](PLAN.md).

## Implementation progress

M1a preserves source expressions and locations. M1b implements typed numeric/
bottom evaluation, operators, assignment, math/special forms and strict legacy/
redirect syntax. Language regressions and headless JSONL replay pass; all 1522
stock trigger expressions parse. The global custom-evaluation hook is removed.
State scheduling, parameter compilation, constant/world/GetHitVar bindings and
reference comparisons are still incomplete. The baseline tables remain a dated
inventory; these language tests alone do not promote gameplay entries to Verified.

M2a moves the shared executor to `engine/`, compiles trigger groups once,
rejects invalid states/unknown controller names before effects, corrects group
order/gaps and persistence, and adds Null. Headless state race tests pass.
Production pause/timer integration, state entry, typed controller parameters,
world ownership, and executable reference confirmation are still outstanding.

M2b adds a single typed state-entry path and integrates hitpause passes/timer
fixes in Character.Update. Regression coverage now includes entry omission/U/
expressions/overrides/persistence, hitpause boundaries, contact timers, PosFreeze,
Gravity, active-hit gating and runtime failure propagation. Entry/physics phase
ordering and exact reference edge cases still require executable comparisons.

M2c compiles typed expressions for core movement, variables/ranges/parent
variables, transitions, life/power/control, angles/multipliers and several simple
controllers. Integer arguments truncate floats with warnings. New headless
behavior tests cover these paths. HitAdd has a separate combo counter and
VictoryQuote now honors its documented default/helper rule. World ownership,
combat integration and reference parity remain outstanding; the dated tables
below describe the original assessment rather than current completion counts.

M3a adds SFF v2/v2.01 and all required sprite encodings. All 1376 stock sprites
decode. Every KFM sprite matches Elecbyte SFF2PNG's opaque pixel output; original
reference-compressed fixtures cover absent encoding variants. Palette selection
is available at the decoder boundary; player/font integration remains pending.

M3b loads FNT v2 palette/sprite-bank bitmaps and TrueType/OpenType/TTC. All 19
runtime font files load. Tests cover banks, alpha, metrics and UTF-8/blend paths;
Windows fonts use an explicit portable fallback if absent. Reference rendering
parity is pending. See BUILD.md for dependencies and font lookup.

M2d unifies animation clocks and binds AnimElem/AnimElemTime/AnimElemNo in
Character. A 24-tick authored looping action matches the supplied executable's
recorded AnimTime, all three element elapsed times, and current-element queries.
Other offset/zero/infinite cases have documentation-based tests but still need
reference probes. Both stock players load; 105/108 states per character compile,
with HitOverride blocking the remaining three. Match playability is pending.

M2e adds executable reference evidence for negative-state order/abortion and
persistence. The success-count interpretation is superseded: N uses a cooldown,
and suppressed/used one-shot controllers skip triggers. Character and headless
hosts pass both authored programs against pinned reference variables.

M1c covers the 74 documented Const names: 73 supported stock values match the
executable and their declared kinds are tested. The beta rejects the documented
jump.changeanim.threshold constant for both 1.0 and 1.1 character declarations;
compiled paths now reject it too. Constant omission/required-field, tuple/default
and localcoord/version edge cases remain open. See REFERENCE.md for evidence.

M3c validates SND/WAV structure and decodes all 45 stock sounds across five banks.
Authored fixtures cover additional PCM widths/channels and malformed archives.
Runtime audio and non-PCM codecs remain incomplete. New resource probes pin fatal
missing-file behavior and the st0..st9 range for the upcoming loader changes.

M3d applies character resource rules with named errors, case/Windows path
resolution, sound loading and independent common-state definitions. The measured
state-file precedence is st, numbered slots, CMD, then common; main cns is
constants-only. All twenty native resource cases now compare directly after
M2f aligned the common probe with the round control grant. Selected palettes and exact common search order remain open.

M2f fixes transition/entry control and animation precedence with executable
evidence. Transition values apply first, are visible to entry expressions, and
explicit Statedef values win. All 24 values in an ordinary seven-state sequence
match both hosts. Production round setup and full entry/owner ordering remain
incomplete; this does not promote the entire ChangeState controller to Verified.

M1d binds controllers to their preceding Statedef, preserves the first duplicate
definition and ignores orphan controllers. Native fixtures match both execution
hosts; no phantom states are created from controller labels. Malformed/edge
header and negative-state ownership cases remain outstanding.

M3e loads every stock motif's main archives/fonts and all three stock stages,
with explicit errors for declared missing/malformed resources. Font lookup now
includes font/ in the documented order, and bitmap child files honor Windows
names. Per-background overrides, storyboards, language/font overrides and visual
parity remain pending.

M3f loads separate sprite archives for title/select/versus/options backgrounds
and uses them in production draw phases, including the KFM title screen. Archive
ownership and layer/cache behavior have regressions; full visual/background
semantics and storyboard integration remain unfinished.

M3g adds the shared storyboard loader and application integration. All eight
stock storyboards load and complete CPU playback; backgrounds, declared fonts/
sound banks, 100 animation layers, safe scene indices and error/path handoffs are
covered. Text, timed audio, full rendering and native timing parity remain open.

M0d production captures show the KFM logo/intro/title flow with readable fonts
and separate title graphics. Normal shared-AST sections no longer generate false
unsupported warnings. This is a startup inspection only; input decoding,
selection-to-fight and complete visual/gameplay parity remain unfinished.

M4a fixes stock SDL keyboard numbers and explicit KeyA/disabled presence. All 22
stock bindings, YAML overrides and facing-relative gameplay input have tests.
Keyboard sources are injectable per manager and confirm bindings are player-local.
Full command/gamepad/layout behavior and playable match flow remain incomplete;
see INPUT.md.

M3h loads both stock fight/HUD definitions and their declared main/effect/
common sound/font/animation resources, retaining raw layout/source metadata.
Production HUD drawing, round announcements/audio/effects and actual fight flow
remain incomplete; resource loading alone is not gameplay compatibility.

M4b retains coordinate profiles and binds all three scaled-constant functions.
Five native cases match, including fractional-width coercion and float return
behavior. HD motif layout/default windows/origin and physical window size are
corrected. Mixed-player geometry, camera/cropping and actual fight flow remain
unfinished; see COORDINATES.md.

## Evidence and status definitions

Primary reference: the unmodified [bundled distribution](../temp/mugen-1.1b1/), especially [CNS](../temp/mugen-1.1b1/docs/cns.html), [controllers](../temp/mugen-1.1b1/docs/sctrls.html), [triggers](../temp/mugen-1.1b1/docs/trigger.html), [release history](../temp/mugen-1.1b1/docs/history.html), [fonts](../temp/mugen-1.1b1/docs/fonts.html), [AIR](../temp/mugen-1.1b1/docs/air.html), [backgrounds/stages](../temp/mugen-1.1b1/docs/bgs.html), and [coordinate spaces](../temp/mugen-1.1b1/docs/coordspace.html).

- **Missing**: no controller registration or trigger evaluation path found.
- **Stub**: registered controller reaches an empty implementation, including indirect empty methods.
- **Present**: some behavior exists, with documented gaps or no semantic tests. This does not mean compatible.
- **Verified**: requires behavior tests and reference evidence for the advertised semantics; no controller/trigger currently qualifies.

Counts are inventory counts, not compatibility percentages. Every row needs parameter/default/type/version coverage, negative cases, interaction tests, and reference evidence before promotion to Verified.

## Controllers

The reference documents 89 controllers. The port registers 84 names: 81 documented names plus three non-reference FVar convenience names. Eight documented names are missing. The stock KFM/KFM720/common-state corpus uses 46 distinct controller types and cannot cover the complete reference. Occurrence counts below come from their CNS/CMD files and are not execution counts.

| Controller | Baseline | Stock occurrences | Principal gap |
| --- | --- | ---: | --- |
| AfterImage | Stub | 14 | Registered handler has no effective implementation. |
| AfterImageTime | Stub | 20 | Registered handler has no effective implementation. |
| AllPalFX | Stub | 0 | Registered handler has no effective implementation. |
| AngleAdd | Present | 0 | Updates DrawAngle; renderer does not apply the full angle/scale behavior. |
| AngleDraw | Present | 0 | Sets a number; angle/scale drawing and enable semantics need implementation. |
| AngleMul | Present | 0 | Updates DrawAngle; render consumption and persistence missing. |
| AngleSet | Present | 0 | Updates DrawAngle; render consumption and persistence missing. |
| AppendToClipboard | Present | 0 | Stores raw text; format parameters and complete debug behavior missing. |
| AssertSpecial | Present | 4 | Stores flags; most engine/render consumers and durations missing. |
| AttackDist | Missing | 0 | Missing dynamic guard-distance controller. |
| AttackMulSet | Present | 0 | Literal parsing; dynamic expressions and combat scaling rules incomplete. |
| BGPalFX | Stub | 0 | Registered handler has no effective implementation. |
| BindToParent | Stub | 0 | Registered handler has no effective implementation. |
| BindToRoot | Stub | 0 | Registered handler has no effective implementation. |
| BindToTarget | Stub | 0 | Registered handler has no effective implementation. |
| ChangeAnim | Present | 68 | Expression value supported; element parameter and animation-owner semantics incomplete. |
| ChangeAnim2 | Stub | 6 | Registered handler has no effective implementation. |
| ChangeState | Present | 246 | Basic transition; entry override ordering, ownership, defaults and expressions incomplete. |
| ClearClipboard | Present | 0 | Clears local text; debug display integration unverified. |
| CtrlSet | Present | 21 | Literal-only value parsing. |
| DefenceMulSet | Present | 2 | Literal parsing; target-version damage rules unverified. |
| DestroySelf | Stub | 0 | Registered handler has no effective implementation. |
| DisplayToClipboard | Present | 0 | Stores raw text; formatting/params and debug display incomplete. |
| EnvColor | Stub | 0 | Registered handler has no effective implementation. |
| EnvShake | Stub | 6 | Registered handler has no effective implementation. |
| Explod | Present | 9 | Basic record; first frame only, lifetime/motion/binding/1.1 space parameters incomplete. |
| ExplodBindTime | Missing | 0 | No dispatcher entry. |
| ForceFeedback | Stub | 16 | Registered handler has no effective implementation. |
| FallEnvShake | Stub | 2 | Registered handler has no effective implementation. |
| GameMakeAnim | Stub | 0 | Registered handler has no effective implementation. |
| Gravity | Present | 0 | Sets a persistent flag rather than implementing one controller activation. |
| Helper | Present | 0 | Basic Character allocation; unique ID, parameter set, control and lifecycle incomplete. |
| HitAdd | Present | 0 | Increments a counter; hit/unique-hit bookkeeping and expressions incomplete. |
| HitBy | Present | 0 | Records masks/timers; collision does not enforce them. |
| HitDef | Present | 82 | Subset parsed as literals; hit/guard/priority/juggle/target/custom-state rules incomplete. |
| HitFallDamage | Present | 2 | Adds damage to life; complete fall/KO/defence behavior unverified. |
| HitFallSet | Present | 1 | Boolean fall value only; velocity parameters and complete semantics missing. |
| HitFallVel | Present | 1 | Basic flag/velocity application; fall lifecycle unverified. |
| HitOverride | Missing | 8 | No dispatcher entry; stock characters use this. |
| HitVelSet | Present | 10 | Interprets selectors as multipliers; typed arguments and hit velocity semantics incomplete. |
| LifeAdd | Present | 0 | Literal value; kill/absolute flags and complete damage rules missing. |
| LifeSet | Present | 0 | Literal value; expressions and bounds/defaults need tests. |
| MakeDust | Stub | 1 | Registered handler has no effective implementation. |
| ModifyExplod | Missing | 0 | No dispatcher entry. |
| MoveHitReset | Present | 0 | Clears hit deduplication, rather than resetting the complete MoveHit/MoveGuarded status. |
| NotHitBy | Present | 26 | Records masks/timers; collision does not enforce them. |
| Null | Missing | 8 | No dispatcher entry; must be an explicit documented no-action controller. |
| Offset | Present | 0 | Stores offsets; renderer does not apply them. |
| PalFX | Stub | 16 | Registered handler has no effective implementation. |
| ParentVarAdd | Missing | 0 | No dispatcher entry. |
| ParentVarSet | Missing | 0 | No dispatcher entry. |
| Pause | Stub | 6 | Registered handler has no effective implementation. |
| PlayerPush | Missing | 0 | No dispatcher entry; push logic cannot be disabled through this controller. |
| PlaySnd | Stub | 90 | Registered handler has no effective implementation. |
| PosAdd | Present | 63 | Literal components; coordinate-space semantics and dynamic expressions incomplete. |
| PosFreeze | Present | 6 | Persistent local flag; tick/hitpause/reset semantics need reference tests. |
| PosSet | Present | 14 | Some expression evaluation; localcoord and world integration incomplete. |
| PowerAdd | Present | 0 | Literal-only value parsing. |
| PowerSet | Present | 0 | Literal-only value parsing; fixed maximum power. |
| Projectile | Present | 0 | Generic Character creation; projectile parameters, HitDef, collision and lifecycle incomplete. |
| RemapPal | Stub | 1 | Registered handler has no effective implementation. |
| RemoveExplod | Present | 0 | Filters matching IDs; default ID -1 does not remove all. |
| ReversalDef | Present | 12 | Stores raw parameters; collision does not enforce reversal. |
| ScreenBound | Stub | 4 | Registered handler has no effective implementation. |
| SelfState | Present | 11 | Aliases ChangeState; cannot restore own states/animations after custom-state ownership. |
| SprPriority | Present | 2 | Stores literal value; full renderer ordering missing. |
| StateTypeSet | Present | 21 | Basic enum setters; unchanged/default and version semantics need tests. |
| SndPan | Stub | 0 | Registered handler has no effective implementation. |
| StopSnd | Stub | 0 | Registered handler has no effective implementation. |
| SuperPause | Stub | 4 | Registered handler has no effective implementation. |
| TargetBind | Stub | 18 | Registered handler has no effective implementation. |
| TargetDrop | Stub | 0 | Registered handler has no effective implementation. |
| TargetFacing | Stub | 2 | Registered handler has no effective implementation. |
| TargetLifeAdd | Stub | 2 | Registered handler has no effective implementation. |
| TargetPowerAdd | Stub | 0 | Registered handler has no effective implementation. |
| TargetState | Stub | 2 | Registered handler has no effective implementation. |
| TargetVelAdd | Stub | 0 | Registered handler has no effective implementation. |
| TargetVelSet | Stub | 0 | Registered handler has no effective implementation. |
| Trans | Stub | 0 | Registered handler has no effective implementation. |
| Turn | Present | 10 | Flips facing; local momentum/drawing interactions need reference tests. |
| VarAdd | Present | 1 | Literal index/value only; alternate var/fvar forms and expressions missing. |
| VarRandom | Stub | 0 | Registered handler has no effective implementation. |
| VarRangeSet | Present | 2 | Uses range instead of complete first/last/value/fvalue schema. |
| VarSet | Present | 21 | Some integer/sysvar forms; fv/fvar and dynamic index/schema coverage incomplete. |
| VelAdd | Present | 39 | Basic expression components; source preservation/types/scaling unverified. |
| VelMul | Present | 15 | Basic expression components; source preservation/types/scaling unverified. |
| VelSet | Present | 46 | Basic expression components; source preservation/types/scaling unverified. |
| VictoryQuote | Present | 0 | Stores literal quote index; victory screen does not consume it. |
| Width | Stub | 32 | Registered handler has no effective implementation. |

Controller status totals: 8 Missing, 34 Stub, 47 Present, 0 Verified.

The extra registrations are `FVarSet`, `FVarAdd`, and `FVarRangeSet`. They do not replace the documented floating-variable forms of VarSet/VarAdd/VarRangeSet.

## Triggers

The reference documents 127 trigger entries, including math functions. Base names below remove documentation annotations such as `(math)` and `(*,***)`. Component variants, redirects, old comparison syntax, accepted arguments, types, and version behavior are additional obligations within each row. All Present entries inherit the evaluator numeric-type/bottom defects described in the assessment.

| Trigger | Baseline | Principal gap / evidence |
| --- | --- | --- |
| Abs | Present | Math call exists; types/bottom not modeled. |
| Acos | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| AILevel | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Alive | Present | Life comparison exists; match/dead-entity semantics unverified. |
| Anim | Present | Reads character animation number. |
| AnimElem | Present | Compares current element index; start-time/old condition syntax not implemented correctly. |
| AnimElemNo | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| AnimElemTime | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| AnimExist | Present | Queries own AIR only; custom-state ownership unverified. |
| AnimTime | Present | Computes remaining element durations; exact tick and loop semantics unverified. |
| Asin | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Atan | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| AuthorName | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| BackEdge | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| BackEdgeBodyDist | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| BackEdgeDist | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| BottomEdge | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| CameraPos | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| CameraZoom | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| CanRecover | Present | Uses fall.recover flag; recovery timing and full get-hit state incomplete. |
| Ceil | Present | Math call exists; types/bottom not modeled. |
| Command | Present | Named commands plus raw-input fallback; matcher and strict name semantics incomplete. |
| Cond | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Const | Present | Small subset of velocity/movement constants; data/size/remaining constants and scaling absent. |
| Const240p | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Const480p | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Const720p | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Cos | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Ctrl | Present | Reads control flag; state-entry override ordering defective. |
| DrawGame | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| E | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Exp | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Facing | Present | Reads facing; entity/coordinate interactions unverified. |
| Floor | Present | Math call exists; types/bottom not modeled. |
| FrontEdge | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| FrontEdgeBodyDist | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| FrontEdgeDist | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| FVar | Present | Reads float array; fixed Go float64 rather than target numeric model. |
| GameHeight | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| GameTime | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| GameWidth | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| GetHitVar | Present | Subset of attributes; get-hit data and defaults incomplete. |
| HitCount | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| HitDefAttr | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| HitFall | Present | Reads local fall flag; hit/fall lifecycle incomplete. |
| HitOver | Present | Reads HitTime; counter decrements twice on unpaused ticks. |
| HitPauseTime | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| HitShakeOver | Present | Reads ShakeTime; pause tick ordering unverified. |
| HitVel | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| ID | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| IfElse | Present | Evaluates one branch lazily, whereas target IfElse evaluates all arguments. |
| InGuardDist | Present | Queries opponent-like Target list; dynamic AttackDist and team/range rules incomplete. |
| IsHelper | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| IsHomeTeam | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| LeftEdge | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Life | Present | Reads life; damage/guard/match rules incomplete. |
| LifeMax | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Ln | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Log | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Lose | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| MatchNo | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| MatchOver | Present | Reads manually set flag; production match progression missing. |
| MoveContact | Present | One-tick-like flag reset before state checks; elapsed-time semantics missing. |
| MoveGuarded | Present | Getter exists, but guarding does not correctly record this status. |
| MoveHit | Present | One-tick-like flag reset before state checks; elapsed-time semantics missing. |
| MoveType | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| MoveReversed | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Name | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| NumEnemy | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| NumExplod | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| NumHelper | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| NumPartner | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| NumProj | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| NumProjID | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| NumTarget | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| P1Name | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| P2BodyDist | Present | Aliases axis distance; body edges are not subtracted. |
| P2Dist | Present | Uses first Target as opponent; real enemy selection/scaling missing. |
| P2Life | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| P2MoveType | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| P2Name | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| P2StateNo | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| P2StateType | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| P3Name | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| P4Name | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| PalNo | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| ParentDist | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Pi | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Pos | Present | X/Y getters only; correct localcoord and legacy Z semantics need work. |
| Power | Present | Reads local power; maximum fixed and power-award integration incomplete. |
| PowerMax | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| PlayerIDExist | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| PrevStateNo | Present | Reads previous state; custom-state/transition semantics unverified. |
| ProjCancelTime | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| ProjContact | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| ProjContactTime | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| ProjGuarded | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| ProjGuardedTime | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| ProjHit | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| ProjHitTime | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Random | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| RightEdge | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| RootDist | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| RoundNo | Present | Hardcoded to 1. |
| RoundsExisted | Present | Reads field; production round reset/progression absent. |
| RoundState | Present | Reads field; real match/intro/outro phase management absent. |
| ScreenPos | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| ScreenHeight | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| ScreenWidth | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| SelfAnimExist | Present | Shares AnimExist path; ownership distinction not implemented. |
| Sin | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| StateNo | Present | Reads current state number. |
| StateType | Present | Implemented through process-global custom evaluation hook. |
| StageVar | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| SysFVar | Present | Reads fixed float array; target typing and bounds behavior unverified. |
| SysVar | Present | Reads fixed int array; target typing and bounds behavior unverified. |
| Tan | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| TeamMode | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| TeamSide | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| TicksPerSecond | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Time | Present | Reads character Time; timing/reset/hitpause semantics need tests. |
| TimeMod | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| TopEdge | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| UniqHitCount | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |
| Var | Present | Reads integer array; fixed-width arithmetic/bounds behavior unverified. |
| Vel | Present | X/Y getters plus fixed zero for Z; scaling and legacy version rules incomplete. |
| Win | Missing | No evaluation implementation found; add syntax, environment binding and semantics tests. |

Trigger status totals: 86 Missing, 41 Present, 0 Verified.

## Language and scheduling obligations

| Area | Baseline | Acceptance examples |
| --- | --- | --- |
| Source fidelity | Token reconstruction changes expressions | Preserve source locations, quotes, operator spelling, tuple nesting, integer/float spelling, and duplicate attributes. |
| Numeric model | float64-only plus ad hoc unknown sentinel | Int32, float32, bottom, casts, overflow/domain/zero-divisor cases, and evaluation order match the reference. |
| Operators | Several missing | Unary, exponent, arithmetic, comparisons/intervals, assignment, bitwise and logical operators; associativity and side effects. |
| Special forms | Partial IfElse only | Eager IfElse versus lazy Cond, including side effects and unused-branch bottom behavior. |
| Trigger syntax | Subset | All old forms and component forms, strings/enums, legal redirects and missing destinations. |
| State entry | Applied in two places | Every Statedef attribute, omission/defaults, expressions, override ordering, self-transition and owner switch. |
| Trigger groups | Partial | triggerall first, ordered contiguous numbered groups, repeated clauses and short-circuit behavior between clauses. |
| Persistence | >1 unimplemented | M2e implements observed activation/cooldown and one-shot suppression; pause/negative-state re-entry interactions still need probes. |
| Tick scheduling | Local Character.Update | Negative-state eligibility/order, hitpause/ignorehitpause, Pause/SuperPause, animation/physics/collision ordering and transitions. |
| Lifetimes and ownership | Incomplete | Distinct entity/input/helper/target identities; state and animation owners; creation/deletion and binding. |
| Version behavior | Mostly absent | Character mugenversion, deprecated syntax and documented 1.1 additions. |

## Reference asset census

Header census of the supplied distribution: 27 SFF, 5 SND, 9 FNT (including two old-tools examples), 33 DEF, 3 AIR, 2 CMD, and 3 CNS files. The archive also contains executable support files, documentation and internal shaders/Lua; those are not themselves evidence of Go runtime asset support. No assets have been copied or modified.

| Asset family | Current port | Required coverage |
| --- | --- | --- |
| DEF/CNS/CMD/config/select | Basic parsers and loaders | Source-preserving syntax, encoding/BOM/newlines, defaults, all file mappings, ordered merge and case/path rules. |
| AIR / embedded AIR | Basic frames/boxes/loops | Frame timing/zero/infinite frames, interpolation, flips, blend/alpha and required animation fallbacks. |
| SFF v1 / PCX / ACT | Partial | Links, palettes, masking versus opaque, exact pixels, malformed inputs and selected palette behavior. |
| SFF v2/v2.01 | Rejected | Raw, RLE8, RLE5, LZ5, PNG8/24/32; palette/sprite links, ldata/tdata, depth, offsets and bounds. |
| FNT v1 | Partial | Fixed/variable metrics, mapping, banks, glyph transparency, offsets and invalid containers. |
| FNT v2 bitmap / TrueType | Missing | SFF palette/sprite banks including RGBA, text metrics/UTF-8 where specified, font file lookup and fallback policy. |
| SND / WAV | SND extraction and menu WAV playback | Complete RIFF variants used by target, decode/resample/mix, source ownership, channel/pan/loop/stop and lifecycle. |
| BGM | Paths partly parsed; playback missing | Target-supported formats, looping/volume/transitions, stage/motif/storyboard music. Additional fixtures needed: stock distribution has no music files. |
| Stage/motif/storyboard/fight | Partial loaders; fight UI absent | Default resource paths, localcoord/zoom, animation/layers/background controllers, stage shadows/reflections, HUD and progression. |

### SFF files and record formats

Versions below are reported in header byte order major.minor.revision.build. v2.01 in the release history appears as `2.0.1.0`. Counts are declared sprite records, including links; these are not decoded-image success counts. Stock v2 record formats are 0/8 raw indexed (94), 2/8 RLE8 (472), 4/5 LZ5 (559), and 12/32 PNG32 (22). The PNG32 record signature was checked after its four-byte decompressed-size prefix.

| File under reference root | Header version | Sprite records | Record formats |
| --- | --- | ---: | --- |
| `chars/kfm/ending.sff` | 1.0.1.0 | 7 | PCX/link: 7 |
| `chars/kfm/intro.sff` | 1.0.1.0 | 12 | PCX/link: 12 |
| `chars/kfm/kfm.sff` | 2.0.1.0 | 281 | RLE8: 1, LZ5: 280 |
| `chars/kfm720/ending.sff` | 1.0.1.0 | 7 | PCX/link: 7 |
| `chars/kfm720/intro.sff` | 1.0.1.0 | 12 | PCX/link: 12 |
| `chars/kfm720/kfm720.sff` | 2.0.0.0 | 281 | RLE8: 2, LZ5: 279 |
| `data/fight.sff` | 1.0.1.0 | 43 | PCX/link: 43 |
| `data/fightfx.sff` | 1.0.1.0 | 104 | PCX/link: 104 |
| `data/kfm/credits.sff` | 1.0.1.0 | 2 | PCX/link: 2 |
| `data/kfm/gameover.sff` | 1.0.1.0 | 1 | PCX/link: 1 |
| `data/kfm/intro.sff` | 1.0.1.0 | 8 | PCX/link: 8 |
| `data/kfm/kfmtitle.sff` | 1.0.1.0 | 2 | PCX/link: 2 |
| `data/kfm/logo.sff` | 1.0.1.0 | 1 | PCX/link: 1 |
| `data/mugen1/fight.sff` | 2.0.0.0 | 51 | RLE8: 51 |
| `data/mugen1/system.sff` | 2.0.0.0 | 36 | RLE8: 36 |
| `data/system.sff` | 1.0.1.0 | 30 | PCX/link: 30 |
| `font/arcade.sff` | 2.0.0.0 | 67 | RLE8: 67 |
| `font/enter48.sff` | 2.0.0.0 | 94 | RLE8: 94 |
| `font/f-4x6.sff` | 2.0.0.0 | 94 | raw indexed: 94 |
| `font/f-6x9.sff` | 2.0.0.0 | 94 | RLE8: 94 |
| `font/name14.sff` | 2.0.0.0 | 94 | RLE8: 94 |
| `font/num1.sff` | 2.0.0.0 | 11 | RLE8: 11 |
| `font/num48.sff` | 2.0.0.0 | 11 | RLE8: 11 |
| `font/num72.sff` | 2.0.0.0 | 22 | PNG32: 22 |
| `stages/kfm.sff` | 2.0.1.0 | 7 | RLE8: 7 |
| `stages/stage0-720.sff` | 2.0.0.0 | 2 | RLE8: 2 |
| `stages/stage0.sff` | 2.0.0.0 | 2 | RLE8: 2 |

RLE5, PNG8/24, many malformed cases, charge/chord input, helpers, projectiles, most controller parameter variants, team behavior and TrueType font payloads require added synthetic or user-provided fixtures. Passing every stock file is necessary but insufficient.

## Completion accounting

During implementation, expand each row into behavior cases and record the fixture/test, parameter/default/type/version scope, reference observation, and known divergence. Parser acceptance, dispatcher registration, line coverage, and a playable KFM demo must remain separate from behavioral compatibility. No item is Verified in this snapshot.
