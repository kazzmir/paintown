package match

import "testing"

// Original native inputs, including rejected schemas, remain immutable.
func TestNativeMakeDustFirstLook(t *testing.T) {
	replayNativeGeometryPrograms(t, "systemanim-dust-*.json", 7)
	replayNativeGeometryPrograms(t, "systemearly-dust-*.json", 10)
}
func TestNativeMakeDustSchemas(t *testing.T) {
	replayNativeGeometryPrograms(t, "dustschema-*.json", 47)
}
func TestNativeMakeDustPhases(t *testing.T) { replayNativeGeometryPrograms(t, "dustphase-*.json", 35) }
func TestNativeMakeDustIntro(t *testing.T) {
	replayNativeGeometryPrograms(t, "dustintrovalid-*.json", 19)
}
func TestNativeMakeDustContexts(t *testing.T) {
	replayNativeGeometryPrograms(t, "dustcontext-*.json", 36)
}
func TestNativeMakeDustEarlyGeometry(t *testing.T) {
	replayNativeGeometryPrograms(t, "dustgeometry-*.json", 4)
}

func TestNativeMakeDustPauseClock(t *testing.T) {
	replayNativeGeometryPrograms(t, "dustpause-*.json", 4)
	replayNativeGeometryPrograms(t, "dustpauselate-*.json", 4)
}
