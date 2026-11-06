package render

import (
	"math"
	"strings"

	"github.com/hajimehoshi/ebiten/v2"
)

// Perspective rotates a sprite about its axis and projects into the game
// surface. The ordinary final viewport fit still runs after this projection.
type Perspective struct {
	X, Y, CenterX, CenterY, Distance float64
	Angle, XAngle, YAngle            float64
	Post                             ebiten.GeoM // Common-surface projection applied after perspective division.
}

func (p Perspective) matrix(local ebiten.GeoM) [9]float64 {
	radians := math.Pi / 180
	cx, sx := math.Cos(p.XAngle*radians), math.Sin(p.XAngle*radians)
	cy, sy := math.Cos(p.YAngle*radians), math.Sin(p.YAngle*radians)
	cz, sz := math.Cos(p.Angle*radians), math.Sin(p.Angle*radians)
	project := func(u, v float64) [3]float64 {
		x, y := local.Apply(u, v)
		x, y = x*cz+y*sz, -x*sz+y*cz
		x, z := x*cy, x*sy
		y, z = y*cx+z*sx, -y*sx+z*cx
		q := p.Distance + z
		u, v = p.Distance*(p.X+x)+p.CenterX*z, p.Distance*(p.Y+y)+p.CenterY*z
		return [3]float64{p.Post.Element(0, 0)*u + p.Post.Element(0, 1)*v + p.Post.Element(0, 2)*q, p.Post.Element(1, 0)*u + p.Post.Element(1, 1)*v + p.Post.Element(1, 2)*q, q}
	}
	a, b, c := project(0, 0), project(1, 0), project(0, 1)
	return [9]float64{b[0] - a[0], c[0] - a[0], a[0], b[1] - a[1], c[1] - a[1], a[1], b[2] - a[2], c[2] - a[2], a[2]}
}

func inverse(m [9]float64) ([9]float64, bool) {
	i := [9]float64{m[4]*m[8] - m[5]*m[7], m[2]*m[7] - m[1]*m[8], m[1]*m[5] - m[2]*m[4], m[5]*m[6] - m[3]*m[8], m[0]*m[8] - m[2]*m[6], m[2]*m[3] - m[0]*m[5], m[3]*m[7] - m[4]*m[6], m[1]*m[6] - m[0]*m[7], m[0]*m[4] - m[1]*m[3]}
	d := m[0]*i[0] + m[1]*i[3] + m[2]*i[6]
	if math.Abs(d) < 1e-9 || math.IsNaN(d) || math.IsInf(d, 0) {
		return i, false
	}
	for n := range i {
		i[n] /= d
	}
	return i, true
}

func projectShader(source string) string {
	source = strings.Replace(source, "package main", `package main
var SourceX vec3
var SourceY vec3
var Divisor vec3
func mapSource(dst vec2) vec2 {
	q := vec3(dst, 1)
	return vec2(dot(SourceX, q), dot(SourceY, q)) / dot(Divisor, q)
}`, 1)
	return strings.ReplaceAll(source, "local.xy", "mapSource(dst.xy - imageDstOrigin())")
}

func projectedDraw(dst, src *ebiten.Image, options *ebiten.DrawImageOptions, p Perspective, shader **ebiten.Shader, source string, indexed bool, style DrawStyle) error {
	m := p.matrix(options.GeoM)
	inv, ok := inverse(m)
	if !ok {
		return nil
	}
	if *shader == nil {
		compiled, err := ebiten.NewShader([]byte(withPaletteShader(projectShader(source))))
		if err != nil {
			return err
		}
		*shader = compiled
	}
	vertices := quad(src, options.GeoM, options.ColorScale)
	x0, y0, x1, y1 := 0.0, 0.0, float64(src.Bounds().Dx()), float64(src.Bounds().Dy())
	if indexed {
		x0, y0, x1, y1 = -1, -1, x1+1, y1+1
	}
	for n, uv := range [4][2]float64{{x0, y0}, {x1, y0}, {x0, y1}, {x1, y1}} {
		x, y := uv[0], uv[1]
		q := m[6]*x + m[7]*y + m[8]
		if q <= 0 {
			return nil
		}
		vertices[n].DstX, vertices[n].DstY = float32((m[0]*x+m[1]*y+m[2])/q), float32((m[3]*x+m[4]*y+m[5])/q)
	}
	uniforms := map[string]any{"SourceX": []float32{float32(inv[0]), float32(inv[1]), float32(inv[2])}, "SourceY": []float32{float32(inv[3]), float32(inv[4]), float32(inv[5])}, "Divisor": []float32{float32(inv[6]), float32(inv[7]), float32(inv[8])}}
	for key, value := range style.uniforms() {
		uniforms[key] = value
	}
	if indexed {
		reverse := sourceReversal(options.GeoM)
		if style.SourceReversal != nil {
			reverse = *style.SourceReversal
		}
		uniforms["Reverse"] = reverse
	}
	dst.DrawTrianglesShader(vertices[:], []uint16{0, 1, 2, 1, 3, 2}, *shader, &ebiten.DrawTrianglesShaderOptions{Images: [4]*ebiten.Image{src}, Blend: options.Blend, CompositeMode: options.CompositeMode, Uniforms: uniforms})
	return nil
}

func (r *Nearest) DrawProjected(dst, src *ebiten.Image, options *ebiten.DrawImageOptions, p Perspective, destination ...float32) error {
	return projectedDraw(dst, src, options, p, &r.projected, nearestShader, true, DrawStyle{DestinationFactor: alphaDestination(destination)})
}

func (r *Linear) DrawProjected(dst, src *ebiten.Image, options *ebiten.DrawImageOptions, p Perspective, destination ...float32) error {
	return projectedDraw(dst, src, options, p, &r.projected, linearShader, false, DrawStyle{DestinationFactor: alphaDestination(destination)})
}

func (r *Nearest) DrawStyledProjected(dst, src *ebiten.Image, options *ebiten.DrawImageOptions, p Perspective, style DrawStyle) error {
	return projectedDraw(dst, src, options, p, &r.projected, nearestShader, true, style)
}
func (r *Linear) DrawStyledProjected(dst, src *ebiten.Image, options *ebiten.DrawImageOptions, p Perspective, style DrawStyle) error {
	return projectedDraw(dst, src, options, p, &r.projected, linearShader, false, style)
}
