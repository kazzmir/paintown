package sff

import (
	"encoding/binary"
	"fmt"
	"image"
	"image/color"
	"io"

	"github.com/kazzmir/paintown/game/mugen/logger"
)

type Sprite struct {
	GroupNumber  uint16
	ImageNumber  uint16
	XAxis        int16
	YAxis        int16
	Linked       bool
	Image        image.Image
	ImageOpaque  image.Image
	PaletteIndex int
	Format       uint8
	ColorDepth   uint8
}

type SFF struct {
	Sprites       []Sprite
	SharedPalette color.Palette
	Version       [4]byte // major, minor, revision, patch
	Palettes      []Palette
	spriteIndex   map[[2]uint16]int // built once after parsing immutable metadata
}

// ParseWithPalette is like Parse but uses an initial palette (e.g. from an .act file).
func ParseWithPalette(r io.ReadSeeker, initialPalette color.Palette) (*SFF, error) {
	return parseWithSelection(r, initialPalette, nil)
}

// ParsePortraits retains archive metadata but decodes only 9000,0/9000,1 and
// their linked pixel sources. Character selection need not decode fight frames.
// Gameplay uses Parse to load and validate the complete sprite set.
func ParsePortraits(r io.ReadSeeker) (*SFF, error) {
	return parseWithSelection(r, nil, func(group, item uint16) bool {
		return group == 9000 && item <= 1
	})
}

type spriteSelection func(group, item uint16) bool

type v1Source struct {
	offset, length  int64
	palette, opaque color.Palette
	embeddedPalette bool
}

func parseWithSelection(r io.ReadSeeker, initialPalette color.Palette, selected spriteSelection) (*SFF, error) {
	var header [512]byte
	if _, err := io.ReadFull(r, header[:]); err != nil {
		return nil, fmt.Errorf("read header: %w", err)
	}

	sig := string(header[:11])
	if sig != "ElecbyteSpr" {
		return nil, fmt.Errorf("invalid signature: %s", sig)
	}

	vHi := header[15]
	if vHi == 2 {
		return parseV2(r, header[:], selected)
	}
	if vHi != 1 {
		logger.LogUnresolved(fmt.Sprintf("SFF Error: Unsupported version %d.%d", vHi, header[12]))
		return nil, fmt.Errorf("unsupported SFF version: %d", vHi)
	}

	totalImages := binary.LittleEndian.Uint32(header[20:24])
	suboffset := binary.LittleEndian.Uint32(header[24:28])
	size, err := r.Seek(0, io.SeekEnd)
	if err != nil {
		return nil, err
	}
	if int64(totalImages) > size/32 || totalImages > 65536 {
		return nil, fmt.Errorf("SFF v1 sprite count %d exceeds file/index limits", totalImages)
	}

	sff := &SFF{
		Version:       [4]byte{header[15], header[14], header[13], header[12]},
		Sprites:       make([]Sprite, 0, totalImages),
		SharedPalette: initialPalette,
	}

	location := int64(suboffset)
	if location < 512 {
		location = 512
	}

	currentPalette := initialPalette
	currentShared := initialPalette != nil
	spriteIndex := make(map[uint16]*Sprite)
	sources := make([]v1Source, totalImages)
	owners := make([]uint16, totalImages)
	decodedPixels := int64(0)
	decode := func(index uint16, data []byte) error {
		sprite := &sff.Sprites[index]
		if sprite.Image != nil {
			return nil
		}
		source := sources[index]
		if data == nil {
			data, err = readRegion(r, size, source.offset, source.length)
			if err != nil {
				return fmt.Errorf("sprite %d (%d,%d): %w", index, sprite.GroupNumber, sprite.ImageNumber, err)
			}
		}
		width, height, _, err := pcxLayout(data)
		if err != nil {
			return fmt.Errorf("sprite %d (%d,%d): %w", index, sprite.GroupNumber, sprite.ImageNumber, err)
		}
		pixels := int64(width) * int64(height)
		if pixels > maxArchivePixels-decodedPixels {
			return fmt.Errorf("sprite %d (%d,%d): decoded archive exceeds %d pixels", index, sprite.GroupNumber, sprite.ImageNumber, maxArchivePixels)
		}
		img, err := decodePCX(data, source.palette, source.embeddedPalette)
		if err != nil {
			return fmt.Errorf("sprite %d (%d,%d): %w", index, sprite.GroupNumber, sprite.ImageNumber, err)
		}
		// Linked sprites share this plane. Selective portrait loading charges
		// only decoded sources, including an earlier non-portrait link owner.
		decodedPixels += pixels
		opaque := *img
		opaque.Palette = source.opaque
		if opaque.Palette == nil {
			opaque.Palette = opaquePalette(img.Palette)
		}
		sprite.Image, sprite.ImageOpaque = img, &opaque
		return nil
	}
	var currentIndex uint16 = 0

	for i := uint32(0); i < totalImages; i++ {
		if _, err := r.Seek(location, io.SeekStart); err != nil {
			return nil, err
		}

		var sub [32]byte
		if _, err := io.ReadFull(r, sub[:]); err != nil {
			return nil, fmt.Errorf("sprite %d header: %w", i, err)
		}

		nextLoc := binary.LittleEndian.Uint32(sub[0:4])
		length := binary.LittleEndian.Uint32(sub[4:8])
		x := int16(binary.LittleEndian.Uint16(sub[8:10]))
		y := int16(binary.LittleEndian.Uint16(sub[10:12]))
		group := binary.LittleEndian.Uint16(sub[12:14])
		item := binary.LittleEndian.Uint16(sub[14:16])
		prev := binary.LittleEndian.Uint16(sub[16:18])
		samePalette := sub[18] == 1
		wanted := selected == nil || selected(group, item)
		if nextLoc != 0 && (int64(nextLoc) < location+32 || int64(nextLoc) > size) {
			return nil, fmt.Errorf("sprite %d: invalid next offset %d", i, nextLoc)
		}

		if length == 0 {
			// Linked sprite (points to a previous PCX decoding)
			linkedSprite, ok := spriteIndex[prev]
			if !ok {
				return nil, fmt.Errorf("sprite %d (%d,%d): invalid link %d", i, group, item, prev)
			} else {
				owner := owners[prev]
				if wanted {
					if err := decode(owner, nil); err != nil {
						return nil, err
					}
				}
				newSprite := *linkedSprite
				newSprite.Image, newSprite.ImageOpaque = nil, nil
				if wanted {
					newSprite.Image, newSprite.ImageOpaque = sff.Sprites[owner].Image, sff.Sprites[owner].ImageOpaque
				}
				newSprite.GroupNumber = group
				newSprite.ImageNumber = item
				newSprite.XAxis = x
				newSprite.YAxis = y
				newSprite.Linked = true
				sff.Sprites = append(sff.Sprites, newSprite)
				owners[currentIndex] = owner
			}
			spriteIndex[currentIndex] = &sff.Sprites[len(sff.Sprites)-1]
		} else {
			// Calculate actual PCX data size.
			// In SFF v1, the 'length' field in the subheader can be unreliable.
			// The reference implementation uses the distance to the next sprite.
			pcxLen := int64(length)
			if nextLoc != 0 {
				pcxLen = int64(nextLoc) - location - 32
			}

			if pcxLen < 0 || pcxLen > 256<<20 || location+32 > size || pcxLen > size-location-32 {
				return nil, fmt.Errorf("sprite %d (%d,%d): invalid PCX length %d", i, group, item, pcxLen)
			}

			// Unselected v1 frames only contribute palette state. Read their
			// small palette trailer, retaining an offset if a later portrait
			// links to their pixels. Shared-palette frames require no read.
			var pcxData []byte
			if wanted || !samePalette {
				offset, length := location+32, pcxLen
				if !wanted && length > 769 {
					offset, length = offset+length-769, 769
				}
				pcxData, err = readRegion(r, size, offset, length)
				if err != nil {
					return nil, fmt.Errorf("sprite %d (%d,%d): %w", i, group, item, err)
				}
			}

			// MUGEN SFF v1 Palette Rules:
			// 1. Portraits (Group 9000) use their own palette and NEVER update currentPalette/SharedPalette.
			// 2. If an ACT palette is provided, (0,0) MUST use it, even if samePalette is false.
			// 3. Otherwise, if samePalette is false, extract the PCX palette and update the shared state.
			var palToUse color.Palette
			var palToUseOpaque color.Palette
			shared := currentShared

			if group == 9000 {
				if !samePalette {
					shared = false
					palToUse, _ = ExtractPalette(pcxData, true)
					palToUseOpaque, _ = ExtractPalette(pcxData, false)
					// Portraits don't update currentPalette/SharedPalette
				} else {
					palToUse = currentPalette
					palToUseOpaque = currentPalette
				}
			} else {
				// Combat sprites
				if !samePalette {
					palM, _ := ExtractPalette(pcxData, true)
					palO, _ := ExtractPalette(pcxData, false)
					if palM != nil {
						// Case: (0,0) and initialPalette exists -> ignore internal PCX palette
						if group == 0 && item == 0 && initialPalette != nil {
							currentPalette = initialPalette
							currentShared = true
						} else {
							currentPalette = palM
							currentShared = sff.SharedPalette == nil
						}
						palToUse = currentPalette
						shared = currentShared
						palToUseOpaque = palO

						if sff.SharedPalette == nil {
							sff.SharedPalette = currentPalette
						}
					}
				} else {
					// samePalette == true
					if currentPalette != nil {
						palToUse = currentPalette
						palToUseOpaque = currentPalette
					} else {
						// Fallback to shared
						palToUse = sff.SharedPalette
						palToUseOpaque = sff.SharedPalette
					}
				}
			}

			if palToUse == nil {
				shared = true
				palToUse = sff.SharedPalette
			}
			if palToUseOpaque == nil {
				palToUseOpaque = palToUse
			}
			palToUseOpaque = opaquePalette(palToUseOpaque)

			paletteIndex := -1
			if shared {
				paletteIndex = 0
			}
			sff.Sprites = append(sff.Sprites, Sprite{PaletteIndex: paletteIndex, GroupNumber: group, ImageNumber: item, XAxis: x, YAxis: y})
			owners[currentIndex] = currentIndex
			sources[currentIndex] = v1Source{location + 32, pcxLen, palToUse, palToUseOpaque, !samePalette}
			if wanted {
				if err := decode(currentIndex, pcxData); err != nil {
					return nil, err
				}
			}
			spriteIndex[currentIndex] = &sff.Sprites[len(sff.Sprites)-1]
		}

		currentIndex++
		location = int64(nextLoc)
		if location == 0 {
			if i+1 != totalImages {
				return nil, fmt.Errorf("SFF v1 sprite chain ends at %d of %d sprites", i+1, totalImages)
			}
			break
		}
	}

	if sff.SharedPalette != nil {
		sff.Palettes = []Palette{{GroupNumber: 1, ItemNumber: 1, Colors: opaquePalette(sff.SharedPalette)}}
	}
	sff.indexSprites()
	return sff, nil
}

func (s *SFF) indexSprites() {
	s.spriteIndex = make(map[[2]uint16]int, len(s.Sprites))
	for i, sprite := range s.Sprites {
		key := [2]uint16{sprite.GroupNumber, sprite.ImageNumber}
		if _, exists := s.spriteIndex[key]; !exists {
			s.spriteIndex[key] = i
		}
	}
}

// Index preserves the first matching sprite and rejects negative AIR sentinels.
// Parsed archives have immutable metadata and an index. Caller-built archives
// retain a linear fallback, so incrementally authored sprites remain usable.
func (s *SFF) Index(group, item int) (int, bool) {
	if s == nil || group < 0 || item < 0 || group > 65535 || item > 65535 {
		return 0, false
	}
	key := [2]uint16{uint16(group), uint16(item)}
	if s.spriteIndex != nil {
		i, ok := s.spriteIndex[key]
		return i, ok
	}
	for i, sprite := range s.Sprites {
		if sprite.GroupNumber == key[0] && sprite.ImageNumber == key[1] {
			return i, true
		}
	}
	return 0, false
}

// Parse slices an SFF file into individual sprites.
// r must support Seeking.
func Parse(r io.ReadSeeker) (*SFF, error) {
	return ParseWithPalette(r, nil)
}
