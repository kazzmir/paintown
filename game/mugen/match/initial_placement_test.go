package match

import "testing"

// These native observers retain post-controller values while the match pauses
// at the initial round phases, including the first RoundState 2 owner pass.
func TestNativeInitialRootPlacement(t *testing.T) {
	replayNativeGeometryPrograms(t, "preintropose-*.json", 10)
	replayNativeGeometryPrograms(t, "posecontext-*.json", 10)
	replayNativeGeometryPrograms(t, "poseboundary-*.json", 2)
	replayNativeGeometryPrograms(t, "posevertical-*.json", 1)
}
