package coord

import "testing"

func TestNativeWorldAndHUDShareViewportFit(t *testing.T) {
	world, err := GameSpaceForStage(1280, Space{320, 240})
	if err != nil {
		t.Fatal(err)
	}
	scale, x, y, err := Fit(world, Space{1280, 720})
	if err != nil || world != (Space{1280, 960}) || scale != .75 || x != 160 || y != 0 {
		t.Fatal("native 4:3 world fit", world, scale, x, y, err)
	}
	hud, err := HUDProject(world, Space{1280, 720}, Space{1280, 720}, true)
	if err != nil || hud != (Transform{.75, .75, 160, 0}) {
		t.Fatal("HUD did not inherit world margins", hud, err)
	}
	world, err = GameSpaceForStage(1280, Space{1280, 720})
	if err != nil || world != (Space{1280, 720}) {
		t.Fatal("HD stage aspect", world, err)
	}
}
