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
