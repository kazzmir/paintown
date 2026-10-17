# Helpers: configuration and shared scheduling

M4aj compares 72 authored programs against the supplied 1.1 beta executable:
984 numeric observations, eight intentional load rejections and 276 authored
CNS/AIR/stage/CMD hashes. TestNativeHelperConfiguration runs the same inputs
through Match. These are focused controls, not complete helper coverage.

## Creation and state passes

Normal helpers skip -3/-2. Both normal and player helpers run their root's -1
only when keyctrl is nonzero. The beta still executes player-helper -3/-2,
despite the manual's unsupported/deprecated label. Positive-state entry
attributes apply at birth. The init-omitted packet pins Ctrl=1, PrevStateNo=0,
Life=1000, Power=0, fresh variables and the creator's animation. An omitted or
negative initial state enters 0. A missing positive state is accepted with a
warning and retained animation/control properties. Negative helper IDs clamp
at zero.

Roots and live helpers now share the ordered tick list. Existing attackers
precede idle entities; creation order breaks ties. New helpers join the end of
the current pass rather than running recursively inside the creator. A nested
three-helper control distinguishes creation order from depth-first recursion.
Birth CNS sees the completed parent's/root's Time=1. Helper families include
nested descendants and the querying helper itself. Neither normal nor player
helpers add to NumEnemy/NumPartner in the one-on-one controls.

Each helper owns its state machine, variables, pause actor and command buffer;
immutable state programs remain shared. Keyctrl=0 suppresses command polling.
Positive physical input, command history/facing and foreign inheritance still
need comparison. Name/P1Name/AuthorName identify the original DEF independently
of the custom display name; string comparisons are case insensitive. Helper
name accepts quoted, empty and unquoted input.

## Typed arguments and geometry

Arguments compile before effects and evaluate in the creating state context.
Tested integer float literals reject, computed values coerce, and bottom is
consumed as zero. A single pos supplies Y=0; a third component rejects. Postype
recognizes initial letters (bad means back, p3 means p1) and rejects quoted
tokens. Helpertype requires unquoted normal/player. Only facing=-1 reverses;
-2, 0 and 2 use the default orientation.

Postype p1 uses the creator's axis; p2 uses the opponent's current axis. Edge
postypes use floor-relative Y, contrary to the manual's general parent-relative
Y claim. Right-edge placement subtracts one common-space pixel. Left/right use
absolute facing; front/back/p1 use creator facing; p2 uses opponent facing.
Controls include both creator orientations, nonzero Y and a nonzero camera.

All thirteen size values have native defaults/override controls: x/y scale,
ground/air front/back, height, projectile scaling, head/mid positions and shadow
offset. Overrides copy helper constants without changing the parent. Measured
zero/negative scales and negative sizes are retained. Collision and sprite
drawing consume entity scale. Helper/projectile Y drawing now follows the same
positive-down convention as roots. Full foreign size conversion and every
collision/push boundary remain open.

## Pause and camera

Helper pausemovetime/supermovetime count permitted paused ticks, not ordinary
unpaused lifetime. Clocks are independent; negative values act as zero. Delayed
and immediate four-tick controls distinguish allowance from birth age. Helper-
owned pauses, paused creation and combined interactions remain open. A rejected
nested-redirect draft stays only in temp; accepted pause programs read the
independently established first helper entity ID 58.

Player helpers are confined at creation, before their first CNS. Normal helpers
default to no confinement/camera participation. New helpers begin participation
at their own first pass. Every completed helper pass adds camera easing, even
when excluded from tracking. Far normal/player/enabled controls distinguish
participation from update count.

Camera endpoints and deltas round independently to float32. A measured maximum-
span correction precedes center/zoom publication. Tiny zoom rounding affects
ScreenPos and edge queries; division retains double precision until the final
script value. See CAMERA.md. This supersedes M4ag's single-endpoint precision
model; full vertical/1.1 zoom behavior remains open.

## Intro flow discovered by helper controls

Native pre-intro changes RoundState 0 to 1 at GameTime 30. Intro requires ten
consecutive ticks with no asserted intro flag. RoundState 2 starts at the fight
announcement (GameTime 100 for empty intro); control begins at 130. Five/twenty
active assertion ticks shift those boundaries to 105/135 and 120/150. Go had
exposed RoundState 2 early and granted control at 121. Match now reproduces all
five flow packets. Fixture 5900 explicitly enters 0, so these results do not
establish automatic state 190 entry or complete round/end/skip timing.

## Evidence and remaining work

Inputs/goldens: tests/testdata/reference/helpercfg-*. Disposable recordings:
temp/reference-runs/helpercfg-*; corrected pause programs use native-flow-final.
Authors/catalogues: temp/author-helpercfg*.py and helpercfg-*-cases.json.
Native CNS -1 shadows CMD -1. The replay runner checks CMD hashes and merges
sidecars with that same file priority.

Initial failures remain in temp/m4aj-helper-first-race.log and
m4aj-helper-second-race.log. First 65 corrected programs passed in
m4aj-helper-third-race.log. Final integration/window/replay results are recorded
with the checkpoint in CHECKPOINTS.md.

Still required: contacts against opposing helpers, multi-contact priority/push,
recursive/orphan/binding/effect/sound retirement, ownpal/remappal RGB, positive
input, foreign ownership, paused creation and complete world phases. Projectiles
remain legacy adapters. No whole helper/controller gate is declared complete.

Real 1280x720 windows at temp/app-runs/helper-size-go/ (2/4/7 seconds) and
helper-stock-throw-go/ (2/3/4/7) pass all seven capture hash/dimension/nonblack
checks, with no runtime/missing-resource errors. The scaled helper's white
clothing occupies exactly [643,401,943,499) in both native and Go, with 10,381
thresholded pixels in each. This pins the measured 2.5/.5 scale and nonzero Y;
it is not full framebuffer equivalence. Native debug magenta versus Go black
outside the minimal stage, background filtering and other rendering rules
remain open. The inspected stock recovery has two fighters and no dust clones.
