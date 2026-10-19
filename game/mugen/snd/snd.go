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
	Wave  *Wave
}

type SoundBank struct {
	Sounds  []Sound
	Map     map[string]*Sound
	Version uint32
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
	size, err := r.Seek(0, io.SeekEnd)
	if err != nil {
		return nil, err
	}
	if int64(header.NumSounds) > size/16 {
		return nil, fmt.Errorf("SND sound count %d exceeds file size", header.NumSounds)
	}

	bank := &SoundBank{
		Sounds:  make([]Sound, 0, header.NumSounds),
		Map:     make(map[string]*Sound),
		Version: binary.LittleEndian.Uint32(header.Version[:]),
	}

	currentOffset := int64(header.FirstOffset)
	seen := make(map[int64]bool)
	var total int64

	for i := uint32(0); i < header.NumSounds; i++ {
		if currentOffset == 0 {
			return nil, fmt.Errorf("SND chain ended at %d of %d sounds", i, header.NumSounds)
		}
		if seen[currentOffset] {
			return nil, fmt.Errorf("sound %d: cyclic SND link at %d", i, currentOffset)
		}
		seen[currentOffset] = true
		if currentOffset < 24 || currentOffset > size-16 {
			return nil, fmt.Errorf("sound %d: subheader offset %d outside file", i, currentOffset)
		}

		if _, err := r.Seek(currentOffset, io.SeekStart); err != nil {
			return nil, fmt.Errorf("failed to seek to sound %d offset %d: %v", i, currentOffset, err)
		}

		var sub subHeader
		if err := binary.Read(r, binary.LittleEndian, &sub); err != nil {
			return nil, fmt.Errorf("failed to read subheader for sound %d: %v", i, err)
		}
		total += int64(sub.Length)
		if int64(sub.Length) > size-currentOffset-16 || sub.Length > 256<<20 || total > 512<<20 {
			return nil, fmt.Errorf("sound %d (%d,%d): invalid/excessive length %d", i, sub.GroupID, sub.ItemID, sub.Length)
		}

		soundData := make([]byte, sub.Length)
		if _, err := io.ReadFull(r, soundData); err != nil {
			return nil, fmt.Errorf("failed to read raw wav block for sound %d (Group: %d, Item: %d): %v", i, sub.GroupID, sub.ItemID, err)
		}
		wave, err := ParseWAV(soundData)
		if err != nil {
			return nil, fmt.Errorf("sound %d (%d,%d): %w", i, sub.GroupID, sub.ItemID, err)
		}

		snd := Sound{
			Group: sub.GroupID,
			Item:  sub.ItemID,
			Data:  soundData,
			Wave:  wave,
		}

		bank.Sounds = append(bank.Sounds, snd)

		key := fmt.Sprintf("%d-%d", snd.Group, snd.Item)
		bank.Map[key] = &bank.Sounds[len(bank.Sounds)-1]

		currentOffset = int64(sub.NextOffset)
	}
	// Stock sndmaker archives terminate with the physical EOF offset. Other
	// writers use zero; neither sentinel points to another sound header.
	if header.NumSounds > 0 && currentOffset != 0 && currentOffset != size {
		return nil, fmt.Errorf("SND chain exceeds declared sound count %d", header.NumSounds)
	}

	return bank, nil
}
