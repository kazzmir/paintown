//go:build !linux

package paintown

// Other platforms use Ebiten's standard gamepad mapping.
func pollLegacyController() (controllerState, string) { return controllerState{}, "" }
