package match

import "testing"

func TestNativeGameAnimationFirstLook(t *testing.T) {
	replayNativeGeometryPrograms(t, "systemanim-game-*.json", 9)
}
func TestNativeGameAnimationEarly(t *testing.T) {
	replayNativeGeometryPrograms(t, "systemearly-game-*.json", 10)
}
func TestNativeGameAnimationResources(t *testing.T) {
	replayNativeGeometryPrograms(t, "systemresource-*.json", 12)
}
func TestNativeGameAnimationSchemas(t *testing.T) {
	replayNativeGeometryPrograms(t, "gameanim-*.json", 42)
}
func TestNativeGameAnimationLayers(t *testing.T) {
	replayNativeGeometryPrograms(t, "gameoverlap-*.json", 9)
}
func TestNativeGameAnimationPhases(t *testing.T) {
	replayNativeGeometryPrograms(t, "gamephase-*.json", 2)
}

func TestNativeGameAnimationDependencies(t *testing.T) {
	replayNativeGeometryPrograms(t, "gamecontext-*.json", 38)
}

func TestNativeGameAnimationBoundaries(t *testing.T) {
	replayNativeGeometryPrograms(t, "gameedge-*.json", 8)
}

func TestNativeGameAnimationHorizontalOrigin(t *testing.T) {
	replayNativeGeometryPrograms(t, "gameorigin-*.json", 4)
}

func TestNativeGameAnimationVerticalOrigin(t *testing.T) {
	replayNativeGeometryPrograms(t, "gamevertical-*.json", 4)
}
