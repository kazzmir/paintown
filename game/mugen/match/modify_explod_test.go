package match

import "testing"

func TestNativeModifyExplodPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "modexplod-*.json", 143)
}

func TestNativeModifyExplodIntent(t *testing.T) {
	assertNativeControllerKind(t, "modexplod-*.json", "ModifyExplod", 143)
}

func TestNativeExplodAlphaGridPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "alphamix-grid-*.json", 3)
}

func TestNativeExplodAlphaGridIntent(t *testing.T) {
	assertNativeControllerCount(t, "alphamix-grid-*.json", "ModifyExplod", 3, 9)
	assertNativeControllerCount(t, "alphamix-grid-*.json", "Explod", 3, 9)
}
