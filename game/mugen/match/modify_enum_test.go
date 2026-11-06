package match

import "testing"

func TestNativeModifyExplodEnums(t *testing.T) {
	replayNativeGeometryPrograms(t, "modifyenum-*.json", 20)
}
