package sff

import (
	"encoding/binary"
	"fmt"
	"image"
	"image/color"
	"io"
)

type Sprite struct {
	GroupNumber uint16
	ImageNumber uint16
	XAxis       uint16
	YAxis       uint16
	Linked      bool
	Image       image.Image
	ImageOpaque image.Image
}

type SFF struct {
	Sprites       []Sprite
	SharedPalette color.Palette
}

// ParseWithPalette is like Parse but uses an initial palette (e.g. from an .act file).
func ParseWithPalette(r io.ReadSeeker, initialPalette color.Palette) (*SFF, error) {
	var header [512]byte
	if _, err := io.ReadFull(r, header[:]); err != nil {
		return nil, fmt.Errorf("read header: %w", err)
	}

	sig := string(header[:11])
	if sig != "ElecbyteSpr" {
		return nil, fmt.Errorf("invalid signature: %s", sig)
	}

	vHi := header[15]
	if vHi != 1 {
		return nil, fmt.Errorf("unsupported SFF version: %d", vHi)
	}

	totalImages := binary.LittleEndian.Uint32(header[20:24])
	suboffset := binary.LittleEndian.Uint32(header[24:28])

	sff := &SFF{
		Sprites:       make([]Sprite, 0, totalImages),
		SharedPalette: initialPalette,
	}

	location := int64(suboffset)
	if location < 512 {
		location = 512
	}

	currentPalette := initialPalette
	var initialPaletteOpaque color.Palette
	if initialPalette != nil {
		// Create opaque version of initialPalette?
		// Usually ACT palettes are already handled.
		// If we set index 0 to Transparent in ReadPaletteACT, we need the original.
		// Actually, let's assume if initialPalette exists, currentPaletteOpaque should be it without index 0 transparency.
		// But initialPalette is color.Palette (interface).
	}
	currentPaletteOpaque := initialPalette
	spriteIndex := make(map[uint16]*Sprite)
	var currentIndex uint16 = 0

	for i := uint32(0); i < totalImages; i++ {
		if _, err := r.Seek(location, io.SeekStart); err != nil {
			return nil, err
		}

		var sub [32]byte
		if _, err := io.ReadFull(r, sub[:]); err != nil {
			// eof before subheader
			break
		}

		nextLoc := binary.LittleEndian.Uint32(sub[0:4])
		length := binary.LittleEndian.Uint32(sub[4:8])
		x := binary.LittleEndian.Uint16(sub[8:10])
		y := binary.LittleEndian.Uint16(sub[10:12])
		group := binary.LittleEndian.Uint16(sub[12:14])
		item := binary.LittleEndian.Uint16(sub[14:16])
		prev := binary.LittleEndian.Uint16(sub[16:18])
		samePalette := sub[18] == 1

		if length == 0 {
			// Linked sprite (points to a previous PCX decoding)
			linkedSprite, ok := spriteIndex[prev]
			if !ok {
				// Warn but don't fail, maybe just skip image linkage
				sff.Sprites = append(sff.Sprites, Sprite{
					GroupNumber: group,
					ImageNumber: item,
					XAxis:       x,
					YAxis:       y,
					Linked:      true,
				})
			} else {
				newSprite := *linkedSprite
				newSprite.GroupNumber = group
				newSprite.ImageNumber = item
				newSprite.XAxis = x
				newSprite.YAxis = y
				newSprite.Linked = true
				sff.Sprites = append(sff.Sprites, newSprite)
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

			if pcxLen < 0 {
				// Should not happen with valid SFF
				break
			}

			pcxData := make([]byte, pcxLen)
			if _, err := io.ReadFull(r, pcxData); err != nil {
				// eof or read error
			}

			palToUse := currentPalette
			palToUseOpaque := currentPaletteOpaque

			// MUGEN SFF v1 Palette Rules:
			// 1. Portraits (Group 9000) use their own palette and NEVER update currentPalette/SharedPalette.
			// 2. If an ACT palette is provided, (0,0) MUST use it, even if samePalette is false.
			// 3. Otherwise, if samePalette is false, extract the PCX palette and update the shared state.

			if group == 9000 {
				if !samePalette {
					if pal, err := ExtractPalette(pcxData, true); err == nil {
						palToUse = pal
					}
					if pal, err := ExtractPalette(pcxData, false); err == nil {
						palToUseOpaque = pal
					}
				}
			} else {
				// Combat sprites
				if !samePalette {
					palM, errM := ExtractPalette(pcxData, true)
					palO, errO := ExtractPalette(pcxData, false)
					if errM == nil && errO == nil {
						// Case: (0,0) and initialPalette exists -> ignore internal PCX palette
						if group == 0 && item == 0 && initialPalette != nil {
							currentPalette = initialPalette
							currentPaletteOpaque = initialPaletteOpaque
							palToUse = initialPalette
							palToUseOpaque = initialPaletteOpaque
						} else {
							currentPalette = palM
							currentPaletteOpaque = palO
							palToUse = palM
							palToUseOpaque = palO
						}

						if sff.SharedPalette == nil {
							sff.SharedPalette = currentPalette
						}
					}
				} else {
					// samePalette == true
					if group == 0 && item == 0 && initialPalette != nil {
						currentPalette = initialPalette
						currentPaletteOpaque = initialPaletteOpaque
						palToUse = initialPalette
						palToUseOpaque = initialPaletteOpaque
					}
				}
			}

			if palToUse == nil {
				palToUse = sff.SharedPalette
			}
			if palToUseOpaque == nil {
				// Fallback to masked if opaque not available, or if initialPaletteOpaque was nil.
				// This might happen if ExtractPalette(..., false) failed or wasn't called.
				// In such cases, we can try to derive an opaque version from palToUse.
				if palToUse != nil {
					derivedOpaque := make(color.Palette, len(palToUse))
					copy(derivedOpaque, palToUse)
					if len(derivedOpaque) > 0 {
						if rgba, ok := derivedOpaque[0].(color.RGBA); ok {
							rgba.A = 255
							derivedOpaque[0] = rgba
						} else {
							derivedOpaque = palToUse // Fallback
						}
					}
					palToUseOpaque = derivedOpaque
				} else {
					// If even palToUse is nil, then there's no palette at all.
					// This should ideally not happen if sff.SharedPalette is set.
					palToUseOpaque = nil
				}
			}

			img, err := DecodePCX(pcxData, palToUse)
			imgOpaque, _ := DecodePCX(pcxData, palToUseOpaque) // Ignore error for opaque, use nil if it fails.
			if err == nil {
				sff.Sprites = append(sff.Sprites, Sprite{
					GroupNumber: group,
					ImageNumber: item,
					XAxis:       x,
					YAxis:       y,
					Linked:      false,
					Image:       img,
					ImageOpaque: imgOpaque,
				})
			} else {
				fmt.Printf("Decode error Group %d Item %d: %v\n", group, item, err)
				// Just store empty if decode fails
				sff.Sprites = append(sff.Sprites, Sprite{
					GroupNumber: group,
					ImageNumber: item,
					XAxis:       x,
					YAxis:       y,
					Linked:      false,
				})
			}
			spriteIndex[currentIndex] = &sff.Sprites[len(sff.Sprites)-1]
		}

		currentIndex++
		location = int64(nextLoc)
		if location == 0 {
			break
		}
	}

	return sff, nil
}

// Parse slices an SFF file into individual sprites.
// r must support Seeking.
func Parse(r io.ReadSeeker) (*SFF, error) {
	return ParseWithPalette(r, nil)
}
