# Current handoff

Updated 2026-10-17. Implementation/local checkpoint commits are authorized. The
user wants continuous iteration without routine review requests. Never push.
Full MUGEN 1.1 beta1 engine/assets/modes remain unfinished. Do not stop at a
checkpoint or claim the whole port complete.

## Latest checkpoint: M4ay actor sampling

M4ay is verified and locally committed (reversal 931467ac, M4ax; see git log for
the checkpoint hash). Continue immediately with M4az RGB HUD/effects; there is
no permission dependency. Keep all authored changes under game/mugen/.

All 37 raster numeric programs pass:444 numbers/180 hashes/zero final rejections.
All 32 original crops (2,176,000 RGB pixels) and28 unchanged flipmark crops match
byte-for-byte, with no precision allowance. Six actual clothing masks match,
including all four M4ax failures and irregular400. The67 app runs/70 captures
share binary b0f0dd90433fcc6958594341d9dd37e886f8b394f80e5b579eb5d1d8d0f33d7b;
all source/config/binary/capture/runtime checks pass. Stock7s capture was viewed:
two healthy fighters, no lingering dust/effect clones. Six fresh native replays
pass, five including strict original crops. Full scoped races pass in
m4ay-full-race.log: Match 461.719s, Character 10.938s. Pure races and root module
integrity checks pass. Final pipeline session 24438 completed with exit0.

Renderer source: render/linear.go, nearest.go and quad.go; Player selects by
indexed source image. Unsnapped triangles/local custom UVs avoid rectangle/atlas
precision loss. RGB clamps edges and uses eight-bit weights plus byte-rounded
bilinear arithmetic. Indexed geometry uses+.5 common pixel centers, four-bit
source-address selection, ceil-minus-one for reversed axes and transparent guard
coverage on every side. The guard preserves biased edge samples without atlas
bleed. Read SPRITE-SAMPLING.md for measured rules and rejected models. Source
raster goldens are original generated RGB/RLE8/LZ5/fine/32x32 banks only; no
reference art or native framebuffer crops are committed.

The first LZ5 author used256 palette colors and exited during native rendering;
corrected32-color input works. Final fine-lz5 and rgb-fine-integer native runs
use native-fixed; all other raster native runs use native. Failed copies and
old raw/ceil/point4/coverage/padded/etc candidates remain diagnostic under temp.
Final accepted captures end go-m4ay-final. Never loosen raster or mask gates.

## Immediate M4az task: RGB HUD/effect consumers

The additional default HUD/world ramp check FAILS color SHA despite exact bounds.
Each 288-pixel core has256 differences, max channel 22. The pre-M4ay binary and
M4ay binary produce identical core SHA 3b6d1fda45f45a849ae8a8e0e26db45d0b985d174f23082d1c04f590bde7cf6a;
native is332a367881b47bfe3c93e477b7557aaf57d19530b1b50f31acc87f04cd4262c8.
Baseline proof is m4ay-hud-baseline-checks.json and app-runs/m4ay-video-hud-baseline.
Both HUD and the SuperPause effect still use nearest first-stage RGB filtering;
those files are unchanged by M4ay. Source locations: system/hud.go drawSprite,
system/effects.go drawEffects. Reuse the verified clamped RGB sampler while
preserving blending/tints/resource/cache behavior; measure before guessing.

Prepared temp/run-m4ay-hud-checks.py tests existing video-fit default, stage0,
window-stretch source copies against native exact HUD/world marker cores. It
stopped at default, so the latter two are unrun. Existing native input directories
are reference-runs/video-fit-hud-{default,stage0,window-stretch}/stock. Permanent
tests/video_render_probe.py provides measure(); original goldens are
 tests/testdata/reference/video-fit.json. Do not relax color hashes or claim
only geometry closes the gate. No GUI/audio client remains active; session 56208
(the baseline replay) exited0. Run read-only ps before the next GUI client.

After M4az, continue power. temp/author-hit-power.py is prepared but NEVER RUN;
it would write36 hitpower programs. temp/power-next-notes.txt records read-only
integration hypotheses, not native results. Get/GivePower lack runtime consumers;
Damage is literal-parsed, .7/.6 config rules are not passed into Match actors,
and helper/root sharing is unverified. Add precision/omission cases if useful,
record native behavior first, then implement. Do not mix unmeasured assumptions
with findings. The HITDEF-SCHEMA audit and PLAN/COMPATIBILITY retain broad gates.

## Scope and environment

Read ../AGENTS.md. All changes stay under game/mugen/. Preserve temp/mugen-1.1b1/
and root Go modules. Stage explicit authored files plus tracked MUGEN edits;
never stage temp, original assets/binaries, caches or captures. No agents, skill
or goal tool is active. Branch go-rewrite-mugen. Sandbox bwrap is broken; scoped
require_escalated calls have passed automatic review. No approval blocker exists.

Private display: DISPLAY=:97,
XAUTHORITY=/home/juvinious/projects/paintown/game/mugen/temp/virtual-display/Xauthority,
LIBGL_ALWAYS_SOFTWARE=1. Xvfb PID2146490. Private Pulse PID1393643:
PULSE_SERVER=unix:/home/juvinious/projects/paintown/game/mugen/temp/audio-reference/native;
PULSE_COOKIE points to that directory's cookie; ALSA_CONFIG_PATH to its alsa.conf.
Never print cookie contents, touch desktop/focus/keyboard or use host audio.

Native probes, actual apps/audio and full Ebiten suites run sequentially. Before
each new client, wait for the prior session then separately check:
ps -eo pid,stat,comm | rg 'mugen.exe|mugen-go|match.test|character.test'
Idle wineserver/winedevice is okay. Pure env -u DISPLAY tests can run alongside.

Full suite from workspace root with private display vars:
GOPROXY=off game/mugen/mugen --check -race
Redirect output to temp. Keep commentary updates during long tests. Scoped builds
use GOCACHE=.../game/mugen/temp/go-build, GOTMPDIR=.../game/mugen/temp/tmp,
GOFLAGS='-modfile=/home/juvinious/projects/paintown/game/mugen/mugen.mod -mod=readonly'.
Root module baseline: temp/root-module.before-font.sha256. Last free space~82G.

Historical notes are in archive/HANDOFF-2026-10-17-long.md; its active instructions
are obsolete. Prefer this handoff and current checkpoint documentation.
