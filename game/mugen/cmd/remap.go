package cmd

var buttonSymbols = [...]string{"a", "b", "c", "x", "y", "z", "s"}

func buttonKey(symbol string) string {
	switch symbol {
	case "a":
		return "ButtonA"
	case "b":
		return "ButtonB"
	case "c":
		return "ButtonC"
	case "x":
		return "ButtonX"
	case "y":
		return "ButtonY"
	case "z":
		return "ButtonZ"
	case "s":
		return "ButtonStart"
	}
	return ""
}

func buttonSymbol(key string) string {
	for _, symbol := range buttonSymbols {
		if buttonKey(symbol) == key {
			return symbol
		}
	}
	return ""
}

func (d *Data) physicalKey(logical string) string {
	if d != nil {
		if physical, remapped := d.Remap[buttonSymbol(logical)]; remapped {
			return buttonKey(physical)
		}
	}
	return logical
}

// RemapInputs reads all logical buttons from the same physical snapshot.
// x=a means logical x reads physical a; it is not a sequential rename. Several
// logical buttons can share one physical button. Directions are unchanged.
func (d *Data) RemapInputs(physical []string) []string {
	if d == nil || len(d.Remap) == 0 {
		return append([]string(nil), physical...)
	}
	held := make(map[string]bool, len(physical))
	var logical []string
	for _, key := range physical {
		if !held[key] && buttonSymbol(key) == "" {
			logical = append(logical, key)
		}
		held[key] = true
	}
	for _, symbol := range buttonSymbols {
		key := buttonKey(symbol)
		if physical := d.physicalKey(key); physical != "" && held[physical] {
			logical = append(logical, key)
		}
	}
	return logical
}

// PhysicalInputs lets authored AI command plans use the same remap as humans.
// A disabled logical button makes the command unreachable. Shared physical
// buttons are pressed once even when a chord names them more than once.
func (d *Data) PhysicalInputs(logical []string) ([]string, bool) {
	var physical []string
	seen := make(map[string]bool, len(logical))
	for _, key := range logical {
		key = d.physicalKey(key)
		if key == "" {
			return nil, false
		}
		if !seen[key] {
			physical = append(physical, key)
			seen[key] = true
		}
	}
	return physical, true
}
