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
