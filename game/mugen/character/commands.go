package character

import (
	"strings"

	"github.com/kazzmir/paintown/game/mugen/input"
)

// Command represents a single MUGEN command from .cmd
type Command struct {
	Name    string
	Command string
	Time    int
	Buffer  int
}

// CommandBuffer tracks input history to match commands
type CommandBuffer struct {
	History [][]string // Active input names per tick
	MaxSize int
}

func NewCommandBuffer(size int) *CommandBuffer {
	return &CommandBuffer{
		History: make([][]string, 0, size),
		MaxSize: size,
	}
}

func (cb *CommandBuffer) Add(inputs []string) {
	cb.History = append(cb.History, inputs)
	if len(cb.History) > cb.MaxSize {
		cb.History = cb.History[1:]
	}
}

// Match checks if a command string (like "D, DF, F, a") is matched in history.
// cmdStr: the comma-separated sequence
// maxTime: the total ticks allowed for the sequence
// bufferTime: the number of ticks from the end of history to find the LAST part of the sequence
func (cb *CommandBuffer) Match(cmdStr string, maxTime, bufferTime int) bool {
	if cmdStr == "" || len(cb.History) == 0 {
		return false
	}

	parts := strings.Split(cmdStr, ",")
	for i := range parts {
		parts[i] = strings.TrimSpace(parts[i])
	}

	// 1. Find the LAST element of the sequence in the bufferTime window
	// If bufferTime is 1, it must be the current tick.
	// If bufferTime is > 1, it can be any of the last N ticks.
	historyIdx := -1
	for i := len(cb.History) - 1; i >= 0 && i >= len(cb.History)-bufferTime; i-- {
		if cb.tickMatches(i, parts[len(parts)-1]) {
			historyIdx = i
			break
		}
	}

	if historyIdx == -1 {
		return false
	}

	// Special case: single button/direction
	if len(parts) == 1 {
		return true
	}

	// 2. Match the rest of the sequence backwards (within maxTime)
	partIdx := len(parts) - 2
	lastMatchTick := historyIdx

	// The total time limit starts from the tick we matched the last part.
	limit := historyIdx - maxTime
	if limit < 0 {
		limit = 0
	}

	for partIdx >= 0 {
		found := false
		// Search backwards from lastMatchTick
		for i := lastMatchTick - 1; i >= limit; i-- {
			if cb.tickMatches(i, parts[partIdx]) {
				// MUGEN Accuracy:
				// For same-direction components (e.g. F, F), there must be a gap.
				// This is only required for "tap" motions.
				if partIdx < len(parts)-1 {
					cleanCurr := strings.TrimPrefix(strings.TrimPrefix(parts[partIdx], "/"), "~")
					cleanPrev := strings.TrimPrefix(strings.TrimPrefix(parts[partIdx+1], "/"), "~")
					if cleanCurr == cleanPrev && !IsButton(cleanCurr) {
						hasGap := false
						for j := i + 1; j < lastMatchTick; j++ {
							if !cb.tickMatches(j, cleanCurr) {
								hasGap = true
								break
							}
						}
						if !hasGap {
							continue
						}
					}
				}

				lastMatchTick = i
				partIdx--
				found = true
				break
			}
		}
		if !found {
			return false
		}
	}

	return true
}

func (cb *CommandBuffer) tickMatches(tickIdx int, target string) bool {
	if tickIdx < 0 || tickIdx >= len(cb.History) {
		return false
	}

	hold := strings.HasPrefix(target, "/")
	release := strings.HasPrefix(target, "~")
	directionOnly := strings.HasPrefix(target, "$") || strings.Contains(target, "$")

	// Strip modifiers for base target
	base := target
	base = strings.TrimPrefix(base, "/")
	base = strings.TrimPrefix(base, "~")
	base = strings.ReplaceAll(base, "$", "")

	has := func(idx int, s string) bool {
		if idx < 0 || idx >= len(cb.History) {
			return false
		}
		for _, in := range cb.History[idx] {
			if in == s {
				return true
			}
		}
		return false
	}

	matchBase := func(idx int, b string) bool {
		// Direction-only matches ($D = D | DF | DB)
		if directionOnly {
			switch b {
			case "D":
				return has(idx, "DirDown")
			case "F":
				return has(idx, "DirForward")
			case "B":
				return has(idx, "DirBack")
			case "U":
				return has(idx, "DirUp")
			}
		}

		// Exact match against spelled-out internal names
		switch b {
		case "F":
			return has(idx, "DirForward")
		case "B":
			return has(idx, "DirBack")
		case "U":
			return has(idx, "DirUp")
		case "D":
			return has(idx, "DirDown")
		case "DF":
			return has(idx, "DirDown") && has(idx, "DirForward")
		case "DB":
			return has(idx, "DirDown") && has(idx, "DirBack")
		case "UF":
			return has(idx, "DirUp") && has(idx, "DirForward")
		case "UB":
			return has(idx, "DirUp") && has(idx, "DirBack")
		case "a":
			return has(idx, "ButtonA")
		case "b":
			return has(idx, "ButtonB")
		case "c":
			return has(idx, "ButtonC")
		case "x":
			return has(idx, "ButtonX")
		case "y":
			return has(idx, "ButtonY")
		case "z":
			return has(idx, "ButtonZ")
		case "s":
			return has(idx, "ButtonStart")
		}

		return false
	}

	if release {
		// Key was pressed at T-1 and is NOT pressed at T
		return matchBase(tickIdx-1, base) && !matchBase(tickIdx, base)
	}

	if hold {
		return matchBase(tickIdx, base)
	}

	// Normal Press (Default)
	isButton := false
	switch strings.ToLower(base) {
	case "a", "b", "c", "x", "y", "z", "s":
		isButton = true
	}

	if isButton {
		// Edge trigger for buttons (requested by user: "So abczxy shouldn't but directionals should.")
		return matchBase(tickIdx, base) && !matchBase(tickIdx-1, base)
	}

	// For directions, match if held (maintain state)
	return matchBase(tickIdx, base)
}

// GetRawInputs returns the current pressed keys as MUGEN input names
func GetRawInputs(player int, facing int) []string {
	var inputs []string

	if input.GlobalManager == nil {
		return inputs
	}

	// Directional keys (Relative to facing)
	rightPressed := input.GlobalManager.IsPressed(player, input.ActionRight)
	leftPressed := input.GlobalManager.IsPressed(player, input.ActionLeft)

	if facing == 1 {
		if rightPressed {
			inputs = append(inputs, "DirForward")
		}
		if leftPressed {
			inputs = append(inputs, "DirBack")
		}
	} else {
		// Facing left (-1)
		if rightPressed {
			inputs = append(inputs, "DirBack")
		}
		if leftPressed {
			inputs = append(inputs, "DirForward")
		}
	}

	if input.GlobalManager.IsPressed(player, input.ActionJump) {
		inputs = append(inputs, "DirUp")
	}
	if input.GlobalManager.IsPressed(player, input.ActionCrouch) {
		inputs = append(inputs, "DirDown")
	}

	// Button keys
	if input.GlobalManager.IsPressed(player, input.ActionX) {
		inputs = append(inputs, "ButtonX")
	}
	if input.GlobalManager.IsPressed(player, input.ActionY) {
		inputs = append(inputs, "ButtonY")
	}
	if input.GlobalManager.IsPressed(player, input.ActionZ) {
		inputs = append(inputs, "ButtonZ")
	}
	if input.GlobalManager.IsPressed(player, input.ActionA) {
		inputs = append(inputs, "ButtonA")
	}
	if input.GlobalManager.IsPressed(player, input.ActionB) {
		inputs = append(inputs, "ButtonB")
	}
	if input.GlobalManager.IsPressed(player, input.ActionC) {
		inputs = append(inputs, "ButtonC")
	}
	if input.GlobalManager.IsPressed(player, input.ActionStart) {
		inputs = append(inputs, "ButtonStart")
	}

	if len(inputs) > 1 {
		// fmt.Printf("Tick Inputs: %v\n", inputs)
	}

	return inputs
}

// IsButton returns true if the input name represents a button (not a direction)
func IsButton(name string) bool {
	switch strings.ToLower(name) {
	case "a", "b", "c", "x", "y", "z", "s", "start":
		return true
	}
	return false
}
