package match

import "testing"

func TestNativeHitPauseTimeBinding(t *testing.T) {
	replayNativeGeometryPrograms(t, "hitpausevar-*.json", 7)
}
