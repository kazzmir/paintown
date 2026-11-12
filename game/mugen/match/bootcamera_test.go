package match

import (
	"fmt"
	"math"
	"os"
	"path/filepath"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"github.com/kazzmir/paintown/game/mugen/stage"
)

// Both fighters save their boot and first two controller-pass camera queries.
// These are camera publication controls, independent of system-effect producers.
func TestCameraStartupViewportAndResetFlow(t *testing.T) {
	// Reuse the four retained startup observations, including P2's constrained
	// first pass after P1 publishes the negative camera. No native run is made.
	replayNativeGeometryPrograms(t, "bootcamera-*.json", 4)

	base := stockMatch(t, 99)
	options := base.Options
	options.StageCrop = true
	m, err := New(base.Request, options)
	if err != nil {
		t.Fatal(err)
	}
	program := `[Statedef 5900]
type=S
movetype=I
physics=N
[State 5900, enter]
type=ChangeState
trigger1=1
value=0
[Statedef 0]
type=S
movetype=I
physics=N
anim=0
ctrl=0
velset=0,-1
[State 0, fixed camera]
type=ScreenBound
trigger1=1
value=0
movecamera=var(50),var(51)
[State 0, temporary camera width]
type=Width
trigger1=var(49)
edge=20,30
`
	for index, expression := range []string{"TopEdge", "BottomEdge", "ScreenPos Y", "Pos Y", "GameHeight", "EnemyNear, TopEdge", "EnemyNear, BottomEdge", "CameraPos Y", "CameraPos X", "CameraZoom", "ScreenPos X", "Pos X", "GameWidth"} {
		program += fmt.Sprintf("[State 0, viewport %d]\ntype=VarSet\ntrigger1=1\nfv=%d\nvalue=%s\n", index, index, expression)
	}
	for _, player := range m.Players {
		file, err := cns.ParseNamed(strings.NewReader(program), "connected-camera.cns")
		if err != nil {
			t.Fatal(err)
		}
		file.SetLanguageVersion("1.1")
		constants := player.Character.StateFile
		file.Data, file.Size, file.Movement = constants.Data, constants.Size, constants.Movement
		player.Character.StateFile, player.CMD = file, nil
	}
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	for slot, player := range m.Players {
		player.Character.Y = float64(-30 * (slot + 1))
		// Both roots are fixed-camera participants from the outset. Their CNS
		// renews this choice, preserving a shared zoom for mixed local spaces.
		if err := player.Character.SetScreenBound(engine.ScreenBoundParameters{}); err != nil {
			t.Fatal(err)
		}
	}
	fixedViewport := true
	step := func() {
		t.Helper()
		zoom, cameraY, shake := float32(m.Viewport.Zoom), float32(m.Viewport.CameraY), float32(m.Timeline.ShakeOffset())
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
		for slot, player := range m.Players {
			c := player.Character
			v := c.FloatVars
			scale := float32(c.LocalCoordinateWidth()) / float32(m.Viewport.Space.Width)
			if float32(v[0]) != float32(v[3])-float32(v[2]) || float32(v[1]) != float32(v[0])+float32(v[4]) {
				t.Fatalf("player %d: edge/position identities lost: %v", slot+1, v[:7])
			}
			if !fixedViewport {
				continue // Zoom can be republished between the two root passes.
			}
			ground := m.Viewport.GroundY + (float64(zoom)-1)*m.Viewport.CropY
			wantTop := cameraY*scale - float32(ground)*scale/zoom - shake*float32(c.LocalCoordinateWidth())/320
			wantHeight := float32(m.Viewport.Space.Height) * scale / zoom
			if float32(v[0]) != wantTop || float32(v[4]) != wantHeight {
				t.Fatalf("player %d: crop/zoom/shake: top=%v height=%v want=%v/%v", slot+1, v[0], v[4], wantTop, wantHeight)
			}
			crop := float32(m.Viewport.CropY) * scale
			if float32(v[7]) != (cameraY*scale-crop)*zoom+crop {
				t.Fatal("CameraPos Y lost post-zoom translation", v[7], cameraY, zoom)
			}
			opponent := m.Players[1-slot].Character.FloatVars
			if v[5] != opponent[0] || v[6] != opponent[1] {
				t.Fatalf("player %d: redirected edges lost target coordinate space", slot+1)
			}
		}
	}
	step()
	m.CameraY, m.Viewport.CameraY, m.Viewport.Zoom = 200, 200, 2
	m.Timeline.RequestShake(engine.ShakeParameters{Time: 20, Amplitude: 3, Phase: 90})
	step() // Installs the shake after the issuing pass.
	step() // Moving roots see the same stage edges in their own local spaces.
	if err := m.Players[0].Character.PauseGame(engine.PauseParameters{Time: 2}); err != nil {
		t.Fatal(err)
	}
	step()
	before := [2][7]float64{}
	for slot, player := range m.Players {
		copy(before[slot][:], player.Character.FloatVars[:7])
	}
	for tick := 0; tick < 2; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
		for slot, player := range m.Players {
			if [7]float64(player.Character.FloatVars[:7]) != before[slot] {
				t.Fatal("paused state pass changed viewport observations")
			}
		}
	}
	step()
	m.Round++
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	if m.Viewport.Zoom != 1 || m.Viewport.CameraY != m.Viewport.CropY || m.Timeline.ShakeOffset() != 0 {
		t.Fatal("round reset retained zoom, camera offset or shake")
	}
	if err := m.Update(); err != nil { // The reset initialization frame skips CNS.
		t.Fatal(err)
	}
	step()

	// Continue into the 1.1 edge-tension camera using the real stage loader.
	// These settled positions come from one original-beta jump/return flow;
	// actors retain their different 320p/1280p local coordinate spaces.
	path := filepath.Join(t.TempDir(), "vertical-camera.def")
	stageSource := fmt.Sprintf(`[Info]
name=Connected vertical camera
mugenversion=1.1
[Camera]
boundleft=-400
boundright=400
boundhigh=-180
boundlow=0
tension=60
tensionhigh=80
tensionlow=40
verticalfollow=0
floortension=1000
[PlayerInfo]
p1startx=-40
p2startx=40
p1facing=1
p2facing=-1
leftbound=-1000
rightbound=1000
[Bound]
screenleft=15
screenright=15
[StageInfo]
localcoord=320,240
zoffset=200
[BGDef]
spr=%q
[BG Anchor]
type=dummy
`, filepath.Join(options.RootDir, "stages/kfm.sff"))
	if err := os.WriteFile(path, []byte(stageSource), 0600); err != nil {
		t.Fatal(err)
	}
	loaded, err := stage.LoadStage(path, options.RootDir)
	if err != nil {
		t.Fatal(err)
	}
	if loaded.Camera.TensionHigh == nil || loaded.Camera.TensionLow == nil || *loaded.Camera.TensionHigh != 80 || *loaded.Camera.TensionLow != 40 {
		t.Fatal("vertical tension parameters were not loaded")
	}
	m.Stage = loaded
	fixedViewport = false
	m.Options.GameSpace = coord.Default()
	m.Viewport, err = coord.StageViewport(coord.Default(), loaded.StageInfo.LocalCoord, true, 0, 0)
	if err != nil {
		t.Fatal(err)
	}
	m.Round++
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	if err := m.Update(); err != nil { // round initialization frame
		t.Fatal(err)
	}
	advanceUntil(t, m, Active, 650) // The fight-start placement reset precedes the scripted jumps.
	place := func(x0, x1, y0, y1 float64) {
		for side, p := range m.Players {
			c := p.Character
			scale := float64(c.LocalCoordinateWidth()) / 320
			c.X, c.Y = []float64{x0, x1}[side]*scale, []float64{y0, y1}[side]*scale
			c.VelX, c.VelY = 0, 0
			c.IntVars[50], c.IntVars[51] = 1, 1
		}
		for tick := 0; tick < 80; tick++ {
			step()
		}
	}
	position := func(y0, y1 float64) { place(-40, 40, y0, y1) }
	for _, pose := range [][3]float64{{0, 0, 0}, {-200, 0, -80}, {-200, -200, -80}, {-100, -100, 0}, {-400, -400, -180}, {0, 0, 0}} {
		position(pose[0], pose[1])
		if m.CameraY != pose[2] {
			t.Fatal("native vertical tension/priority/return/bounds", pose, m.CameraY)
		}
	}
	// The same flow with vertical zoom: native visible height fits the player
	// span PLUS stage-unit margins. CameraPos Y is the zoomed translation.
	high, low := 70.0, 30.0
	loaded.Camera.TensionHigh, loaded.Camera.TensionLow = &high, &low
	loaded.Camera.ZoomOut = .5
	position(-200, 0)
	if math.Abs(m.Viewport.Zoom-.8) > .001 || math.Abs(m.CameraY*m.Viewport.Zoom-(-16)) > .02 {
		t.Fatal("native vertical zoom/camera coupling", m.CameraY, m.Viewport.Zoom)
	}
	for _, p := range m.Players {
		factor := 320 / float64(p.Character.LocalCoordinateWidth())
		if math.Abs(p.Character.FloatVars[7]*factor-(-16)) > .02 {
			t.Fatal("native post-zoom CameraPos Y trigger", p.Character.FloatVars[7])
		}
	}
	position(-200, -200)
	if math.Abs(m.Viewport.Zoom-1) > .001 || math.Abs(m.CameraY-(-70)) > .02 || math.Abs(m.Players[0].Character.FloatVars[2]-70) > .02 {
		t.Fatal("native shared-height zoom return/ScreenPos Y", m.CameraY, m.Viewport.Zoom)
	}
	if err := m.Players[0].Character.PauseGame(engine.PauseParameters{Time: 2}); err != nil {
		t.Fatal(err)
	}
	step()
	pausedCamera := m.CameraY
	for tick := 0; tick < 2; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
		if m.CameraY != pausedCamera {
			t.Fatal("paused camera drifted with stationary participants")
		}
	}
	// An ineligible high actor must not influence the remaining participant.
	m.Players[0].Character.IntVars[51] = 0
	m.Players[0].Character.Y = -500
	m.Players[1].Character.Y = 0
	for tick := 0; tick < 80; tick++ {
		step()
	}
	if m.CameraY != 0 || math.Abs(m.Viewport.Zoom-1) > .001 {
		t.Fatal("ScreenBound movecamera=0 retained vertical influence")
	}
	// Omitting either tension parameter retains legacy follow/floor behavior.
	loaded.Camera.TensionLow = nil
	loaded.Camera.VerticalFollow, loaded.Camera.FloorTension = .5, 20
	position(-200, 0)
	if m.CameraY != -90 {
		t.Fatal("legacy verticalfollow/floortension fallback changed", m.CameraY)
	}

	// Continue across horizontal tension, zoom return and both zoom-dependent
	// stage-edge limits. One beta flow gives the settled camera/zoom pairs
	// 0/.761905, 70/.761905, 20/1, 0/.5, 560/1 and 400/.5.
	loaded.Camera.TensionHigh, loaded.Camera.TensionLow = nil, nil
	loaded.Camera.VerticalFollow = 0
	for index, pose := range [][4]float64{
		{-150, 150, 0, 16.0 / 21}, {-80, 220, 70, 16.0 / 21},
		{-80, 0, 20, 1}, {-300, 300, 0, .5},
		{700, 720, 560, 1}, {100, 700, 400, .5},
	} {
		place(pose[0], pose[1], 0, 0)
		if math.Abs(m.CameraX-pose[2]) > .02 || math.Abs(m.Viewport.Zoom-pose[3]) > .001 {
			t.Fatal("native horizontal tension/zoom/return/bounds", pose, m.CameraX, m.Viewport.Zoom)
		}
		for _, p := range m.Players {
			v := p.Character.FloatVars
			factor := 320 / float64(p.Character.LocalCoordinateWidth())
			if math.Abs(v[8]*factor-pose[2]) > .02 || math.Abs(v[9]-pose[3]) > .001 || math.Abs((v[10]-v[11]-v[12]/2)*factor) > .001 {
				t.Fatal("mixed-coordinate horizontal camera/zoom/position queries", pose, v[8:13])
			}
		}
		if index < 3 && math.Abs(m.Players[0].Character.FloatVars[10]-60) > .02 {
			t.Fatal("native horizontal ScreenPos X", pose, m.Players[0].Character.FloatVars[10])
		}
	}
	// Camera-disabled outliers cannot hold a wide zoom or force horizontal travel.
	m.Players[0].Character.X = 0
	m.Players[1].Character.X = 5000 * float64(m.Players[1].Character.LocalCoordinateWidth()) / 320
	m.Players[1].Character.IntVars[50] = 0
	for tick := 0; tick < 80; tick++ {
		step()
	}
	if math.Abs(m.CameraX-100) > .02 || math.Abs(m.Viewport.Zoom-1) > .001 {
		t.Fatal("ScreenBound movecamera=0 retained horizontal influence", m.CameraX, m.Viewport.Zoom)
	}
	// Reset publishes the same fixed stage extents to projectile retirement,
	// even though the initial camera starts zoomed in.
	m.Round++
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	if m.World.ProjectileStageBounds != [2]float64{-720, 720} || m.Viewport.Zoom != 1 {
		t.Fatal("zoomed stage extents or startup zoom lost on reset", m.World.ProjectileStageBounds, m.Viewport.Zoom)
	}

	// Offscreen movement and camera fitting have separate bounds. One beta
	// flow leaves the actual roots outside the stage while limiting the view
	// and incorporating temporary Width edge bars, not containment margins.
	loaded.Camera.BoundLeft, loaded.Camera.BoundRight = -100, 100
	loaded.Camera.StartZoom = .5
	m.Round++
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	for _, p := range m.Players {
		if err := p.Character.SetScreenBound(engine.ScreenBoundParameters{}); err != nil {
			t.Fatal(err)
		}
	}
	advanceUntil(t, m, Active, 650)
	for phase, pose := range [][4]float64{{-1000, 1000, 0, .5}, {700, 1300, 260, 1}, {0, 1300, 165, 32.0 / 51}} {
		for side, p := range m.Players {
			c := p.Character
			factor := float64(c.LocalCoordinateWidth()) / 320
			c.X, c.Y, c.VelX, c.VelY = pose[side]*factor, 0, 0, 0
			c.IntVars[49], c.IntVars[50], c.IntVars[51] = 0, 0, 0
			if phase > 0 {
				c.IntVars[50], c.IntVars[51] = 1, 1
			}
			if phase == 2 {
				c.IntVars[49] = 1
			}
		}
		for tick := 0; tick < 80; tick++ {
			step()
		}
		if math.Abs(m.CameraX-pose[2]) > .02 || math.Abs(m.Viewport.Zoom-pose[3]) > .001 {
			t.Fatal("native offscreen/Width camera fitting", phase, m.CameraX, m.Viewport.Zoom)
		}
		for side, p := range m.Players {
			factor := 320 / float64(p.Character.LocalCoordinateWidth())
			if p.Character.X*factor != pose[side] || math.Abs(p.Character.FloatVars[8]*factor-pose[2]) > .02 {
				t.Fatal("camera fitting constrained an offscreen root or lost its coordinate space", phase, side, p.Character.X, p.Character.FloatVars[8])
			}
		}
	}
	if err := m.Players[0].Character.PauseGame(engine.PauseParameters{Time: 2}); err != nil {
		t.Fatal(err)
	}
	step()
	pausedX, pausedZoom := m.CameraX, m.Viewport.Zoom
	for tick := 0; tick < 2; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
		if m.CameraX != pausedX || m.Viewport.Zoom != pausedZoom {
			t.Fatal("paused offscreen camera drifted")
		}
	}

	// The beta's omitted-version legacy stage still publishes fractional X/Y
	// queries. Whole-stage-unit rendering must not quantize the CNS camera.
	// Its 640p stage converts to the same 320p game with 320p/1280p actors.
	legacySource := strings.NewReplacer(
		"mugenversion=1.1\n", "", "localcoord=320,240", "localcoord=640,480",
		"zoffset=200", "zoffset=400", "tension=60", "tension=120",
		"boundleft=-400", "boundleft=-800", "boundright=400", "boundright=800",
		"boundhigh=-180", "boundhigh=-360", "floortension=1000", "floortension=0",
		"verticalfollow=0", "verticalfollow=.5", "tensionhigh=80\n", "", "tensionlow=40\n", "",
	).Replace(stageSource)
	if err := os.WriteFile(path, []byte(legacySource), 0600); err != nil {
		t.Fatal(err)
	}
	m.Stage, err = stage.LoadStage(path, options.RootDir)
	if err != nil {
		t.Fatal(err)
	}
	m.Round++
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	advanceUntil(t, m, Active, 650)
	for _, pose := range [][3]float64{{140.75, 220.75, 120.75}, {-220.75, -140.75, -120.75}, {140.25, 220.25, 120.25}} {
		place(pose[0], pose[1], -41.5, -41.5)
		if math.Abs(m.CameraX-pose[2]) > .001 || m.CameraY != -20.75 {
			t.Fatal("legacy fractional camera queries were rounded", pose, m.CameraX, m.CameraY)
		}
		for _, p := range m.Players {
			factor := 320 / float64(p.Character.LocalCoordinateWidth())
			if math.Abs(p.Character.FloatVars[8]*factor-pose[2]) > .001 || p.Character.FloatVars[7]*factor != -20.75 {
				t.Fatal("legacy fractional CNS camera changed across local coordinates", p.Character.FloatVars[7:9])
			}
		}
	}
	m.Round++
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	if m.CameraY != 0 || m.Viewport.Zoom != 1 {
		t.Fatal("vertical camera state survived round reset")
	}
	cameraZoomMotionFlow(t, base)

}

// Continue through zoom motion using one beta spread/translate/close/bounds/
// return sequence. Fixed world poses remove camera-relative PosSet feedback.
func cameraZoomMotionFlow(t *testing.T, base *Match) {
	t.Helper()
	request, options := base.Request, base.Options
	request.Teams = [2]TeamSelection{}
	request.Players[1] = request.Players[0]
	request.Players[1].Palette = 2
	options.GameSpace = coord.Default()
	m, err := New(request, options)
	if err != nil {
		t.Fatal(err)
	}
	camera := &m.Stage.Camera
	camera.StartX, camera.StartY = 0, 0
	camera.BoundLeft, camera.BoundRight = -400, 400
	camera.BoundHigh, camera.BoundLow = 0, 0
	camera.Tension, camera.VerticalFollow, camera.FloorTension = 60, 0, 0
	camera.TensionHigh, camera.TensionLow = nil, nil
	camera.StartZoom, camera.ZoomIn, camera.ZoomOut = 1, 1, .5
	const program = `[Statedef 5900]
type=S
movetype=I
physics=N
[State 5900, start]
type=ChangeState
trigger1=1
value=0
[Statedef 0]
type=S
movetype=I
physics=N
anim=0
ctrl=1
velset=0,0
[State 0, active]
type=ChangeState
trigger1=RoundState=2
value=600
[Statedef 600]
type=S
movetype=I
physics=N
anim=0
ctrl=0
velset=0,0
[State 600, settle]
type=ScreenBound
trigger1=1
value=0
movecamera=1,1
[State 600, position]
type=PosSet
trigger1=1
x=ifelse(TeamSide=1,-40,40)-CameraPos X
y=0
[State 600, ready]
type=ChangeState
trigger1=Time=90
value=700
[Statedef 700]
type=S
movetype=I
physics=N
anim=0
ctrl=0
velset=0,0
[State 700, camera]
type=ScreenBound
trigger1=1
value=0
movecamera=1,1
[State 700, no push]
type=PlayerPush
trigger1=1
value=0
[State 700, location]
type=PosSet
trigger1=1
x=ifelse(Time<20,ifelse(TeamSide=1,-150,150),ifelse(Time<40,ifelse(TeamSide=1,-80,220),ifelse(Time<60,ifelse(TeamSide=1,-80,0),ifelse(Time<80,ifelse(TeamSide=1,-300,300),ifelse(Time<100,ifelse(TeamSide=1,700,720),ifelse(TeamSide=1,-40,40))))))-CameraPos X
y=0
[State 700, observe x]
type=VarSet
trigger1=1
fv=0
value=CameraPos X
[State 700, observe zoom]
type=VarSet
trigger1=1
fv=1
value=CameraZoom
`
	for _, p := range m.Players {
		file, err := cns.ParseNamed(strings.NewReader(program), "camera-motion.cns")
		if err != nil {
			t.Fatal(err)
		}
		file.SetLanguageVersion("1.1")
		previous := p.Character.StateFile
		file.Data, file.Size, file.Movement = previous.Data, previous.Size, previous.Movement
		p.Character.StateFile, p.CMD = file, nil
	}
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	observations := map[int][2]float64{
		1: {-2.3465, .96929}, 10: {-.58743, .799}, 21: {8.5815, .76933},
		41: {64.008, .78111}, 81: {77.076, .54067},
		99: {531.609, .949448}, 100: {535.943, .956343},
		101: {480.412, .922284}, 105: {286.254, .956450},
		110: {163.748, .979341}, 120: {80.6977, .995493},
	}
	for tick := 0; tick < 1400; tick++ {
		if err := m.Update(); err != nil {
			t.Fatal(err)
		}
		c := m.Players[0].Character
		if c.StateNo != 700 {
			continue
		}
		if want, ok := observations[c.Time-1]; ok {
			if math.Abs(c.FloatVars[0]-want[0]) > .001 || math.Abs(c.FloatVars[1]-want[1]) > .00001 {
				t.Fatalf("native moving zoom at state time %d: camera/zoom=%v want=%v", c.Time-1, c.FloatVars[:2], want)
			}
			delete(observations, c.Time-1)
		}
		if c.Time > 121 {
			// The native return samples above include the mixed tick with one
			// root still offscreen; both midpoint and zoom must survive it.
			if len(observations) != 0 || m.Viewport.Zoom < .99 || m.CameraX > 100 {
				t.Fatal("moving zoom did not return after stage-bound travel", observations, m.CameraX, m.Viewport.Zoom)
			}
			m.Round++
			if err := m.resetRound(); err != nil {
				t.Fatal(err)
			}
			if m.CameraX != 0 || m.Viewport.Zoom != 1 {
				t.Fatal("round reset retained moving zoom state")
			}
			return
		}
	}
	t.Fatal("camera motion sequence did not finish")
}
