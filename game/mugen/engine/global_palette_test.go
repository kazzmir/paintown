package engine

import "testing"

func TestGlobalPaletteFrameClocksAndIndependentCancel(t *testing.T) {
	clock := &Timeline{}
	clock.BeginFrame()
	clock.SetGlobalPalette(PaletteFXParameters{Scope: PaletteFXAll, Time: 1, Color: 256, Mul: [3]int{256, 256, 256}})
	clock.SetGlobalPalette(PaletteFXParameters{Scope: PaletteFXBackground, Time: -1, Color: 256, Mul: [3]int{256, 256, 256}, SinAdd: [3]int{64, 0, 0}, Period: 4})
	clock.RequestPause(NewPauseActor(), PauseParameters{Time: 30}, false)
	clock.EndFrame()
	if !clock.AllPalette.Transform().Enabled || clock.BackgroundPalette.Transform().Add[0] != 0 {
		t.Fatal("global issuing frame must remain visible at sine age zero")
	}
	clock.BeginFrame()
	clock.BeginFrame()
	if !clock.Paused() || clock.AllPalette.Transform().Enabled || clock.BackgroundPalette.Transform().Add[0] != float32(64)/255 {
		t.Fatal("global clocks stopped for Pause or advanced twice for a nested frame")
	}
	clock.EndFrame()
	clock.EndFrame()
	clock.SkipSimulationFrame()
	clock.SkipSimulationFrame()
	if clock.BackgroundPalette.Transform().Add[0] != float32(-64)/255 {
		t.Fatal("global palette did not advance with the skipped simulation frame clock")
	}
	clock.SetGlobalPalette(PaletteFXParameters{Scope: PaletteFXAll, Time: -1, Color: 256, Mul: [3]int{256, 256, 256}})
	clock.SetGlobalPalette(PaletteFXParameters{Scope: PaletteFXBackground, Time: 0})
	if !clock.AllPalette.Transform().Enabled || clock.BackgroundPalette.Transform().Enabled {
		t.Fatal("BGPalFX cancel replaced AllPalFX")
	}
}
