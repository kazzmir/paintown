package match

import (
	"fmt"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
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
movecamera=0,0
`
	for index, expression := range []string{"TopEdge", "BottomEdge", "ScreenPos Y", "Pos Y", "GameHeight", "EnemyNear, TopEdge", "EnemyNear, BottomEdge"} {
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
			wantTop := cameraY*scale - float32(m.Viewport.GroundY)*scale/zoom - shake*float32(c.LocalCoordinateWidth())/320
			wantHeight := float32(m.Viewport.Space.Height) * scale / zoom
			if float32(v[0]) != wantTop || float32(v[4]) != wantHeight {
				t.Fatalf("player %d: crop/zoom/shake: top=%v height=%v want=%v/%v", slot+1, v[0], v[4], wantTop, wantHeight)
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
}
