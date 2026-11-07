package config

import (
	"fmt"
	"math/bits"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
)

const NoKey ebiten.Key = -1

// keyField canonicalizes UI aliases and assigns a presence bit. Presence cannot
// be inferred from a nonzero key: Ebitengine's valid KeyA is zero.
func (k *Keys) keyField(name string) (*ebiten.Key, uint16) {
	switch strings.ToLower(name) {
	case "jump", "up":
		return &k.Jump, 1 << 0
	case "crouch", "down":
		return &k.Crouch, 1 << 1
	case "left":
		return &k.Left, 1 << 2
	case "right":
		return &k.Right, 1 << 3
	case "a":
		return &k.A, 1 << 4
	case "b":
		return &k.B, 1 << 5
	case "c":
		return &k.C, 1 << 6
	case "x":
		return &k.X, 1 << 7
	case "y":
		return &k.Y, 1 << 8
	case "z":
		return &k.Z, 1 << 9
	case "start":
		return &k.Start, 1 << 10
	case "escape":
		return &k.Escape, 1 << 11
	}
	return nil, 0
}

// SetBinding retains explicit KeyA and disabled bindings across value copies.
func (k *Keys) SetBinding(action string, key ebiten.Key) bool {
	return k.SetBindings(action, key)
}

// SetBindings replaces the action, deduplicating alternatives and preserving
// value-copy isolation. An empty list or only NoKey explicitly disables it.
func (k *Keys) SetBindings(action string, keys ...ebiten.Key) bool {
	field, bit := k.keyField(action)
	if field == nil {
		return false
	}
	unique := make([]ebiten.Key, 0, len(keys))
	for _, key := range keys {
		if key == NoKey {
			continue
		}
		duplicate := false
		for _, previous := range unique {
			duplicate = duplicate || previous == key
		}
		if !duplicate {
			unique = append(unique, key)
		}
	}
	*field = NoKey
	k.alternates[bits.TrailingZeros16(bit)] = nil
	if len(unique) > 0 {
		*field = unique[0]
		k.alternates[bits.TrailingZeros16(bit)] = unique[1:]
	}
	k.configured |= bit
	return true
}

func (k Keys) Binding(action string) (ebiten.Key, bool) {
	field, bit := k.keyField(action)
	if field == nil {
		return NoKey, false
	}
	// Retain compatibility with older callers constructing nonzero fields.
	return *field, k.configured&bit != 0 || *field != 0
}

// Bindings returns an independent snapshot. Binding retains the original
// primary-key API; runtime matching below avoids allocating these snapshots.
func (k Keys) Bindings(action string) ([]ebiten.Key, bool) {
	key, present := k.Binding(action)
	if !present || key == NoKey {
		return nil, present
	}
	_, bit := k.keyField(action)
	return append([]ebiten.Key{key}, k.alternates[bits.TrailingZeros16(bit)]...), true
}

func (k *Keys) MatchBinding(action string, match func(ebiten.Key) bool) (bool, bool) {
	field, bit := k.keyField(action)
	if field == nil || (k.configured&bit == 0 && *field == 0) {
		return false, false
	}
	key := *field
	if key != NoKey && match(key) {
		return true, true
	}
	for _, key := range k.alternates[bits.TrailingZeros16(bit)] {
		if match(key) {
			return true, true
		}
	}
	return false, true
}

// SDL 1.2 symbols used by the supplied 1.1 beta configuration. Reference:
// https://github.com/libsdl-org/SDL-1.2/blob/main/include/SDL_keysym.h
// Layout-specific WORLD symbols and keys without an Ebitengine equivalent are
// left unbound and diagnosed by the loader, rather than becoming KeyA.
func sdlKeyToKey(code int) ebiten.Key {
	if code >= 'a' && code <= 'z' || code >= '0' && code <= '9' {
		key, _ := stringToKey(string(rune(code)))
		return key
	}
	if code >= 256 && code <= 265 {
		key, _ := stringToKey(fmt.Sprintf("Numpad%d", code-256))
		return key
	}
	if code >= 282 && code <= 296 {
		key, _ := stringToKey(fmt.Sprintf("F%d", code-281))
		return key
	}
	name := map[int]string{
		8: "Backspace", 9: "Tab", 13: "Enter", 19: "Pause", 27: "Escape", 32: "Space",
		39: "Quote", 44: "Comma", 45: "Minus", 46: "Period", 47: "Slash", 59: "Semicolon",
		61: "Equal", 91: "BracketLeft", 92: "Backslash", 93: "BracketRight", 96: "Backquote", 127: "Delete",
		266: "NumpadDecimal", 267: "NumpadDivide", 268: "NumpadMultiply", 269: "NumpadSubtract",
		270: "NumpadAdd", 271: "NumpadEnter", 272: "NumpadEqual",
		273: "ArrowUp", 274: "ArrowDown", 275: "ArrowRight", 276: "ArrowLeft", 277: "Insert",
		278: "Home", 279: "End", 280: "PageUp", 281: "PageDown",
		300: "NumLock", 301: "CapsLock", 302: "ScrollLock", 303: "ShiftRight", 304: "ShiftLeft",
		305: "ControlRight", 306: "ControlLeft", 307: "AltRight", 308: "AltLeft",
		309: "MetaRight", 310: "MetaLeft", 311: "MetaLeft", 312: "MetaRight",
		316: "PrintScreen", 319: "ContextMenu",
	}[code]
	key, _ := stringToKey(name)
	return key
}

func stringToKey(name string) (ebiten.Key, bool) {
	name = strings.ToLower(strings.TrimSpace(name))
	name = strings.TrimPrefix(name, "key")
	if name == "none" || name == "disabled" {
		return NoKey, true
	}
	if name == "up" || name == "down" || name == "left" || name == "right" {
		name = "arrow" + name
	}
	var key ebiten.Key
	if err := key.UnmarshalText([]byte(name)); err != nil {
		return NoKey, false
	}
	return key, true
}
