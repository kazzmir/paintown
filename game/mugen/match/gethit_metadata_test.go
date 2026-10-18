package match

import "testing"

// Every program and observation is frozen against beta1, including rejected
// manual selectors. The shared harness verifies all authored source hashes.
func TestNativeGetHitMetadata(t *testing.T) {
	replayNativeGeometryPrograms(t, "metadata-*.json", 13)
}
