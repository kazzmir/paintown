package snd

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
)

type Sound struct {
	Group uint32
	Item  uint32
	Data  []byte // Raw embedded WAV payload
}

type SoundBank struct {
	Sounds []Sound
	Map    map[string]*Sound
}

type fileHeader struct {
	Signature   [12]byte
	Version     [4]byte
	NumSounds   uint32
	FirstOffset uint32
}

type subHeader struct {
	NextOffset uint32
	Length     uint32
	GroupID    uint32
	ItemID     uint32
}

// Parse unmarshals a M.U.G.E.N binary .snd file
func Parse(r io.ReadSeeker) (*SoundBank, error) {
	var header fileHeader
	if err := binary.Read(r, binary.LittleEndian, &header); err != nil {
		return nil, fmt.Errorf("failed to read sequence header: %v", err)
	}

	sig := string(bytes.TrimRight(header.Signature[:], "\x00"))
	if sig != "ElecbyteSnd" {
		return nil, fmt.Errorf("invalid SND signature: expected ElecbyteSnd, got %s", sig)
	}

	bank := &SoundBank{
		Sounds: make([]Sound, 0, header.NumSounds),
		Map:    make(map[string]*Sound),
	}

	currentOffset := int64(header.FirstOffset)

	for i := uint32(0); i < header.NumSounds; i++ {
		if currentOffset == 0 {
			break // End of linked list
		}

		if _, err := r.Seek(currentOffset, io.SeekStart); err != nil {
			return nil, fmt.Errorf("failed to seek to sound %d offset %d: %v", i, currentOffset, err)
		}

		var sub subHeader
		if err := binary.Read(r, binary.LittleEndian, &sub); err != nil {
			return nil, fmt.Errorf("failed to read subheader for sound %d: %v", i, err)
		}

		soundData := make([]byte, sub.Length)
		if _, err := io.ReadFull(r, soundData); err != nil {
			return nil, fmt.Errorf("failed to read raw wav block for sound %d (Group: %d, Item: %d): %v", i, sub.GroupID, sub.ItemID, err)
		}

		snd := Sound{
			Group: sub.GroupID,
			Item:  sub.ItemID,
			Data:  soundData,
		}

		bank.Sounds = append(bank.Sounds, snd)

		key := fmt.Sprintf("%d-%d", snd.Group, snd.Item)
		bank.Map[key] = &bank.Sounds[len(bank.Sounds)-1]

		currentOffset = int64(sub.NextOffset)
	}

	return bank, nil
}
