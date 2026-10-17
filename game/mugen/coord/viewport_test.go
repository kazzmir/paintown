package coord

import "testing"

func TestNativeWorldAndHUDUseIndependentFits(t *testing.T) {
	world, err := GameSpaceForStage(1280, Space{320, 240})
	if err != nil {
		t.Fatal(err)
	}
	scale, x, y, err := Fit(world, Space{1280, 720})
	if err != nil || world != (Space{1280, 960}) || scale != .75 || x != 160 || y != 0 {
		t.Fatal("native 4:3 world fit", world, scale, x, y, err)
	}
	scale, x, y, err = Fit(Space{1280, 720}, Space{1280, 720})
	if err != nil || scale != 1 || x != 0 || y != 0 {
		t.Fatal("HUD inherited world margins", scale, x, y, err)
	}
	world, err = GameSpaceForStage(1280, Space{1280, 720})
	if err != nil || world != (Space{1280, 720}) {
		t.Fatal("HD stage aspect", world, err)
	}
}
