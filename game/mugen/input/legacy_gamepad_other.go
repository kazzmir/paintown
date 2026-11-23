//go:build !linux

package input

// Other-platform native device namespaces need their own capability adapter.
// Portable standard-layout and explicit SDK raw YAML/session bindings still
// work there; never guess the real-button/hat boundary from a combined count.
func discoverLegacyJoystick(_, _ string, _, _ int) (LegacyJoystickLayout, bool) {
	return LegacyJoystickLayout{}, false
}
