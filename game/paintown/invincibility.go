package paintown

import "sync/atomic"

var invincibilityEnabled atomic.Bool

// Invincible is shared by the human player and AI buddies, including fighters
// already in a level. It does not heal or revive previously defeated fighters.
func Invincible() bool           { return invincibilityEnabled.Load() }
func SetInvincible(enabled bool) { invincibilityEnabled.Store(enabled) }

func (option MenuOption) Label() string {
	if option.Kind == "invincible" {
		if Invincible() {
			return option.Name + ": On"
		}
		return option.Name + ": Off"
	}
	return option.Name
}
