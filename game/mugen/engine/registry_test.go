package engine

import "testing"

func TestRegistryRetirementAndPersistentRootReplacement(t *testing.T) {
	a, b := NewRegistry[string](56), NewRegistry[string](56)
	root, err := a.Allocate("root round 1")
	if err != nil {
		t.Fatal(err)
	}
	helper, err := a.Allocate("helper")
	if err != nil {
		t.Fatal(err)
	}
	a.Remove(helper)
	if _, ok := a.Lookup(helper); ok {
		t.Fatal("retired ID remains live")
	}
	if err := a.Replace(root, "root round 2"); err != nil {
		t.Fatal(err)
	}
	next, err := a.Allocate("new helper")
	if err != nil {
		t.Fatal(err)
	}
	if next <= helper {
		t.Fatal("retirement reused identity")
	}
	if got, _ := a.Lookup(root); got != "root round 2" {
		t.Fatal("persistent root did not rebind")
	}
	other, err := b.Allocate("independent match")
	if err != nil {
		t.Fatal(err)
	}
	if other != root {
		t.Fatal("matches shared an ID allocator")
	}
}
