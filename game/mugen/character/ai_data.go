package character

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"math"

	"github.com/kazzmir/paintown/game/mugen/cmd"
	"github.com/kazzmir/paintown/game/mugen/combat"
)

// ReadAIMoves reads the beta's optional learned-move cache. The sample's old
// "unused hints" comment does not describe the executable's actual consumer.
// Records contain game-pixel estimates; the format carries no resolution tag.
func ReadAIMoves(r io.Reader, commands *cmd.Data, width int) (*AIMoveLibrary, error) {
	const recordSize = 1204
	const maximum = 64 + 8 + (80+40)*recordSize + 256
	data, err := io.ReadAll(io.LimitReader(r, maximum+1))
	if err != nil {
		return nil, err
	}
	if len(data) > maximum || len(data) < 64 || !bytes.Equal(data[:12], []byte("ElecbyteAIB\x00")) {
		return nil, fmt.Errorf("invalid AI cache header or size")
	}
	word := func(offset int) uint32 { return binary.LittleEndian.Uint32(data[offset:]) }
	if word(12) != 0x100 || binary.LittleEndian.Uint16(data[16:]) != 2013 || data[18] != 8 || data[19] != 11 {
		return nil, fmt.Errorf("unsupported AI cache version")
	}
	library := NewAIMoveLibrary(commands, width)
	offset := 64
	for bank, limit := range []int{80, 40} {
		if len(data)-offset < 4 {
			return nil, fmt.Errorf("AI bank %d is truncated", bank)
		}
		count := int(word(offset))
		offset += 4
		if count > limit || count > (len(data)-offset)/recordSize {
			return nil, fmt.Errorf("AI bank %d has invalid record count %d", bank, count)
		}
		for index := 0; index < count; index++ {
			move, err := readAIMove(data[offset : offset+recordSize])
			if err != nil {
				return nil, fmt.Errorf("AI bank %d record %d: %w", bank, index, err)
			}
			offset += recordSize
			if bank == 1 {
				move.StateType = "A"
			} else if move.StateType != "C" {
				move.StateType = "S"
			}
			library.complete(move)
		}
	}
	if len(data)-offset < 256 {
		return nil, fmt.Errorf("AI common pose bounds are truncated")
	}
	for pose := range library.Poses {
		bounds, err := readAIBounds(data[offset+pose*64:])
		if err != nil {
			return nil, fmt.Errorf("AI common pose %d: %w", pose, err)
		}
		library.Poses[pose] = bounds
	}
	return library, nil
}

func readAIStatistic(data []byte, valid bool) (AIStatistic, error) {
	mean := math.Float32frombits(binary.LittleEndian.Uint32(data))
	deviation := math.Float32frombits(binary.LittleEndian.Uint32(data[12:]))
	if math.IsNaN(float64(mean)) || math.IsInf(float64(mean), 0) || math.IsNaN(float64(deviation)) || math.IsInf(float64(deviation), 0) {
		return AIStatistic{}, fmt.Errorf("non-finite AI estimate")
	}
	return AIStatistic{Mean: mean, Min: int(int32(binary.LittleEndian.Uint32(data[4:]))), Max: int(int32(binary.LittleEndian.Uint32(data[8:]))), Deviation: deviation, Valid: valid}, nil
}

func readAIPhase(data []byte, startup bool) (AIPhase, error) {
	valid := startup || binary.LittleEndian.Uint32(data) != 0
	var phase AIPhase
	for axis, value := range []*AIStatistic{&phase.Time, &phase.X, &phase.Y} {
		s, err := readAIStatistic(data[axis*16:], valid)
		if err != nil {
			return phase, err
		}
		*value = s
	}
	if phase.Time.Mean < 0 || phase.Time.Mean > math.MaxInt32 {
		return phase, fmt.Errorf("invalid AI duration")
	}
	return phase, nil
}

func readAIBounds(data []byte) (AIBounds, error) {
	var values [4]float32
	for index := range values {
		s, err := readAIStatistic(data[index*16:], true)
		if err != nil {
			return AIBounds{}, err
		}
		values[index] = s.Mean
	}
	if values[0] == -math.MaxFloat32 {
		return AIBounds{}, nil
	}
	if values[0] > values[2] || values[1] > values[3] {
		return AIBounds{}, fmt.Errorf("reversed AI bounds")
	}
	return AIBounds{values[0], values[1], values[2], values[3], true}, nil
}

func readAIMove(data []byte) (AIMove, error) {
	word := func(offset int) int { return int(int32(binary.LittleEndian.Uint32(data[offset:]))) }
	stateType := func(n int) string {
		if n >= 0 && n < 5 {
			return [...]string{"", "S", "C", "A", "L"}[n]
		}
		return ""
	}
	move := AIMove{StateNo: word(516), StateType: stateType(word(520)), CommandIndex: word(524)}
	count := word(512)
	if count < 0 || count > 32 || move.StateNo < 0 || move.CommandIndex < 0 || move.CommandIndex >= 128 {
		return move, fmt.Errorf("invalid AI state, command index or context count")
	}
	for index := 0; index < count; index++ {
		offset := index * 16
		kind := ""
		if n := word(offset + 8); n >= 0 && n < 3 {
			kind = [...]string{"I", "A", "H"}[n]
		}
		move.Contexts = append(move.Contexts, AIEntryContext{word(offset), stateType(word(offset + 4)), kind, word(offset+12) != 0})
	}
	var err error
	move.Startup, err = readAIPhase(data[564:], true)
	if err != nil {
		return move, err
	}
	for outcome := range move.Active {
		for group, phase := range []*AIPhase{&move.Active[outcome], &move.Control[outcome], &move.End[outcome]} {
			*phase, err = readAIPhase(data[[...]int{612, 916, 1060}[group]+outcome*48:], false)
			if err != nil {
				return move, err
			}
		}
	}
	move.Attack, err = readAIBounds(data[756:])
	if err != nil {
		return move, err
	}
	move.Body, err = readAIBounds(data[820:])
	if err != nil {
		return move, err
	}
	move.Damage, err = readAIStatistic(data[900:], true)
	move.HitFlag, move.GuardFlag = aiContactFlags(uint32(word(884))), aiContactFlags(uint32(word(888)))
	move.Attribute, move.ReversalAttribute = aiAttributes(uint32(word(892))), aiAttributes(uint32(word(896)))
	return move, err
}

func aiContactFlags(mask uint32) string {
	var flags string
	if mask&14 == 14 {
		flags = "M"
		mask &^= 14
	}
	for index, letter := range "HLAFD" {
		if mask&(4<<index) != 0 {
			flags += string(letter)
		}
	}
	return flags
}

func aiAttributes(mask uint32) string {
	a := combat.Attributes{States: uint8(mask & 7)}
	for level := 0; level < 3; level++ {
		for kind := 0; kind < 3; kind++ {
			if mask&(1<<uint(3+kind*3+level)) != 0 {
				a.Attacks |= 1 << uint(level*3+kind)
			}
		}
	}
	if a.Empty() {
		return ""
	}
	return a.String()
}
