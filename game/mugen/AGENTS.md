# MUGEN development instructions

## Scope and authorization

Author only game/mugen/ and preserve unrelated work. Local source/text commits
are authorized; do not push, commit binaries or user configuration, spawn
subagents or create goals. Keep mugen.yaml ignored. Continue through coherent
checkpoints without individual approval; recap about every ten to fifteen
commits, or sooner when input is needed. A recap is not an automatic stop.

## Portable documentation

Keep documentation, feature catalogs and handoffs useful to another contributor.
Do not record personal filesystem paths, private environment setup, local asset
or pack names, executable fingerprints, server/process identities, copied run
logs or inventories of unavailable test content. Describe general compatibility
patterns and implementation limits. Use placeholders for external game data.
Repository-relative source/tool references and public-source coverage
fingerprints remain valid project metadata.

Reference game content and runtimes are external prerequisites. A bundled
runtime may differ from the target despite a familiar version banner; separate
pack observations from verified target behavior. Paintown C++ turning is not
normative. A script defect reproduced in the target runtime is a content issue;
do not repair further third-party scripts or add engine workarounds for them.

## Implementation and validation

Use docs/PROGRESS.md as the SINGLE project completion ledger. Implement coherent
mechanics batches from actual gaps using existing evidence and target beta
documentation. Connect the production consumer before marking a route complete.
The ledger currently has 89 controller families, 127 trigger routes and 32/35
implemented project rows; route counts are not arbitrary native conformance.

The five connected segments cover state flow, combat/recovery, entity/binding/
retirement, effects/pause and complete match flow. Completed segments stay checked
until covered source changes, a regression identifies the segment, or a rerun is
explicitly requested. Validate affected segments at the end of a coherent batch;
do not run every legacy test after each parameter edit or create fixture matrices.
Extend existing state paths and keep handoffs short.

Use the scoped mugen.mod/mugen.sum overlay and preserve root module metadata.
Use a supported toolchain and discover the current display/audio environment;
do not assume another contributor's cached tools or private wrapper. Native,
application and display-dependent runs stay serial. Await owned clients before
edits or another run; do not reset shared services or expose credentials.

Reuse external data in place and a shared build cache. Clean disposable scratch
on success and failure. Preserve external assets, user configuration and
unrelated files; remove only output created for the current task. Never commit
generated test data, captures, recordings, copied distributions or binaries.
Optional historical diagnostics may require separately supplied inputs; they
are not routine completion gates.

Three broad areas remain partial: arbitrary content/font/media acceptance,
native runtime/render/audio/AI/menu fidelity and physical controller acceptance.
Physical CD playback, unavailable licensed/CJK fonts, other-platform runtime
acceptance and standalone music-file CLI playback are deferred. Do not turn
unknown permutations into mandatory parameter matrices.

## Implementation invariants

- Keep first character/motif Info/Files, CNS constants, Statedef attributes,
  persistent settings and ordinary controller parameters. Preserve repeated
  trigger conditions and legacy StateTypeSet's second Type parameter.
  Duplicate AIR actions keep the first lookup but still undergo validation.
  Legacy/v2 font metadata, stage metadata/resources and fight layout lookups
  also keep first fields. BG/controller fields are first within each instance;
  repeated BG names remain separate objects. Motif/storyboard metadata follows
  the same policy while scenes/AIR remain ordered. An authored blank private
  motif BG sprite is a resource error. All motif BGDef groups share first-field
  color/resource loading; victory drawing applies its clear color before art.
  Character Arcade fields and portrait sprites also retain first lookups.
  Retain primary-error locations.
- Preserve dimensional constants across redirects/borrowed states: recovery
  multipliers, friction and counters are unitless; velocities/offsets scale.
- Localize complete groups with English fallback, original source lines and
  stable scene order. Commands/states and individual BG elements are exempt.
  Standalone AIR stays undecorated; preserve runtime victory-quote banks.
  Motif-relative select.def precedes shared data; Arcade uses the same selected
  language and first Options fields. Confirmed roster entries retain their own
  presentation snapshots rather than being reselected by DEF identity.
- Accept escaped quotes in CNS debug formats without changing literal Windows
  resource names or native semicolon comment admission.
- CMD defaults apply globally; first settings/remaps/command attributes win.
  Keep time >=1, buffer 1–30, nonempty full-character command sets and original
  resource context. A disabled remap stays disabled; remaps read simultaneous
  physical snapshots. Declared command labels cannot fall back to held buttons.
- /x+y holds the entire chord; normal chords require button press edges. Held
  sequence endings complete on a fresh edge. Preserve release/charge windows.
  Histories size lazily from CMD durations; AI uses timed input steps, not one
  allocated frame per charge tick. Charge chord components before simultaneous
  release/press completion; strict steps retain unrelated preceding inputs. Player/helper buffers remain independent.
- Gamepads are opt-in through YAML standard-layout or explicit SDK raw controls.
  Keep two-player seat ownership across hotplug and non-consuming held/edge
  snapshots through menus/CNS. Native joystick numbers are not SDK raw indices;
  physical/native acceptance remains partial until verified on hardware.
- YAML key alternatives apply to menus and gameplay; omitted actions inherit,
  lists replace, and none/disabled/[] disable. Keep KeyA(0), shared-key ownership
  and value-copy isolation. User YAML remains ignored and untouched.
- Variable destination/range admission precedes value evaluation. Invalid IDs
  and invalid/reversed ranges are no-ops; valid ranges evaluate once. Target
  controllers check existence then ID, then apply numeric fallback; TargetBind
  checks ID before time/pos.
  ParentVarSet/Add require a live parent before evaluating their arguments.
- Helper bank admission precedes expressions. Forward deferred creation through
  stateEnvironment: convert position once after admission, leave size overrides
  in helper units, and retain issuer profile/banks/state programs. DestroySelf
  parameters still evaluate for roots in native; do not generalize no-op gates.
- Confirmed ordinary-bottom zero fallback covers movement, life/control/power,
  transitions/entry, animations, variables/ranges, Pause/SuperPause, protection,
  sound/palette commands and calibrated control/effect/combat scalars. Preserve
  omitted defaults and later argument evaluation. Real VM failures remain fatal
  with source context and no partly applied vector/power/pause effects. Other
  parameter policies require evidence; do not blanket-convert controller values.
- Invalid HitOverride slots select zero; durations below -1 clear that slot.
  Keep compiler/direct adapter normalization consistent. Nested fatal VM errors
  retain their identity and stop before later selector/bound/index expressions.
- Command reads see the old heading; controlled ground destination entry sees
  corrected facing/velocity. Walk/crouch turns use action5/6 without resetting
  Time. Preserve locked attack/air headings, NoAutoTurn, stage disable and paused
  commands; rematch current input without appending another history tick.
- Held guard keeps Ctrl=1 and its clock; it must not fall into walking/crouching
  each pass. Forward release and up-back jump remain available. InGuardDist
  includes enemy helpers/live projectiles after their owner goes idle; removal
  art is not an attack. Preserve explicit guard.dist and profile conversion.
- Common state45 owns air-jump startup/motion/animation. Require a fresh raw up
  edge, control, height and budget; low held input cannot activate later. Reset
  the per-flight budget in S/C before held guard can return early.
- Base attack/defence reach contact damage. Negative nonabsolute LifeAdd and
  TargetLifeAdd use receiver defence, including SuperPause; healing/absolute
  additions stay unscaled. Nonzero negative scaled additions have a one-point
  minimum; explicit zero stays zero. Native TargetLifeAdd ignores issuer attack.
- Hit-paused binding anchors place victims but transfer zero velocity. Preserve
  self-binding/Pause allowances, expiry, recovery and fresh round identities.
  PosFreeze suppresses velocity movement, not late body push. Keep pre-CNS
  clipping and vertical Clsn2 gating; projectile-owner corner push is excluded.
- Borrowed Explods retain creating-state geometry/private-pixel units and issuer
  banks through deferred admission, Pause/SelfState/expiry. Convert storage once;
  do not restore the obsolete extra CreateExplod adapter. Legacy velocity is an
  alias for vel, keeping KFM's incoming intro blocks visible.
- Keep ZIP expansions engine-owned: stop audio readers before deletion, close
  title music before retiring packs, and clean startup errors/signals. Reuse
  canonical media by links in scenarios. Portraits decode selectively; gameplay
  loads the bank. Keep resource bounds/context and corrected-content reloads.
- Preserve PCX padding/shared-palette handling, SFF pixel budgets/link charging,
  commented FNT maps and explicit glyph coordinates. TrueType uses the resource
  resolver and reported fallback. Lazy sprite caches share pixels, while fighter
  and HUD effects stay separate; palettes use MappingRevision for outfit changes.
- Retain linked/animated BG clocks and geometry, moving floors/reflections and
  camera return based on the unclipped participant midpoint. Storyboards retain
  clearcolor=-1 canvases and compose once per tick. Keep motif-aspect presentation,
  fitted top-left layer/window origins and viewport-sized surfaces without
  implicit BG masking. layerall.pos already supplies an authored center; do not
  add a BG-style half-width to layers. BG projection keeps its separate center. Native scene windows clip BGs too, despite the manual; preserve window
  inheritance. OnTop/HUD/global PalFX order
  and window/subimage origins stay intact.
- HD selection honors portrait.spr and both face.spr requests, including absent
  sprites; do not substitute defaults over authored screenpack art. Missing
  roster DEFs retain empty grid cells; corrupt present assets remain errors.
  SFF 2.01 retains authored palette alpha, including opaque index zero, through
  links/remaps. SFF 2.00 ignores its reserved alpha byte and masks index zero.
  AIR AI is native additive 256/256; sub is subtractive. HUD numbered backgrounds
  may be sparse; draw them numerically and retain first animation components.
- Parenthesized projectile contact IDs retain the legacy comparison/time
  grammar and != behavior; native Var(...) IDs select the unfiltered zero ID.
  Empty effect animations may have a zero-duration cycle: use the effect clock
  and retire at its first pass. Preserve strict fighter-loop diagnostics.
- Preserve indexed 3/8 game-pixel calibration without grid snapping, byte
  premultiplication, nearest indexed/linear true-color sampling and delta-first
  round-to-even interpolation. Keep AfterImage AIR clocks and saved mappings.
  Existing bounded RGB/KO-echo acceptance is not whole-renderer/audio parity.
- Debug restart/reload uses pre-state5900 carry/score snapshots, normal entity/
  effect/voice cleanup, stable round counters and continuing process time/RNG.
  Reload banks and presentation caches together; keep current input/AI ownership
  in copied live rosters and leave campaign templates/archive ownership intact.
- Debug-disabled roots retain identity/resources and AI ownership; hide/freeze
  them and exclude CNS peer queries, contacts, push and camera participation.
  Retain disabled slots through F4/reload; shared clocks/HUD keep running. PNG
  is the native screenshot format since 1.0 RC4; do not reopen a PCX gap for 1.1.
- Options must remain readable when a motif draws its title labels as art and
  omits menu fonts. Keep the fitted built-in fallback, authored BG/heading and
  explicit Back action. Enter adjusts values/opens actions; edits reach gameplay.
- Keep native font tuples' optional RGB multipliers (neutral 256) separate from
  bank selection and immutable glyph caches across menus, HUD and result text.
  Victory portrait windows clip after full-canvas scaling; inset quote wrapping
  uses the remaining aligned space. Screen holds include their exit fades.
- Campaigns use opponent stages without requiring a manual stage pool; a missing
  random pool is contextual failure. Resolve unused character/chained storyboards
  only on playback. Win/Survival result poses advance clocks without committing
  another result, decreasing the timer or starting a round.
- Options input edits remain session-only. Preserve release admission, inherited
  keyboard fallbacks, raw-axis neutral baselines and device-change rearming.
  Requested F12 captures use exclusive PNG files; never commit generated output.
- User Pause holds match/HUD/CNS GameTime while input/audio service continues.
  Fresh Scroll Lock advances one pass; AnyKeyUnpauses does not dismiss training
  menus. Keep manual/AI/cooperative dummy ownership through the shared input
  consumer. Native dummy spacing/jam timing and menu parity remain partial.
- Idle demos retain their own sequence/AI requests and consume interruption
  input before the returned title can act on it. Preserve demo bar/debug/music
  policy without changing user configuration. Title BG controllers advance.
- Font height overrides apply only to TrueType rasterization, keeping bitmap
  cells and source precedence. CFG first fields precede explicit YAML overrides.
  Preserve background Pause versus Mute cursor semantics and quick-stage defaults.
- Audio uses consumed cursors, shared WAV decoding and reusable Linux codec
  buffers. Retain SFX two-stage quantization and existing KO-echo scheduling.

Current behavior and remaining work are recorded in docs/STATE-MACHINE-FEATURES.json
and PROGRESS.md. Keep future handoffs portable and concise; use Git history for
code changes rather than accumulating per-run transcripts.
