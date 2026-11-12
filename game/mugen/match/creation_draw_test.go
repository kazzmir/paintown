package match

import "testing"

func TestNativeExplodCreationDrawing(t *testing.T) {
	replayNativeGeometryPrograms(t, "creationdraw-*.json", 41)
}

func TestNativeExplodCreationDrawingExtras(t *testing.T) {
	replayNativeGeometryPrograms(t, "creationextra-*.json", 6)
}
