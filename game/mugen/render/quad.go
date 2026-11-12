package render

import "github.com/hajimehoshi/ebiten/v2"

func quad(src *ebiten.Image, geometry ebiten.GeoM, tint ebiten.ColorScale) [4]ebiten.Vertex {
	bounds := src.Bounds()
	return quadDimensions(src, geometry, tint, float64(bounds.Dx()), float64(bounds.Dy()))
}

func paddedQuad(src *ebiten.Image, geometry ebiten.GeoM, tint ebiten.ColorScale) [4]ebiten.Vertex {
	// Preserve coverage of biased edge samples. Reads beyond the logical
	// sprite remain transparent through imageSrc0At's image-region checks.
	return quadRectangle(src, geometry, tint, -1, -1,
		float64(src.Bounds().Dx()+1), float64(src.Bounds().Dy()+1))
}

func quadDimensions(src *ebiten.Image, geometry ebiten.GeoM, tint ebiten.ColorScale, w, h float64) [4]ebiten.Vertex {
	return quadRectangle(src, geometry, tint, 0, 0, w, h)
}

func quadRectangle(src *ebiten.Image, geometry ebiten.GeoM, tint ebiten.ColorScale, x0, y0, x1, y1 float64) [4]ebiten.Vertex {
	bounds := src.Bounds()
	var vertices [4]ebiten.Vertex
	for i, point := range [4][2]float64{{x0, y0}, {x1, y0}, {x0, y1}, {x1, y1}} {
		x, y := geometry.Apply(point[0], point[1])
		// Keep interpolation fractions independent of the image's atlas offset.
		vertices[i] = ebiten.Vertex{DstX: float32(x), DstY: float32(y),
			SrcX: float32(point[0] + float64(bounds.Min.X)), SrcY: float32(point[1] + float64(bounds.Min.Y)),
			Custom0: float32(point[0]), Custom1: float32(point[1]),
			ColorR: tint.R(), ColorG: tint.G(), ColorB: tint.B(), ColorA: tint.A()}
	}
	return vertices
}
