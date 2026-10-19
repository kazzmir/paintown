package match

import "testing"

func TestNativeProjectileSpawnPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "projspawn-*.json", 82)
}

func TestNativeProjectilePositionAndDefaults(t *testing.T) {
	replayNativeGeometryPrograms(t, "projdraw-*.json", 15)
}

func TestNativeProjectileCountAndSchema(t *testing.T) {
	replayNativeGeometryPrograms(t, "projschema-*.json", 7)
}

func TestNativeProjectileContactPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "projlife-*.json", 33)
}

func TestNativeProjectileCancellationPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "projcancel-*.json", 25)
}

func TestNativeProjectileDetailPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "projdetail-*.json", 14)
}

func TestNativeProjectileFollowPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "projfollow-*.json", 7)
}

func TestNativeProjectilePhasePrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "projphase-*.json", 3)
}

func TestNativeProjectileClockPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "projclock-*.json", 6)
}

func TestNativeProjectileAnimationEndPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "projend-*.json", 3)
}

func TestNativeProjectileCoordinatePrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "projspace-*.json", 11)
}
