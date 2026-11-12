package coord

import "testing"

func TestWidthBasedCoordinateTranslation(t *testing.T) {
	base, err := Parse("")
	if err != nil || base != (Space{320, 240}) {
		t.Fatal(base, err)
	}
	high, err := Parse("1280, 720")
	if err != nil {
		t.Fatal(err)
	}
	x, y, err := base.Translate(2, -3, high)
	if err != nil || x != 8 || y != -12 {
		t.Fatal("both axes must use width ratio despite different aspect", x, y, err)
	}
	x, y, err = high.Translate(x, y, base)
	if err != nil || x != 2 || y != -3 {
		t.Fatal("inverse conversion", x, y, err)
	}
	if got, err := Parse("640.5,480.5"); err != nil || got != (Space{640, 480}) {
		t.Fatal("native fractional dimensions must truncate", got, err)
	}
}

func TestInvalidCoordinateDimensions(t *testing.T) {
	for _, raw := range []string{"320", "0,240", "320,-1", "0.5,2", "x,2", "320,240,1", "2147483648,240", "NaN,2", "Inf,2"} {
		if _, err := Parse(raw); err == nil {
			t.Errorf("accepted %q", raw)
		}
	}
	if _, err := (Space{}).ScaleTo(Default()); err == nil {
		t.Fatal("invalid source produced a scale")
	}
}
