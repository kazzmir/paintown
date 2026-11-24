//go:build linux && (amd64 || arm64)

package font

// FreeType's public LP64 ABI is shared by Linux amd64 and arm64. Loading it
// dynamically keeps development headers and additional module dependencies out
// of the portable build. API: freetype.org/freetype2/docs/reference/.
import (
	"fmt"
	"image"
	"runtime"
	"sync"
	"unsafe"

	"github.com/ebitengine/purego"
	"golang.org/x/image/font"
	"golang.org/x/image/math/fixed"
)

type ftVector struct{ X, Y int64 }
type ftBitmap struct {
	Rows, Width            uint32
	Pitch                  int32
	Buffer                 unsafe.Pointer
	Grays                  uint16
	PixelMode, PaletteMode uint8
	Palette                uintptr
}

// Only public prefixes are read, never FreeType's private driver structures.
type ftFaceRecord struct {
	Faces, Index, Flags, Style, Glyphs        int64
	Family, Name                              uintptr
	FixedSizes                                int32
	AvailableSizes                            uintptr
	Charmaps                                  int32
	CharmapTable                              uintptr
	Generic                                   [2]uintptr
	Bounds                                    [4]int64
	Units                                     uint16
	Ascender, Descender, Height               int16
	MaxWidth, MaxHeight, Underline, Thickness int16
	Glyph, Size, Charmap                      unsafe.Pointer
}
type ftGlyphSlot struct {
	Library, Face, Next uintptr
	Index               uint32
	Generic             [2]uintptr
	Metrics             [8]int64 // width, height, horizontal bearing X/Y, advance, vertical counterparts
	LinearH, LinearV    int64
	Advance             ftVector
	Format              uint32
	Bitmap              ftBitmap
	Left, Top           int32
}

var ftAPI struct {
	init      func(*unsafe.Pointer) int32
	done      func(unsafe.Pointer) int32
	newFace   func(unsafe.Pointer, unsafe.Pointer, int64, int64, *unsafe.Pointer) int32
	doneFace  func(unsafe.Pointer) int32
	setSize   func(unsafe.Pointer, int64, int64, uint32, uint32) int32
	charIndex func(unsafe.Pointer, uint64) uint32
	loadGlyph func(unsafe.Pointer, uint32, int32) int32
	render    func(unsafe.Pointer, int32) int32
	kern      func(unsafe.Pointer, uint32, uint32, uint32, *ftVector) int32
	format    func(unsafe.Pointer) string
}
var ftOnce sync.Once
var ftError error

func bindFreeType() {
	handle, err := purego.Dlopen("libfreetype.so.6", purego.RTLD_NOW|purego.RTLD_LOCAL)
	if err != nil {
		ftError = fmt.Errorf("automatic hinting needs libfreetype.so.6: %w", err)
		return
	}
	for _, entry := range []struct {
		name string
		fn   any
	}{
		{"FT_Init_FreeType", &ftAPI.init}, {"FT_Done_FreeType", &ftAPI.done},
		{"FT_New_Memory_Face", &ftAPI.newFace}, {"FT_Done_Face", &ftAPI.doneFace},
		{"FT_Set_Char_Size", &ftAPI.setSize}, {"FT_Get_Char_Index", &ftAPI.charIndex},
		{"FT_Load_Glyph", &ftAPI.loadGlyph}, {"FT_Render_Glyph", &ftAPI.render},
		{"FT_Get_Kerning", &ftAPI.kern}, {"FT_Get_Font_Format", &ftAPI.format},
	} {
		address, err := purego.Dlsym(handle, entry.name)
		if err != nil {
			ftError = fmt.Errorf("FreeType %s: %w", entry.name, err)
			purego.Dlclose(handle)
			return
		}
		purego.RegisterFunc(entry.fn, address)
	}
	// Process-global function pointers require the module to remain loaded.
}

type ftResources struct {
	mu            sync.Mutex
	library, face unsafe.Pointer
	data          []byte
	pin           runtime.Pinner
}

func (r *ftResources) close() {
	r.mu.Lock()
	defer r.mu.Unlock()
	if r.face != nil {
		ftAPI.doneFace(r.face)
		r.face = nil
	}
	if r.library != nil {
		ftAPI.done(r.library)
		r.library = nil
	}
	r.pin.Unpin()
	r.data = nil
}

type ftGlyphMetrics struct {
	index   uint32
	bounds  image.Rectangle
	advance fixed.Int26_6
}
type hintedFace struct {
	r           *ftResources
	cleanup     runtime.Cleanup
	flags, mode int32
	metrics     font.Metrics
	glyphs      map[rune]ftGlyphMetrics
	kernPairs   map[uint64]fixed.Int26_6
}

func newHintedFace(data []byte, height int, blend bool) (font.Face, error) {
	if len(data) == 0 || height < 1 || height > 2048 {
		return nil, fmt.Errorf("invalid hinted font data or height")
	}
	ftOnce.Do(bindFreeType)
	if ftError != nil {
		return nil, ftError
	}
	r := &ftResources{data: data}
	if code := ftAPI.init(&r.library); code != 0 {
		return nil, fmt.Errorf("FreeType initialization failed (%d)", code)
	}
	r.pin.Pin(&data[0]) // FT_New_Memory_Face borrows bytes until FT_Done_Face.
	if code := ftAPI.newFace(r.library, unsafe.Pointer(&data[0]), int64(len(data)), 0, &r.face); code != 0 {
		r.close()
		return nil, fmt.Errorf("FreeType face failed (%d)", code)
	}
	if code := ftAPI.setSize(r.face, 0, int64(height)*64, 0, 0); code != 0 {
		r.close()
		return nil, fmt.Errorf("FreeType height failed (%d)", code)
	}
	raw := (*ftFaceRecord)(r.face)
	f := &hintedFace{r: r, glyphs: make(map[rune]ftGlyphMetrics), kernPairs: make(map[uint64]fixed.Int26_6)}
	// The reference ordinary TrueType path uses the old automatic hinter.
	// Its exceptional 'tricky' faces require their native interpreter instead.
	if ftAPI.format(r.face) == "TrueType" && raw.Flags&(1<<13) == 0 {
		f.flags = 1 << 5 // FT_LOAD_FORCE_AUTOHINT; keep the normal hint target.
	}
	if !blend {
		f.mode = 2 // FT_RENDER_MODE_MONO, not a threshold of the grayscale mask.
	}
	if raw.Units != 0 {
		scale := func(n int16) fixed.Int26_6 { return fixed.Int26_6(int64(n) * int64(height) * 64 / int64(raw.Units)) }
		f.metrics = font.Metrics{Height: scale(raw.Height), Ascent: scale(raw.Ascender), Descent: -scale(raw.Descender)}
	}
	f.cleanup = runtime.AddCleanup(f, (*ftResources).close, r)
	return f, nil
}

func (f *hintedFace) Close() error {
	f.cleanup.Stop()
	f.r.close()
	return nil
}
func (f *hintedFace) Metrics() font.Metrics { return f.metrics }

// Caller holds the face lock; the cache contains Go values only.
func (f *hintedFace) glyphMetrics(r rune) (ftGlyphMetrics, bool) {
	if f.r.face == nil {
		return ftGlyphMetrics{}, false
	}
	if m, found := f.glyphs[r]; found {
		return m, true
	}
	index := ftAPI.charIndex(f.r.face, uint64(r))
	if ftAPI.loadGlyph(f.r.face, index, f.flags) != 0 {
		return ftGlyphMetrics{}, false
	}
	slot := (*ftGlyphSlot)((*ftFaceRecord)(f.r.face).Glyph)
	m := slot.Metrics
	// SDL_ttf's bounds use floor(bearing), then ceil(width/height), even
	// where the rasterizer's bitmap origin would round differently.
	x, y := int(m[2]>>6), int(m[3]>>6)
	w, h := int((m[0]+63)>>6), int((m[1]+63)>>6)
	if w < 0 || h < 0 || w > 8192 || h > 8192 {
		return ftGlyphMetrics{}, false
	}
	result := ftGlyphMetrics{index, image.Rect(x, -y, x+w, h-y), fixed.I(int((m[4] + 63) >> 6))}
	f.glyphs[r] = result
	return result, true
}

func (f *hintedFace) GlyphBounds(r rune) (fixed.Rectangle26_6, fixed.Int26_6, bool) {
	defer runtime.KeepAlive(f)
	f.r.mu.Lock()
	defer f.r.mu.Unlock()
	m, ok := f.glyphMetrics(r)
	return fixed.R(m.bounds.Min.X, m.bounds.Min.Y, m.bounds.Max.X, m.bounds.Max.Y), m.advance, ok
}
func (f *hintedFace) GlyphAdvance(r rune) (fixed.Int26_6, bool) {
	_, advance, ok := f.GlyphBounds(r)
	return advance, ok
}
func (f *hintedFace) Kern(a, b rune) fixed.Int26_6 {
	defer runtime.KeepAlive(f)
	f.r.mu.Lock()
	defer f.r.mu.Unlock()
	key := uint64(uint32(a))<<32 | uint64(uint32(b))
	if k, found := f.kernPairs[key]; found {
		return k
	}
	var delta ftVector
	if f.r.face != nil {
		ftAPI.kern(f.r.face, ftAPI.charIndex(f.r.face, uint64(a)), ftAPI.charIndex(f.r.face, uint64(b)), 0, &delta)
	}
	k := fixed.I(int(delta.X >> 6))
	if len(f.kernPairs) < 512 {
		f.kernPairs[key] = k
	}
	return k
}

func (f *hintedFace) Glyph(dot fixed.Point26_6, r rune) (image.Rectangle, image.Image, image.Point, fixed.Int26_6, bool) {
	defer runtime.KeepAlive(f)
	f.r.mu.Lock()
	defer f.r.mu.Unlock()
	m, ok := f.glyphMetrics(r)
	if !ok || ftAPI.loadGlyph(f.r.face, m.index, f.flags) != 0 {
		return image.Rectangle{}, nil, image.Point{}, m.advance, false
	}
	slot := (*ftGlyphSlot)((*ftFaceRecord)(f.r.face).Glyph)
	if ftAPI.render(unsafe.Pointer(slot), f.mode) != 0 {
		return image.Rectangle{}, nil, image.Point{}, m.advance, false
	}
	b := slot.Bitmap
	if b.Rows > 8192 || b.Width > 8192 || b.Pitch == -1<<31 {
		return image.Rectangle{}, nil, image.Point{}, m.advance, false
	}
	mask := image.NewAlpha(image.Rect(0, 0, m.bounds.Dx(), m.bounds.Dy()))
	pitch := int(b.Pitch)
	base := b.Buffer
	if pitch < 0 {
		if b.Rows > 0 && base != nil {
			base = unsafe.Add(base, pitch*(int(b.Rows)-1))
		}
		pitch = -pitch
	}
	if b.Buffer != nil && pitch > 0 && pitch <= 32768 {
		pixels := unsafe.Slice((*byte)(base), pitch*int(b.Rows))
		// Modern monochrome rendering may trim blank border columns/rows.
		// Restore their position inside the metric cell using bitmap bearings;
		// treating a trimmed bitmap as cell-aligned shifts italic characters.
		offsetX, offsetY := int(slot.Left)-m.bounds.Min.X, -int(slot.Top)-m.bounds.Min.Y
		for y := 0; y < int(b.Rows); y++ {
			dy := y + offsetY
			if dy < 0 || dy >= mask.Rect.Dy() {
				continue
			}
			row := y
			if b.Pitch < 0 {
				row = int(b.Rows) - 1 - y
			}
			for x := 0; x < int(b.Width); x++ {
				dx := x + offsetX
				if dx < 0 || dx >= mask.Rect.Dx() {
					continue
				}
				var alpha uint8
				switch b.PixelMode {
				case 1: // FT_PIXEL_MODE_MONO
					alpha = ((pixels[row*pitch+x/8] >> uint(7-x%8)) & 1) * 255
				case 2: // FT_PIXEL_MODE_GRAY
					alpha = pixels[row*pitch+x]
					if b.Grays > 1 && b.Grays != 256 {
						alpha = uint8(min(255, uint32(alpha)*255/uint32(b.Grays-1)))
					}
				case 3: // FT_PIXEL_MODE_GRAY2
					alpha = ((pixels[row*pitch+x/4] >> uint(6-2*(x%4))) & 3) * 85
				case 4: // FT_PIXEL_MODE_GRAY4
					alpha = ((pixels[row*pitch+x/2] >> uint(4-4*(x%2))) & 15) * 17
				}
				mask.Pix[dy*mask.Stride+dx] = alpha
			}
		}
	}
	dr := m.bounds.Add(image.Pt(dot.X.Round(), dot.Y.Round()))
	return dr, mask, image.Point{}, m.advance, true
}
