package match

import "testing"

func TestNativeAssertSpecialSchemas(t *testing.T) {
	replayNativeGeometryPrograms(t, "assertflags-*.json", 32)
}

func TestNativeAssertSpecialGlobalFlags(t *testing.T) {
	replayNativeGeometryPrograms(t, "assertglobal-*.json", 16)
}

func TestNativeAssertSpecialGuardContacts(t *testing.T) {
	replayNativeGeometryPrograms(t, "assertguard-*.json", 15)
}

func TestNativeAssertSpecialInputControls(t *testing.T) {
	replayNativeGeometryPrograms(t, "assertwalk2-*.json", 4)
	replayNativeGeometryPrograms(t, "assertentry2-*.json", 9)
}

func TestNativeAssertSpecialControlDrawing(t *testing.T) {
	replayNativeGeometryPrograms(t, "assertwalkfx-*.json", 4)
	replayNativeGeometryPrograms(t, "assertentryfx-*.json", 6)
	replayNativeGeometryPrograms(t, "assertguardfx-*.json", 6)
}

func TestNativeAssertSpecialCommandSyntax(t *testing.T) {
	replayNativeGeometryPrograms(t, "assertwalk-backward-baseline.json", 1)
}
