package match

import "testing"

func TestNativeForceFeedbackPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "forcefeedback-*.json", 18)
}
