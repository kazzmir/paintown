package match

import "testing"

func TestNativeExplodCreationPalettes(t *testing.T) {
	replayNativeGeometryPrograms(t, "creationpal-*.json", 55)
	replayNativeGeometryPrograms(t, "creationphase-*.json", 8)
}
