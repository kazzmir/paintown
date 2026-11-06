package match

import "testing"

func TestNativeAssertSpecialSchemas(t *testing.T) {
	replayNativeGeometryPrograms(t, "assertflags-*.json", 32)
}

func TestNativeAssertSpecialGlobalFlags(t *testing.T) {
	replayNativeGeometryPrograms(t, "assertglobal-*.json", 16)
}
