package palette

import (
	"image"
	"image/color"
	"testing"
)

func TestNativeHitPaletteColorQuantization(t *testing.T) {
	source := color.NRGBA{247, 239, 231, 255}
	for _, test := range []struct {
		mul, add [3]int
		want     color.NRGBA
	}{
		{[3]int{128, 256, 64}, [3]int{}, color.NRGBA{124, 239, 58, 255}},
		{[3]int{256, 256, 256}, [3]int{20, -40, 5}, color.NRGBA{255, 199, 236, 255}},
		{[3]int{128, 128, 128}, [3]int{20, -40, 5}, color.NRGBA{134, 100, 118, 255}},
		{[3]int{-256, 256, 256}, [3]int{-300, 0, 0}, color.NRGBA{53, 239, 231, 255}},
	} {
		actual := (HitEffect{Mul: test.mul, Add: test.add}).Color(source)
		if actual != test.want {
			t.Fatal(test, actual)
		}
	}
}

func TestHitPaletteLifetimeAndClone(t *testing.T) {
	s := New(nil)
	s.SetHitEffect(-1, [3]int{}, [3]int{})
	revision := s.Revision
	for _, duration := range []int{0, -2} {
		s.SetHitEffect(duration, [3]int{256, 256, 256}, [3]int{})
		if s.Revision != revision || s.hitFX.Time != -1 {
			t.Fatal("non-effect contact replaced the existing effect")
		}
	}
	clone := s.Clone()
	s.SetHitEffect(1, [3]int{256, 256, 256}, [3]int{})
	s.AdvanceHitEffect()
	if s.hitFX.Time != 0 || s.Revision != revision+2 || clone.hitFX.Time != -1 {
		t.Fatal("expiry/cache revision/independent clone", s.hitFX, s.Revision, clone.hitFX)
	}
}

func TestHitPalettePreservesIndexedPixelsAndTransparency(t *testing.T) {
	source := image.NewPaletted(image.Rect(0, 0, 2, 1), color.Palette{color.NRGBA{}, color.NRGBA{247, 239, 231, 255}})
	source.SetColorIndex(1, 0, 1)
	fx := HitEffect{Mul: [3]int{128, 128, 128}, Add: [3]int{20, -40, 5}}
	actual := fx.Image(source)
	indexed, ok := actual.(*image.Paletted)
	if !ok || indexed.ColorIndexAt(1, 0) != 1 || indexed.At(1, 0) != (color.NRGBA{134, 100, 118, 255}) {
		t.Fatal("indexed sampling or palette color changed", actual)
	}
	_, _, _, alpha := indexed.At(0, 0).RGBA()
	if alpha != 0 || source.At(1, 0) != (color.NRGBA{247, 239, 231, 255}) {
		t.Fatal("transparent key or immutable source changed")
	}
}
