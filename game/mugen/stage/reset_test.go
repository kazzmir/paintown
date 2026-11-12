package stage

import (
	"os"
	"path/filepath"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/background"
)

func TestRoundBackgroundReset(t *testing.T) {
	for _, reset := range []bool{true, false} {
		t.Run(map[bool]string{true: "reset", false: "continue"}[reset], func(t *testing.T) {
			dir := t.TempDir()
			// No sprites are needed to observe controller, velocity and link clocks.
			source := "[StageInfo]\nresetBG=1\n[Bound]\nscreenleft=15\nscreenright=15\n[BG floor]\ntype=normal\nspriteno=0,0\nid=1\nvelocity=2,0\n[BG linked]\ntype=normal\nspriteno=0,0\npositionlink=1\n[BGCtrlDef]\n[BGCtrl]\ntype=VelSet\ntime=2\nctrlid=1\nx=9\n"
			path := filepath.Join(dir, "stage.def")
			if err := os.WriteFile(path, []byte(source), 0600); err != nil {
				t.Fatal(err)
			}
			s, err := LoadStage(path, dir)
			if err != nil {
				t.Fatal(err)
			}
			s.StageInfo.ResetBG = reset
			for i := 0; i < 6; i++ {
				s.Update()
			}
			old := s.Background.Elements[0].(*background.NormalElement)
			oldTime := old.Time
			oldX, _ := old.GetPosition()
			s.ResetBackground()
			current := s.Background.Elements[0].(*background.NormalElement)
			if reset {
				x, _ := current.GetPosition()
				if current == old || current.Time != 0 || x != 0 || current.Velocity[0] != 2 || s.Background.Controllers[0].Time != 0 {
					t.Fatal("background retained mutable round state", current)
				}
				if s.Background.Elements[1].GetLink() != current {
					t.Fatal("link retained the old element")
				}
				s.Update()
				x, _ = current.GetPosition()
				if x != 2 || old.Time != oldTime {
					t.Fatal("new round changed retired state", x, old.Time)
				}
			} else {
				x, _ := current.GetPosition()
				if current != old || current.Time != oldTime || x != oldX {
					t.Fatal("resetBG=0 restarted background")
				}
			}
		})
	}
}
