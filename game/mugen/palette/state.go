// Package palette owns mutable palette mappings over an immutable SFF archive.
package palette

import (
	"fmt"
	"image"
	"math/bits"

	"github.com/kazzmir/paintown/game/mugen/sff"
)

type ID [2]int

type State struct {
	Archive  *sff.SFF
	Revision uint64
	mappings map[ID]ID
	hitFX    HitEffect
}

func New(archive *sff.SFF) *State { return &State{Archive: archive, mappings: map[ID]ID{}} }

func (s *State) Clone() *State {
	copy := New(s.Archive)
	copy.Revision = s.Revision
	copy.hitFX = s.hitFX
	for source, dest := range s.mappings {
		copy.mappings[source] = dest
	}
	return copy
}

func (s *State) index(id ID) (int, bool) {
	if s.Archive != nil {
		for index, p := range s.Archive.Palettes {
			if int(p.GroupNumber) == id[0] && int(p.ItemNumber) == id[1] {
				return index, true
			}
		}
	}
	return 0, false
}

func (s *State) Remap(source, dest ID) error {
	wildcard := ID{-1, 0}
	if source[0] == -1 {
		source = wildcard
	}
	if dest[0] == -1 || source == dest {
		if source == wildcard {
			if len(s.mappings) > 0 {
				clear(s.mappings)
				s.Revision++
			}
		} else if _, present := s.mappings[source]; present {
			delete(s.mappings, source)
			s.Revision++
		}
		return nil
	}
	destIndex, found := s.index(dest)
	if !found {
		return fmt.Errorf("destination palette %v does not exist", dest)
	}
	if source != wildcard {
		sourceIndex, found := s.index(source)
		if !found {
			return fmt.Errorf("source palette %v does not exist", source)
		}
		if bits.Len(uint(len(s.Archive.Palettes[sourceIndex].Colors)-1)) != bits.Len(uint(len(s.Archive.Palettes[destIndex].Colors)-1)) {
			return fmt.Errorf("palette %v and %v have different color depths", source, dest)
		}
		if _, present := s.mappings[source]; !present && len(s.mappings) >= 8 {
			return fmt.Errorf("eight palette mappings are already active")
		}
	} else {
		if len(s.mappings) == 1 && s.mappings[source] == dest {
			return nil
		}
		clear(s.mappings)
	}
	if previous, present := s.mappings[source]; present && previous == dest {
		return nil
	}
	if s.mappings == nil {
		s.mappings = map[ID]ID{}
	}
	s.mappings[source] = dest
	s.Revision++
	return nil
}

// Destination applies exactly one mapping, never following mapping chains.
func (s *State) Destination(source ID) ID {
	if dest, present := s.mappings[source]; present {
		return dest
	}
	if dest, present := s.mappings[ID{-1, 0}]; present {
		srcIndex, srcOK := s.index(source)
		dstIndex, dstOK := s.index(dest)
		if srcOK && dstOK && bits.Len(uint(len(s.Archive.Palettes[srcIndex].Colors)-1)) == bits.Len(uint(len(s.Archive.Palettes[dstIndex].Colors)-1)) {
			return dest
		}
	}
	return source
}

func (s *State) Image(index int, masked bool) (image.Image, error) {
	pixels, err := s.MappedImage(index, masked)
	if err != nil || pixels == nil || s.hitFX.Time == 0 {
		return pixels, err
	}
	return s.hitFX.Image(pixels), nil
}

// HUD portraits use palette mappings without a fighter's temporary hit effect.
func (s *State) MappedImage(index int, masked bool) (image.Image, error) {
	if s.Archive == nil {
		return nil, fmt.Errorf("palette archive unavailable")
	}
	if index < 0 || index >= len(s.Archive.Sprites) {
		return nil, fmt.Errorf("sprite index %d outside archive", index)
	}
	sprite := s.Archive.Sprites[index]
	paletteIndex := -1
	if _, indexed := sprite.Image.(*image.Paletted); indexed && sprite.PaletteIndex >= 0 && sprite.PaletteIndex < len(s.Archive.Palettes) {
		p := s.Archive.Palettes[sprite.PaletteIndex]
		source := ID{int(p.GroupNumber), int(p.ItemNumber)}
		dest := s.Destination(source)
		if dest != source {
			paletteIndex, _ = s.index(dest)
		}
	}
	return s.Archive.SpriteImage(index, paletteIndex, masked)
}
