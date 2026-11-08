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
	Archive         *sff.SFF
	Revision        uint64
	MappingRevision uint64 // only changes when MappedImage pixels can change
	mappings        map[ID]ID
	legacyInitial   bool
	hitFX           HitEffect
	controllerFX    Effect
}

func New(archive *sff.SFF) *State { return &State{Archive: archive, mappings: map[ID]ID{}} }

func (s *State) Clone() *State {
	copy := New(s.Archive)
	copy.Revision = s.Revision
	copy.MappingRevision = s.MappingRevision
	copy.legacyInitial = s.legacyInitial
	copy.hitFX = s.hitFX
	copy.controllerFX = s.controllerFX
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

// SelectLegacy marks the engine's initial V1 outfit separately from authored
// RemapPal calls: native V1 HUD portraits retain their default bank at entry.
func (s *State) SelectLegacy(dest ID) error {
	if err := s.Remap(ID{1, 1}, dest); err != nil {
		return err
	}
	initial := dest != (ID{1, 1})
	if s.legacyInitial != initial {
		s.legacyInitial = initial
		s.mappingChanged()
	}
	return nil
}

func (s *State) mappingChanged() {
	s.Revision++
	s.MappingRevision++
}

func (s *State) Remap(source, dest ID) (result error) {
	clearInitial := s.legacyInitial && (source == (ID{1, 1}) || source[0] == -1)
	defer func() {
		if result == nil && clearInitial {
			s.legacyInitial = false
			s.mappingChanged()
		}
	}()

	wildcard := ID{-1, 0}
	if source[0] == -1 {
		source = wildcard
	}
	if dest[0] == -1 || source == dest {
		if source == wildcard {
			if len(s.mappings) > 0 {
				clear(s.mappings)
				s.mappingChanged()
			}
		} else if _, present := s.mappings[source]; present {
			delete(s.mappings, source)
			s.mappingChanged()
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
	s.mappingChanged()
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
	if err != nil || pixels == nil {
		return pixels, err
	}
	if s.controllerFX.Time != 0 {
		return s.Transform().Image(pixels), nil
	}
	if s.hitFX.Time == 0 {
		return pixels, nil
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
		if s.legacyInitial && s.Archive.Version[0] == 1 && sprite.GroupNumber == 9000 && source == (ID{1, 1}) {
			dest = source
		}
		if dest != source {
			paletteIndex, _ = s.index(dest)
		}
	}
	return s.Archive.SpriteImage(index, paletteIndex, masked)
}
