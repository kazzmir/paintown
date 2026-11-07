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
`
	for index, expression := range []string{"TopEdge", "BottomEdge", "ScreenPos Y", "Pos Y", "GameHeight", "EnemyNear, TopEdge", "EnemyNear, BottomEdge", "CameraPos Y"} {
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
	if err := os.WriteFile(path, []byte(fmt.Sprintf(`[Info]
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
`, filepath.Join(options.RootDir, "stages/kfm.sff"))), 0600); err != nil {
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
	position := func(y0, y1 float64) {
		for side, p := range m.Players {
			c := p.Character
			c.X, c.Y = float64(side*80-40)*float64(c.LocalCoordinateWidth())/320, []float64{y0, y1}[side]*float64(c.LocalCoordinateWidth())/320
			c.VelX, c.VelY = 0, 0
			c.IntVars[50], c.IntVars[51] = 1, 1
		}
		for tick := 0; tick < 80; tick++ {
			step()
		}
	}
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
	m.Round++
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	if m.CameraY != 0 || m.Viewport.Zoom != 1 {
		t.Fatal("vertical camera state survived round reset")
	}
}
