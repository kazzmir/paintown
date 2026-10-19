// Package coord describes MUGEN local and game coordinate spaces.
package coord

import (
	"fmt"
	"math"
	"strconv"
	"strings"
)

type Space struct {
	Width, Height int
}

func Default() Space { return Space{Width: 320, Height: 240} }

func Parse(raw string) (Space, error) {
	if strings.TrimSpace(raw) == "" {
		return Default(), nil
	}
	parts := strings.Split(raw, ",")
	if len(parts) != 2 {
		return Space{}, fmt.Errorf("coordinate space requires width,height: %q", raw)
	}
	values := [2]int{}
	for i, part := range parts {
		value, err := strconv.ParseFloat(strings.TrimSpace(part), 64)
		value = math.Trunc(value)
		if err != nil || math.IsNaN(value) || math.IsInf(value, 0) || value < 1 || value > math.MaxInt32 {
			return Space{}, fmt.Errorf("coordinate dimensions must truncate to positive int32 values: %q", raw)
		}
		values[i] = int(value)
	}
	return Space{Width: values[0], Height: values[1]}, nil
}

// ScaleTo follows the supplied coordinate-space specification: vector scaling
// uses the width ratio for both axes, even when the aspect ratios differ.
func (s Space) ScaleTo(target Space) (float64, error) {
	if s.Width <= 0 || s.Height <= 0 || target.Width <= 0 || target.Height <= 0 {
		return 0, fmt.Errorf("invalid coordinate-space conversion: %v to %v", s, target)
	}
	return float64(target.Width) / float64(s.Width), nil
}

func (s Space) Translate(x, y float64, target Space) (float64, float64, error) {
	scale, err := s.ScaleTo(target)
	if err != nil {
		return 0, 0, err
	}
	return x * scale, y * scale, nil
}
