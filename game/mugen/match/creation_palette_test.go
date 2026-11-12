package match

import "testing"

func TestNativeExplodCreationPalettes(t *testing.T) {
	replayNativeGeometryPrograms(t, "creationpal-*.json", 55)
	replayNativeGeometryPrograms(t, "creationphase-*.json", 8)
}

func TestNativeExplodPaletteContexts(t *testing.T) {
	replayNativeGeometryPrograms(t, "palettecontext-*.json", 36)
}

func TestNativeExplodPaletteEdges(t *testing.T) {
	replayNativeGeometryPrograms(t, "paletteedge-*.json", 6)
}
