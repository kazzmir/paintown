package match

import "testing"

func TestNativeNoKOContacts(t *testing.T) {
	replayNativeGeometryPrograms(t, "assertnoko-*.json", 8)
}

func TestNativeNoKOLifetime(t *testing.T) {
	replayNativeGeometryPrograms(t, "nokopass-*.json", 22)
}

func TestNativeNoKOFrozenOwner(t *testing.T) {
	replayNativeGeometryPrograms(t, "nokofreeze-*.json", 8)
}

func TestNativeSelfKOEntry(t *testing.T) {
	replayNativeGeometryPrograms(t, "selfkoentry-*.json", 6)
}
