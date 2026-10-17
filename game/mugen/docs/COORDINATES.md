# Coordinate spaces

The supplied [coordinate-space specification](../temp/mugen-1.1b1/docs/coordspace.html)
defines a common game space through Config.GameWidth/GameHeight. Characters and
motifs use Info.localcoord; stages use StageInfo.localcoord. Vector translation
uses the ratio of target width to source width for both axes, even when their
aspect ratios differ. The default local space is 320x240.

`coord.Space` retains dimensions and performs width-ratio translations. Loaders
preserve character, motif, stage and game profiles with contextual errors. Native
probes show fractional widths are accepted and truncated: 320.5 behaves as 320,
and 640.5 as 640. The parser applies integer coercion to both dimensions; height
rounding has not been isolated separately. Zero/nonfinite/out-of-range resulting
dimensions are rejected.

Character environments bind Const240p/Const480p/Const720p to local width. They
always return float32, propagate bottom and reject bad arity before compiled
controller effects. Base widths are 320, 640 and 1280. Five source/hash-pinned
native cases cover standard spaces and fractional widths, float division and
fractional/negative arguments. Helpers/projectiles inherit their creator's
profile; their complete ownership/lifecycle behavior remains separate work.

Production menus use the motif's declared logical dimensions. Background default
windows use that profile, and drawing uses the actual logical horizontal center.
The physical window honors Video.Width/Height without the former implicit x2.
The high-resolution title was inspected in temp/app-runs/hd-menu-centered/.

Separate-player native probes now pin 1280/320 and 320/1280 pairings. The
querying player sees its own local units: start -280/-70, P2Dist X 560/140,
and P2BodyDist X 480/120. Body distance compares body fronts rather than axes;
Y remains an axis distance. All 24 captured fvars match the Character fixture.

Cross-player position queries, guard range/facing, collision boxes and push
movement now translate the other player. Authored geometric tests verify equal
physical extents and displacement across spaces. Push/collision timing and
corner rules, dynamic Width overrides, full airborne cases and query redirection
remain open. M4x separates Opponents from an acquired-target ledger and adds
the match identity registry. Complete HitDef target/state ownership remains
open; see ENTITIES.md.

Received hit vectors now translate with float32 rounding into the defender's
local units. Six native contact cases cover explicit/default acceleration/fall
values and unchanged entry velocity. Source vectors stay immutable. Omitted
fall X is a no-change sentinel reported as -MaxFloat32 times local/game width;
it can overflow to negative infinity. Match supplies the common profile and
helpers/projectiles inherit it. Applying the no-change bounce itself is pending.

Production Match spawn, camera metadata, character/HUD portrait drawing and
stage drawing use explicit profiles. Exact stage overdraw/filtering,
zoom, mixed ownership, hit/guard selection and all storyboard spaces still need
reference-backed conversion and timing. These tests are not full mixed-resolution
gameplay parity.

M4k native guard range is attacker-owned/directional, ignores receiver facing/Y,
retains explicit overrides until normal state entry, and scales across both
mixed-profile directions. Body push near axes adds max(0,10-abs(separation)) in
240p units before splitting the overlap correction. Ordinary/fractional/signed/
cutoff and mixed cases are pinned; see REFERENCE.md. PlayerPush disables a pair
for the tick. Complete airborne/corner/Width/pause behavior remains open.

## M4q viewport and language-version observations

Seven accepted native viewport profiles pin 168 float32 observations. In the
measured quick matches the common game width follows Config.GameWidth, while
height follows the stage aspect, including a stage explicitly changed to 1.1.
Changing configured height alone does not change that match space. Character
conversion still follows local width: 1280x720 local characters see GameHeight
960 on the stock 320x240 stage. The 320x180 HD stage instead gives local height
180 to a 320-wide character. These observations supersede interpreting configured
GameHeight as the match height regardless of stage. Broader stage/zoom cases
remain open.

The shared viewport binds GameWidth/Height, ScreenWidth/Height, ScreenPos X/Y,
CameraPos X/Y and CameraZoom as float32 values in the querying character's units.
ScreenPos reads current position immediately: a PosSet to X=.5 returns 160.5 in
the same state pass, before body push changes the following tick's position.
Native legacy probes accept GameWidth/Height, ScreenPos and CameraPos for a 1.0
character, but reject ScreenWidth, ScreenHeight and CameraZoom. The loader now
retains character language version and compiles nonempty states before gameplay;
typed parameters, entry expressions and cached triggers enforce those three
measured gates. Other version rules and legacy parameter handlers remain open.

M4q introduced separate world/HUD fits. M4v identified that the HUD fit remained
incorrect; M4w below replaces it with the measured shared viewport transform.
A 320x240 stage at 1280x720 occupies X=160..1120. Actors/effects draw at common
game resolution, avoiding an intermediate 320-wide actor raster. Backgrounds
still use the stage raster. The owned-window
capture temp/app-runs/viewport-spark-hd/ was inspected at the actual requested
1280x720 size; metadata pins the binary/capture. It confirms corrected framing,
but spark filter-edge pixels differ from native. The earlier attempt at
viewport-spark-production/ stayed 320x240 because commented window settings were
not overridden; it is diagnostic only. The smoke harness now sets explicit YAML
dimensions and rejects mismatching capture sizes.

All native viewport scenes have zero camera displacement and zoom one. Moving
camera/zoom timing, full screen bounds, stage raster/filter fidelity and complete
framebuffer comparisons are not established by this checkpoint.

## M4w fight viewport fitting and crop allocation

Ten native cases pin 240 numeric values and twenty original RGB marker cores,
one each for HUD and world per case. StageFit=1 keeps the stage-aspect game
height. StageFit=0 uses configured GameWidth/Height: the stock 320x240 stage
with 1280x720 configuration reports local GameHeight/ScreenHeight=180 and
CameraPos Y=35. ScreenPos Y becomes 165 while Pos Y stays zero. SystemFit=0
leaves both script coordinates and fight marker projection unchanged in these
scenes; menu/system fitting is a separate remaining obligation.

The top cut is truncated in stage units, then converted by width ratio. Within
cuthigh+cutlow, it divides the crop by that ratio. Beyond the guidelines the
low cut stays fixed and the excess is removed from the top. KFM's 35/25 guides
produce top cuts 8, 23, 28, 35, 55, 65 at local viewport heights 225, 200,
191.25, 180, 160, 150. The crop contributes to script CameraPos and is applied
uniformly to the stage raster, separately from background delta movement.
Match retains it through normal camera updates and round resets.

Fight HUD coordinates use motif-width conversion followed by the game viewport's
physical transform, with the same top origin. They do not independently center
the motif rectangle. The authored HUD marker at (1152,64) appears at (1024,48)
with StageFit=1, and (1152,64) with StageFit=0 at 1280x720. KeepAspect=1 fits the
effective viewport directly into the actual window; KeepAspect=0 stretches X/Y
independently. At a 960x600 window these give HUD positions (800,40) versus
(864,40), while script coordinates remain unchanged. Fight Layout now uses the
actual window size instead of first imposing motif aspect.

Video flags load from CFG, with presence-aware stage_fit/system_fit/keep_aspect
YAML overrides. Absent fullscreen/vsync YAML flags now also preserve CFG values.
The launcher's default window dimensions follow configured game resolution.
RenderMode is retained; System/DirectX renderer differences, forced fit behavior,
omitted/invalid native defaults, menu/SystemFit projection, tall overdraw,
different cut guidelines, moving cameras/zoom and full raster/filter parity
remain open. These ten cases do not establish all video/stage schemas.

Full scoped races and pure coordinate races pass. Fresh crop765/window-stretch
native replays reproduce packets and marker hashes. Actual Go captures at
temp/app-runs/video-default-go, video-crop-go and video-stretch-go have identical
HUD cores and matching world centers. The Go world core is smaller by filtered
edge pixels: e.g. native [505,450,529,462] versus Go [507,452,527,460]. This is a
remaining renderer issue, not a reason to change the established projection.
Inspection hashes/measurements are in temp/app-runs/video-inspection.json.
The actual mixed KFM/KFM720 stock fight at video-stock-hud-go/ was also inspected;
portraits now share the stage margins and measured native HUD scale.
The initial 1280x960 physical-window experiment could not read its clipboard
completely on the isolated display and is diagnostic only; promoted windows
fit inside it. Original reference assets and desktop settings remain unchanged.

M4y now compares direct redirected geometry in caller units, eager origin
arguments and delayed Cond destination units. Geometric Const kinds are
preserved, while raw scalar variables/IDs stay unscaled. Mixed Target velocity
controllers convert caller widths into victim widths. See TARGETS.md for the
48 scope values and controller traces; fractional profiles and complete camera/
zoom/constant metadata remain open.
