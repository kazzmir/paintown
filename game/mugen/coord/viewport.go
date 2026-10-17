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

// Fit preserves aspect and centers one surface inside another. The world and
// HUD use independent transforms, matching the native stage margins.
func Fit(source, target Space) (scale, x, y float64, err error) {
	if source.Width <= 0 || source.Height <= 0 || target.Width <= 0 || target.Height <= 0 {
		return 0, 0, 0, fmt.Errorf("invalid fitted surface dimensions")
	}
	scale = math.Min(float64(target.Width)/float64(source.Width), float64(target.Height)/float64(source.Height))
	return scale, (float64(target.Width) - float64(source.Width)*scale) / 2, (float64(target.Height) - float64(source.Height)*scale) / 2, nil
}
