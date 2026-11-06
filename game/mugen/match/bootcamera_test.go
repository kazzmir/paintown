package match

import "testing"

// Both fighters save their boot and first two controller-pass camera queries.
// These are camera publication controls, independent of system-effect producers.
func TestNativeStartupCameraPublication(t *testing.T) {
	replayNativeGeometryPrograms(t, "bootcamera-*.json", 4)
}
