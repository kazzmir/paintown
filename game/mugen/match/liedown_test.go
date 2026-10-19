package match

import "testing"

func TestNativeLiedownTimer(t *testing.T) {
	replayNativeGeometryPrograms(t, "recoverytimer-*.json", 10)
}
func TestNativeLiedownInput(t *testing.T) {
	replayNativeGeometryPrograms(t, "recoverymash-*.json", 5)
}
func TestNativeAuthoredRecoveryRoutes(t *testing.T) {
	replayNativeGeometryPrograms(t, "recoveryroute-*.json", 6)
}
