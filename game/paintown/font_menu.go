package paintown

import (
	"bytes"
	"fmt"
	"image"
	"io/fs"
	"os"
	"path/filepath"
	"slices"
	"sort"
	"strings"
	"sync"

	"github.com/hajimehoshi/ebiten/v2/text/v2"
	"github.com/kazzmir/paintown/game/data"
	"golang.org/x/image/font/sfnt"
	"golang.org/x/image/math/fixed"
)

const minMenuFontSize, maxMenuFontSize = 8, 64

type menuFontFile struct{ Name, Path string }
type menuFontConfig struct {
	path          string
	width, height float64
	source        *text.GoTextFaceSource
	revision      uint64
}

var menuFontPreferences struct {
	sync.RWMutex
	config menuFontConfig
}
var menuFontCatalog struct {
	sync.Once
	fonts []menuFontFile
}
var menuFontSources = struct {
	sync.Mutex
	sources map[string]*text.GoTextFaceSource
	order   []string
}{sources: make(map[string]*text.GoTextFaceSource)}

func menuFontRoots() []string {
	roots := []string{data.DataPath("fonts"), "/usr/share/fonts", "/usr/local/share/fonts", "/Library/Fonts", "/System/Library/Fonts", "/Network/Library/Fonts"}
	if home, err := os.UserHomeDir(); err == nil {
		roots = append(roots, filepath.Join(home, ".fonts"), filepath.Join(home, ".local", "share", "fonts"), filepath.Join(home, "Library", "Fonts"))
	}
	if dir := os.Getenv("XDG_DATA_HOME"); dir != "" {
		roots = append(roots, filepath.Join(dir, "fonts"))
	}
	for _, dir := range filepath.SplitList(os.Getenv("XDG_DATA_DIRS")) {
		if dir != "" {
			roots = append(roots, filepath.Join(dir, "fonts"))
		}
	}
	windows := os.Getenv("WINDIR")
	if windows == "" {
		windows = "C:/Windows"
	}
	roots = append(roots, filepath.Join(windows, "Fonts"))
	if local := os.Getenv("LOCALAPPDATA"); local != "" {
		roots = append(roots, filepath.Join(local, "Microsoft", "Windows", "Fonts"))
	}
	return roots
}

func canonicalFontPath(path string) string {
	absolute, err := filepath.Abs(path)
	if err == nil {
		path = absolute
	}
	if resolved, err := filepath.EvalSymlinks(path); err == nil {
		path = resolved
	}
	return filepath.Clean(path)
}

func discoverMenuFonts(roots []string) []menuFontFile {
	seenRoots, seenFiles := make(map[string]bool), make(map[string]bool)
	var fonts []menuFontFile
	for _, root := range roots {
		root = canonicalFontPath(root)
		if seenRoots[root] {
			continue
		}
		seenRoots[root] = true
		// WalkDir deliberately does not follow directory symlinks, avoiding
		// cycles. File symlinks are resolved and deduplicated below.
		filepath.WalkDir(root, func(path string, entry fs.DirEntry, err error) error {
			if err != nil || entry.IsDir() || !strings.EqualFold(filepath.Ext(path), ".ttf") {
				return nil
			}
			info, err := os.Stat(path)
			if err != nil || !info.Mode().IsRegular() {
				return nil
			}
			canonical := canonicalFontPath(path)
			if seenFiles[canonical] {
				return nil
			}
			seenFiles[canonical] = true
			if !usableMenuFontFile(canonical) {
				return nil
			}
			fonts = append(fonts, menuFontFile{Name: fontFileName(path), Path: canonical})
			return nil
		})
	}
	sort.Slice(fonts, func(i, j int) bool {
		a, b := strings.ToLower(fonts[i].Name), strings.ToLower(fonts[j].Name)
		if a == b {
			return fonts[i].Path < fonts[j].Path
		}
		return a < b
	})
	return fonts
}

// Validate CPU outlines rather than GPU pixels, so discovery can run before
// the game starts without readbacks or rasterizing every installed font.
func usableMenuFontFile(path string) bool {
	file, err := os.Open(path)
	if err != nil {
		return false
	}
	defer file.Close()
	font, err := sfnt.ParseReaderAt(file)
	if err != nil || validateMenuFont(font) != nil {
		return false
	}
	if _, err := file.Seek(0, 0); err != nil {
		return false
	}
	source, err := text.NewGoTextFaceSource(file)
	return err == nil && validateMenuFontRenderer(source, font) == nil
}

func normalizedMenuGlyph(font *sfnt.Font, buffer *sfnt.Buffer, glyph sfnt.GlyphIndex) (sfnt.Segments, error) {
	segments, err := font.LoadGlyph(buffer, glyph, fixed.I(24), nil)
	if err != nil {
		return nil, err
	}
	bounds := segments.Bounds()
	if len(segments) == 0 || bounds.Max.X <= bounds.Min.X || bounds.Max.Y <= bounds.Min.Y {
		return nil, fmt.Errorf("blank glyph")
	}
	out := make(sfnt.Segments, len(segments))
	for index, segment := range segments {
		out[index].Op = segment.Op
		count := 1
		if segment.Op == sfnt.SegmentOpQuadTo {
			count = 2
		}
		if segment.Op == sfnt.SegmentOpCubeTo {
			count = 3
		}
		for point := range count {
			out[index].Args[point] = fixed.Point26_6{X: segment.Args[point].X - bounds.Min.X, Y: segment.Args[point].Y - bounds.Min.Y}
		}
	}
	return out, nil
}

func hollowMenuGlyphBox(outline sfnt.Segments) bool {
	var rectangles []fixed.Rectangle26_6
	var points []fixed.Point26_6
	finish := func() bool {
		if len(points) > 1 && points[0] == points[len(points)-1] {
			points = points[:len(points)-1]
		}
		if len(points) != 4 {
			return false
		}
		bounds := fixed.Rectangle26_6{Min: points[0], Max: points[0]}
		for _, point := range points {
			bounds.Min.X, bounds.Min.Y = min(bounds.Min.X, point.X), min(bounds.Min.Y, point.Y)
			bounds.Max.X, bounds.Max.Y = max(bounds.Max.X, point.X), max(bounds.Max.Y, point.Y)
		}
		if bounds.Min.X == bounds.Max.X || bounds.Min.Y == bounds.Max.Y {
			return false
		}
		seen := make(map[fixed.Point26_6]bool)
		for index, point := range points {
			next := points[(index+1)%4]
			if seen[point] || (point.X != bounds.Min.X && point.X != bounds.Max.X) ||
				(point.Y != bounds.Min.Y && point.Y != bounds.Max.Y) || (point.X != next.X && point.Y != next.Y) {
				return false
			}
			seen[point] = true
		}
		rectangles = append(rectangles, bounds)
		points = nil
		return true
	}
	for _, segment := range outline {
		switch segment.Op {
		case sfnt.SegmentOpMoveTo:
			if len(points) > 0 && !finish() {
				return false
			}
		case sfnt.SegmentOpLineTo:
		default:
			return false
		}
		points = append(points, segment.Args[0])
	}
	if !finish() || len(rectangles) != 2 {
		return false
	}
	inside := func(a, b fixed.Rectangle26_6) bool {
		return a.Min.X > b.Min.X && a.Min.Y > b.Min.Y && a.Max.X < b.Max.X && a.Max.Y < b.Max.Y
	}
	return inside(rectangles[0], rectangles[1]) || inside(rectangles[1], rectangles[0])
}

func validateMenuFont(font *sfnt.Font) error {
	var buffer sfnt.Buffer
	var replacements []sfnt.Segments
	if outline, err := normalizedMenuGlyph(font, &buffer, 0); err == nil {
		replacements = append(replacements, outline)
	}
	for _, character := range []rune{'\ufffd', '\u25a1'} {
		glyph, err := font.GlyphIndex(&buffer, character)
		if err != nil || glyph == 0 {
			continue
		}
		if outline, err := normalizedMenuGlyph(font, &buffer, glyph); err == nil {
			replacements = append(replacements, outline)
		}
	}
	var a sfnt.Segments
	for _, character := range "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" {
		glyph, err := font.GlyphIndex(&buffer, character)
		if err != nil || glyph == 0 {
			return fmt.Errorf("missing basic glyph %q", character)
		}
		outline, err := normalizedMenuGlyph(font, &buffer, glyph)
		if err != nil {
			return fmt.Errorf("unrenderable basic glyph %q: %w", character, err)
		}
		for _, replacement := range replacements {
			if slices.Equal(outline, replacement) {
				return fmt.Errorf("replacement box for basic glyph %q", character)
			}
		}
		if (character == 'a' || character == 'b') && hollowMenuGlyphBox(outline) {
			return fmt.Errorf("empty square for basic glyph %q", character)
		}
		if character == 'a' {
			a = outline
		}
		if character == 'b' && slices.Equal(a, outline) {
			return fmt.Errorf("identical fallback glyphs for a and b")
		}
	}
	return nil
}

// Check the actual renderer too: legacy fonts can have conflicting cmap
// tables, and GoText may select a different mapping than the outline parser.
func validateMenuFontRenderer(source *text.GoTextFaceSource, font *sfnt.Font) error {
	face := &text.GoTextFace{Source: source, Size: 24}
	const basic = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
	glyphs := text.AppendLazyGlyphs(nil, basic, face, nil)
	if len(glyphs) == 0 {
		return fmt.Errorf("no basic glyphs rendered")
	}
	var buffer sfnt.Buffer
	missing, _ := normalizedMenuGlyph(font, &buffer, 0)
	var a sfnt.Segments
	for _, glyph := range glyphs {
		if glyph.GID == 0 {
			return fmt.Errorf("renderer selected a missing glyph")
		}
		outline, err := normalizedMenuGlyph(font, &buffer, sfnt.GlyphIndex(glyph.GID))
		if err != nil || slices.Equal(outline, missing) {
			return fmt.Errorf("renderer selected a blank or replacement glyph")
		}
		if glyph.StartIndexInBytes < 2 && hollowMenuGlyphBox(outline) {
			return fmt.Errorf("renderer selected an empty square for a basic letter")
		}
		if glyph.StartIndexInBytes == 0 {
			a = outline
		}
		if glyph.StartIndexInBytes == 1 && slices.Equal(a, outline) {
			return fmt.Errorf("renderer selected identical fallback glyphs for a and b")
		}
	}
	return nil
}

func fontFileName(path string) string {
	return strings.TrimSuffix(filepath.Base(path), filepath.Ext(path))
}

func resolvedMenuFontPath(path string) string {
	if !filepath.IsAbs(path) {
		path = data.DataPath(path)
	}
	return canonicalFontPath(path)
}

func loadMenuFontSource(path string) (*text.GoTextFaceSource, error) {
	path = resolvedMenuFontPath(path)
	menuFontSources.Lock()
	defer menuFontSources.Unlock()
	if source := menuFontSources.sources[path]; source != nil {
		return source, nil
	}
	raw, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}
	parsed, err := sfnt.Parse(raw)
	if err != nil {
		return nil, err
	}
	if err := validateMenuFont(parsed); err != nil {
		return nil, err
	}
	// Keep owned bytes: GoTextFaceSource lazily loads glyphs from its reader.
	source, err := text.NewGoTextFaceSource(bytes.NewReader(raw))
	if err != nil {
		return nil, err
	}
	if err := validateMenuFontRenderer(source, parsed); err != nil {
		return nil, err
	}
	// Browsing many fonts should not retain every font's glyph cache forever.
	if len(menuFontSources.order) >= 16 {
		delete(menuFontSources.sources, menuFontSources.order[0])
		menuFontSources.order = menuFontSources.order[1:]
	}
	menuFontSources.sources[path] = source
	menuFontSources.order = append(menuFontSources.order, path)
	return source, nil
}

func currentMenuFontConfig() menuFontConfig {
	menuFontPreferences.RLock()
	defer menuFontPreferences.RUnlock()
	return menuFontPreferences.config
}

func (menu *Menu) syncMenuFont() {
	config := currentMenuFontConfig()
	if menu.fontRevision == config.revision {
		return
	}
	menu.fontRevision = config.revision
	if config.source == nil {
		return
	}
	scale := menu.fontScale
	if scale <= 0 {
		scale = 1
	}
	menu.FontPath, menu.FontWidth, menu.FontSize = config.path, config.width*scale, config.height*scale
	menu.font = &text.GoTextFace{Source: config.source, Size: menu.FontSize}
	menu.refreshFontLabels()
}

// Theme fonts can have very different ascenders/descenders. Use actual glyph
// bounds so switching from Neuropol to system fonts does not overlap rows.
func (menu *Menu) listSpacing() float64 {
	if menu.font == nil {
		return menu.FontSize / 1.3
	}
	if menu.spacingFace != menu.font {
		menu.spacingFace = menu.font
		menu.lineHeight = menu.FontSize / 1.3
		first := true
		top, bottom := 0.0, 0.0
		for _, glyph := range text.AppendGlyphs(nil, "AgjpQ", menu.font, nil) {
			if glyph.Image == nil {
				continue
			}
			if first {
				top, bottom, first = glyph.Y, glyph.Y, false
			}
			top = min(top, glyph.Y)
			bottom = max(bottom, glyph.Y+float64(glyph.Image.Bounds().Dy()))
		}
		if !first {
			menu.lineHeight = max(menu.lineHeight, bottom-top+2)
		}
	}
	return menu.lineHeight
}

func (menu *Menu) fontFiles() []menuFontFile {
	menuFontCatalog.Do(func() { menuFontCatalog.fonts = discoverMenuFonts(menuFontRoots()) })
	fonts := append([]menuFontFile(nil), menuFontCatalog.fonts...)
	// Include the theme's current font even if it is outside standard folders.
	current := resolvedMenuFontPath(menu.FontPath)
	for _, font := range fonts {
		if font.Path == current {
			return fonts
		}
	}
	if info, err := os.Stat(current); err == nil && info.Mode().IsRegular() && usableMenuFontFile(current) {
		fonts = append(fonts, menuFontFile{Name: fontFileName(current), Path: current})
	}
	return fonts
}

func makeFontSelectionMenu(parent *Menu, adjust string) *Menu {
	menu := *parent
	menu.Options = nil
	menu.Selected, menu.ticks = 0, 0
	menu.opacity, menu.scrollOffset = 0, 0
	menu.Notice = ""
	menu.Bounds = image.Rect(50, 155, 590, 420)
	menu.InfoX, menu.InfoY, menu.MenuInfoX, menu.MenuInfoY = 320, 85, 320, 450
	menu.Scroll, menu.Wrap, menu.DistanceFade = true, true, true
	if adjust == "width" || adjust == "height" {
		title, value := "Font Width", int(menu.FontWidth)
		if adjust == "height" {
			title, value = "Font Height", int(menu.FontSize)
		}
		menu.Name, menu.MenuInfo = title, title
		for size := minMenuFontSize; size <= maxMenuFontSize; size++ {
			menu.Options = append(menu.Options, MenuOption{Kind: "set-font-" + adjust, Value: size, Name: fmt.Sprintf("%d px", size), Info: "Aa Bb Cc 0123456789\nEnter / Cross: apply"})
			if size == value {
				menu.Selected = len(menu.Options) - 1
			}
		}
	} else {
		menu.Name, menu.MenuInfo = "Choose Font", "Choose Font"
		fonts := menu.fontFiles()
		names := make(map[string]int)
		for _, font := range fonts {
			names[font.Name]++
		}
		current := resolvedMenuFontPath(menu.FontPath)
		for _, font := range fonts {
			name := font.Name
			if names[name] > 1 {
				name += " (" + filepath.Base(filepath.Dir(font.Path)) + ")"
			}
			menu.Options = append(menu.Options, MenuOption{Kind: "set-font", Path: font.Path, Name: name, Info: font.Name + "\nAa Bb Cc 0123456789"})
			if font.Path == current {
				menu.Selected = len(menu.Options) - 1
			}
		}
		if len(menu.Options) == 0 {
			menu.Options = []MenuOption{{Kind: "notice", Name: "No TTF fonts found", Info: "Install fonts in a system or user font folder."}}
		}
	}
	menu.refreshFontLabels()
	return &menu
}

func (menu *Menu) refreshFontLabels() {
	currentPath := ""
	for index := range menu.Options {
		option := &menu.Options[index]
		switch option.Kind {
		case "font-select":
			switch option.Adjust {
			case "width":
				option.Name = fmt.Sprintf("Font Width: %.0f", menu.FontWidth)
			case "height":
				option.Name = fmt.Sprintf("Font Height: %.0f", menu.FontSize)
			default:
				option.Name = "Font: " + fontFileName(menu.FontPath)
			}
			option.Info = "Left / Right: adjust; Enter / Cross: browse"
		case "set-font":
			option.Name = strings.TrimPrefix(option.Name, "* ")
			if currentPath == "" {
				currentPath = resolvedMenuFontPath(menu.FontPath)
			}
			if option.Path == currentPath {
				option.Name = "* " + option.Name
			}
		case "set-font-width", "set-font-height":
			option.Name = fmt.Sprintf("%d px", option.Value)
			value := menu.FontWidth
			if option.Kind == "set-font-height" {
				value = menu.FontSize
			}
			if float64(option.Value) == value {
				option.Name = "* " + option.Name
			}
		}
	}
}

func (menu *Menu) applyFontSetting(option MenuOption) bool {
	menu.syncMenuFont()
	path, width, height := menu.FontPath, menu.FontWidth, menu.FontSize
	var source *text.GoTextFaceSource
	if menu.font != nil {
		source = menu.font.Source
	}
	switch option.Kind {
	case "set-font":
		path = option.Path
		source = nil
	case "set-font-width":
		width = float64(max(minMenuFontSize, min(maxMenuFontSize, option.Value)))
	case "set-font-height":
		height = float64(max(minMenuFontSize, min(maxMenuFontSize, option.Value)))
	default:
		return false
	}
	if source == nil {
		var err error
		source, err = loadMenuFontSource(path)
		if err != nil {
			menu.Notice = "Could not load font: " + filepath.Base(path)
			return true
		}
	}
	menuFontPreferences.Lock()
	menuFontPreferences.config = menuFontConfig{path: path, width: width, height: height, source: source, revision: menuFontPreferences.config.revision + 1}
	menuFontPreferences.Unlock()
	menu.syncMenuFont()
	menu.Notice = "Aa Bb Cc 0123456789"
	return true
}

// Adjust handles sliders and the original menu's Left/Right font controls.
// Font and size pickers can also be opened with Enter or Cross.
func (menu *Menu) Adjust(direction int) bool {
	if len(menu.Options) == 0 || direction == 0 {
		return false
	}
	menu.syncMenuFont()
	option := menu.Options[menu.Selected]
	if option.Kind == "invincible" { return menu.ApplySetting(option) }
	if option.Slider != nil { return option.Slider.Adjust(direction) }
	if option.Kind != "font-select" {
		return false
	}
	switch option.Adjust {
	case "width":
		return menu.applyFontSetting(MenuOption{Kind: "set-font-width", Value: int(menu.FontWidth) + direction})
	case "height":
		return menu.applyFontSetting(MenuOption{Kind: "set-font-height", Value: int(menu.FontSize) + direction})
	default:
		fonts := menu.fontFiles()
		if len(fonts) == 0 {
			menu.Notice = "No TTF fonts found"
			return true
		}
		current := -1
		for index, font := range fonts {
			if font.Path == resolvedMenuFontPath(menu.FontPath) {
				current = index
				break
			}
		}
		for offset := 1; offset <= len(fonts); offset++ {
			index := ((current+direction*offset)%len(fonts) + len(fonts)) % len(fonts)
			// Skip broken font files when cycling, but keep them selectable in
			// the picker so a failed explicit selection can report an error.
			if _, err := loadMenuFontSource(fonts[index].Path); err != nil {
				continue
			}
			return menu.applyFontSetting(MenuOption{Kind: "set-font", Path: fonts[index].Path})
		}
		menu.Notice = "No readable TTF fonts found"
		return true
	}
}
