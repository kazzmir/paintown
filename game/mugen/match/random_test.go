package match

import "testing"

func TestNativeRandomPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "randomdraw-*.json", 21)
}
