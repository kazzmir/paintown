package match

import "testing"

func TestNativeExplodCreationPool(t *testing.T) {
	replayNativeGeometryPrograms(t, "creationpool-*.json", 28)
}

func TestNativeExplodCreationPoolExtras(t *testing.T) {
	replayNativeGeometryPrograms(t, "creationpoolextra-*.json", 19)
}

func TestNativeExplodCreationPoolCues(t *testing.T) {
	replayNativeGeometryPrograms(t, "creationpoolcue-*.json", 3)
}

func TestNativeExplodCreationPoolCueRandom(t *testing.T) {
	replayNativeGeometryPrograms(t, "creationpoolcuerng-*.json", 3)
}
