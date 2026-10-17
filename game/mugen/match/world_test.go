package match

import "testing"

func TestRoundResetRebindsRootsAndRetiresHelperIDs(t *testing.T) {
	m := stockMatch(t, 99)
	root := m.Players[0].Character
	root.Helper(map[string]string{"stateno": "0", "id": "777"})
	if root.RuntimeError != nil || len(root.Helpers) != 1 {
		t.Fatal("helper setup", root.RuntimeError)
	}
	rootID, helperID := root.EntityID, root.Helpers[0].EntityID
	if err := m.resetRound(); err != nil {
		t.Fatal(err)
	}
	if _, ok := m.World.Registry.Lookup(helperID); ok {
		t.Fatal("round reset retained helper ID")
	}
	fresh := m.Players[0].Character
	if fresh == root || fresh.EntityID != rootID {
		t.Fatal("round state did not replace persistent root identity")
	}
	if who, ok := m.World.Registry.Lookup(rootID); !ok || who != fresh {
		t.Fatal("root lookup retained old round state")
	}
	fresh.Helper(map[string]string{"stateno": "0", "id": "777"})
	if fresh.RuntimeError != nil || len(fresh.Helpers) != 1 || fresh.Helpers[0].EntityID <= helperID {
		t.Fatal("new round reused a retired helper ID", fresh.RuntimeError)
	}
}
