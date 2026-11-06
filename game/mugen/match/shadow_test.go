package match

import "testing"

func TestNativeShadowPrograms(t *testing.T) {
	replayNativeGeometryPrograms(t, "shadowshape-*.json", 62)
	replayNativeGeometryPrograms(t, "shadowdetail-*.json", 26)
}

func TestNativeShadowCreationSchema(t *testing.T) {
	replayNativeGeometryPrograms(t, "shadowschema-*.json", 10)
}
