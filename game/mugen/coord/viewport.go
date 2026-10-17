package coord

import (
	"fmt"
	"math"
)

// Viewport is the world context shared by match entities. All values are in
// common game units. Zoom is retained for the camera work; stock scenes use 1.
type Viewport struct {
	Space                     Space
	CameraX, CameraY, GroundY float64
	Zoom                      float64
	// CropY is the stage origin removed by StageFit=0, in common game units.
	// It contributes to CameraPos but is a uniform crop, independent of BG delta.
	CropY float64
}

// GameSpaceForStage matches the beta's measured stage-aspect match resolution,
// including both declared stage versions and a differing configured height.
func GameSpaceForStage(width int, stage Space) (Space, error) {
	if width <= 0 || stage.Width <= 0 || stage.Height <= 0 {
		return Space{}, fmt.Errorf("invalid stage/game dimensions")
	}
	height := int64(width) * int64(stage.Height) / int64(stage.Width)
	if height <= 0 || height > math.MaxInt32 {
		return Space{}, fmt.Errorf("stage-aspect game height outside positive int32 range")
	}
	return Space{Width: width, Height: int(height)}, nil
}

// Fit preserves aspect and centers one surface inside another.
func Fit(source, target Space) (scale, x, y float64, err error) {
	if source.Width <= 0 || source.Height <= 0 || target.Width <= 0 || target.Height <= 0 {
		return 0, 0, 0, fmt.Errorf("invalid fitted surface dimensions")
	}
	scale = math.Min(float64(target.Width)/float64(source.Width), float64(target.Height)/float64(source.Height))
	return scale, (float64(target.Width) - float64(source.Width)*scale) / 2, (float64(target.Height) - float64(source.Height)*scale) / 2, nil
}

// StageViewport selects the native script-visible dimensions. StageFit=1 uses
// stage aspect; StageFit=0 uses configured game aspect and divides a shorter
// viewport's crop according to the stage's cuthigh/cutlow guidelines.
func StageViewport(configured, stage Space, fit bool, cutHigh, cutLow float64) (*Viewport, error) {
	space, err := GameSpaceForStage(configured.Width, stage)
	if err != nil || configured.Height <= 0 {
		return nil, fmt.Errorf("invalid stage viewport dimensions")
	}
	v := &Viewport{Space: space, Zoom: 1}
	if fit {
		return v, nil
	}
	v.Space = configured
	scale := float64(configured.Width) / float64(stage.Width)
	removed := float64(stage.Height) - float64(configured.Height)/scale
	if removed > 0 {
		cut := removed / 2
		if total := cutHigh + cutLow; total > 0 {
			if removed <= total {
				cut = removed * cutHigh / total
			} else {
				cut = removed - cutLow
			}
		}
		// The beta truncates the cut in stage units before common conversion.
		v.CropY = math.Trunc(cut) * scale
	}
	return v, nil
}

type Transform struct{ ScaleX, ScaleY, X, Y float64 }

func (t Transform) Point(x, y float64) (float64, float64) {
	return t.X + x*t.ScaleX, t.Y + y*t.ScaleY
}

// Project maps the script-visible game viewport into a physical window.
func Project(source, target Space, keepAspect bool) (Transform, error) {
	scale, x, y, err := Fit(source, target)
	if err != nil {
		return Transform{}, err
	}
	if keepAspect {
		return Transform{scale, scale, x, y}, nil
	}
	return Transform{ScaleX: float64(target.Width) / float64(source.Width), ScaleY: float64(target.Height) / float64(source.Height)}, nil
}

// HUDProject uses the game viewport's transform and top origin. A fight HUD
// uses motif-width units, rather than an independent motif-aspect fit.
func HUDProject(game, motif, target Space, keepAspect bool) (Transform, error) {
	t, err := Project(game, target, keepAspect)
	if err != nil {
		return Transform{}, err
	}
	scale, err := motif.ScaleTo(game)
	if err != nil {
		return Transform{}, err
	}
	t.ScaleX, t.ScaleY = t.ScaleX*scale, t.ScaleY*scale
	return t, nil
}
