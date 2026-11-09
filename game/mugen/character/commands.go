package character

import (
	"sort"
	"strconv"
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
	if strings.TrimSpace(cmdStr) == "" || len(cb.History) == 0 {
		return false
	}
	parts := strings.Split(cmdStr, ",")
	for i := range parts {
		parts[i] = strings.TrimSpace(parts[i])
		if parts[i] == "" {
			return false
		}
	}
	// Repeated directions require an uninterrupted release/press pair.
	var expanded []string
	for _, part := range parts {
		if len(expanded) > 0 {
			previous := strings.TrimLeft(expanded[len(expanded)-1], "/>$")
			current := strings.TrimLeft(part, "/>$")
			if previous == current && !IsButton(current) && !strings.HasPrefix(part, "~") {
				expanded = append(expanded, ">~"+current)
				part = ">" + part
			}
		}
		expanded = append(expanded, part)
	}
	parts = expanded
	for end := len(cb.History) - 1; end >= 0 && end >= len(cb.History)-max(1, bufferTime); end-- {
		if !cb.tickMatches(end, parts[len(parts)-1]) {
			continue
		}
		held := true
		for _, part := range parts {
			if strings.HasPrefix(strings.TrimPrefix(part, ">"), "/") && !cb.tickMatches(end, part) {
				held = false
				break
			}
		}
		if !held {
			continue
		}
		last := end
		limit := max(0, end-max(0, maxTime))
		matched := true
		for part := len(parts) - 2; part >= 0; part-- {
			found := false
			first := last - 1
			// A release can coincide with the next direction; /held
			// symbols can coincide with the button that completes a chord.
			if strings.ContainsAny(parts[part], "~/") {
				first = last
			}
			for tick := first; tick >= limit; tick-- {
				if !cb.tickMatches(tick, parts[part]) {
					continue
				}
				if strings.HasPrefix(parts[part+1], ">") && !cb.uninterrupted(tick, last, parts[part+1]) {
					continue
				}
				last, found = tick, true
				break
			}
			if !found {
				matched = false
				break
			}
		}
		if matched {
			return true
		}
	}
	return false
}

func (cb *CommandBuffer) uninterrupted(first, last int, target string) bool {
	allowed := map[string]bool{}
	for _, key := range commandKeys(target) {
		allowed[key] = true
	}
	for tick := first + 1; tick <= last; tick++ {
		previous, current := map[string]bool{}, map[string]bool{}
		for _, key := range cb.History[tick-1] {
			previous[key] = true
		}
		for _, key := range cb.History[tick] {
			current[key] = true
		}
		for key := range previous {
			if previous[key] != current[key] && !allowed[key] {
				return false
			}
		}
		for key := range current {
			if previous[key] != current[key] && !allowed[key] {
				return false
			}
		}
	}
	return true
}

func commandKeys(symbol string) []string {
	symbol = strings.TrimLeft(symbol, ">/~$0123456789")
	var keys []string
	for _, part := range strings.Split(symbol, "+") {
		switch part {
		case "F":
			keys = append(keys, "DirForward")
		case "B":
			keys = append(keys, "DirBack")
		case "U":
			keys = append(keys, "DirUp")
		case "D":
			keys = append(keys, "DirDown")
		case "DF":
			keys = append(keys, "DirDown", "DirForward")
		case "DB":
			keys = append(keys, "DirDown", "DirBack")
		case "UF":
			keys = append(keys, "DirUp", "DirForward")
		case "UB":
			keys = append(keys, "DirUp", "DirBack")
		case "a", "b", "c", "x", "y", "z":
			keys = append(keys, "Button"+strings.ToUpper(part))
		case "s":
			keys = append(keys, "ButtonStart")
		}
	}
	return keys
}

func (cb *CommandBuffer) tickMatches(tickIdx int, target string) bool {
	if tickIdx < 0 || tickIdx >= len(cb.History) {
		return false
	}

	target = strings.TrimPrefix(strings.TrimSpace(target), ">")
	if strings.Contains(target, "+") {
		for _, part := range strings.Split(target, "+") {
			if !cb.tickMatches(tickIdx, part) {
				return false
			}
		}
		return true
	}
	hold := strings.HasPrefix(target, "/")
	release := strings.HasPrefix(target, "~")
	directionOnly := strings.HasPrefix(target, "$") || strings.Contains(target, "$")

	// Strip modifiers for base target
	base := target
	base = strings.TrimPrefix(base, "/")
	base = strings.TrimPrefix(base, "~")
	charge := 1
	if release {
		digits := 0
		for digits < len(base) && base[digits] >= '0' && base[digits] <= '9' {
			digits++
		}
		if digits > 0 {
			n, err := strconv.Atoi(base[:digits])
			if err != nil || n > len(cb.History) {
				return false
			}
			charge = max(1, n)
			base = base[digits:]
		}
	}
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
			return has(idx, "DirForward") && !has(idx, "DirUp") && !has(idx, "DirDown")
		case "B":
			return has(idx, "DirBack") && !has(idx, "DirUp") && !has(idx, "DirDown")
		case "U":
			return has(idx, "DirUp") && !has(idx, "DirForward") && !has(idx, "DirBack")
		case "D":
			return has(idx, "DirDown") && !has(idx, "DirForward") && !has(idx, "DirBack")
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
		if matchBase(tickIdx, base) {
			return false
		}
		for tick := tickIdx - 1; tick >= tickIdx-charge; tick-- {
			if !matchBase(tick, base) {
				return false
			}
		}
		return true
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

func (c *Character) updateCommands(isPaused bool) {
	c.ActiveCommands = nil
	c.LastCommandMatched = ""
	if c.Parent != nil && !c.isProjectile && !c.HelperKeyControl {
		return
	}
	if c.Commands != nil && c.CmdFile != nil {
		var physicalRaw []string
		if c.InputSource != nil {
			physicalRaw = c.InputSource(c.InputSlot, c.Facing)
		} else {
			physicalRaw = GetRawInputs(c.InputSlot, c.Facing)
		}
		raw := c.CmdFile.RemapInputs(physicalRaw)
		c.updateRecoveryInput(raw)

		// Preserve actual press/release transitions. An unconditional direction
		// latch changes D -> DF -> F into extra held diagonals and breaks
		// rolling motions, charged releases and strict double taps. Authored
		// time/buffer/hold modifiers provide command leniency instead.

		sort.Strings(raw)
		c.Commands.Add(raw)
		c.matchCurrentCommands()
	}
}

// Turning changes forward/back for the current physical input, without adding
// another simulation tick or rewriting earlier motion/charge history.
func (c *Character) turnCommandFrame() {
	if c.Commands == nil || c.CmdFile == nil || len(c.Commands.History) == 0 {
		return
	}
	last := len(c.Commands.History) - 1
	frame := append([]string(nil), c.Commands.History[last]...)
	for i, key := range frame {
		if key == "DirForward" {
			frame[i] = "DirBack"
		} else if key == "DirBack" {
			frame[i] = "DirForward"
		}
	}
	sort.Strings(frame)
	c.Commands.History[last] = frame
	c.matchCurrentCommands()
}

func (c *Character) matchCurrentCommands() {
	c.ActiveCommands = nil
	c.LastCommandMatched = ""
	if c.Commands != nil && c.CmdFile != nil && len(c.Commands.History) > 0 {
		var matched []string
		for name, cmdList := range c.CmdFile.Commands {
			for _, cmd := range cmdList {
				// MUGEN command matching
				if c.Commands.Match(cmd.Sequence, cmd.Time, cmd.BufferTime) {
					c.ActiveCommands = append(c.ActiveCommands, name)
					matched = append(matched, name)
					break // Found a match for this command name
				}
			}
		}
		// Turning may rematch held directions on the pause-release pass.
		// Its previously buffered command must still reach this CNS pass.
		for name := range c.pauseCommands {
			if !c.Command(name) {
				c.ActiveCommands = append(c.ActiveCommands, name)
				matched = append(matched, name)
			}
		}
		if len(matched) > 0 {
			sort.Strings(matched)
			c.LastCommandMatched = strings.Join(matched, ", ")
		}
	}
}
