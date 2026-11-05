package character

import (
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/engine"
	"math"
	"testing"
)

func TestAngleRegisterAndFrameDrawingAreIndependent(t *testing.T) {
	c := NewCharacter(&cns.CNS{}, nil, nil)
	c.AngleSet(90)
	c.AngleAdd(0.000001)
	if c.DrawAngle != 90 || c.AngleDrawingEnabled {
		t.Fatal(c.DrawAngle, c.AngleDrawingEnabled)
	}
	c.SetAngleDrawing(engine.AngleDrawing{Scale: [2]float32{2, 0.5}})
	c.SetAngleDrawing(engine.AngleDrawing{Scale: [2]float32{3, 2}})
	if c.DrawAngle != 90 || c.DrawScale != [2]float32{6, 1} {
		t.Fatal(c.DrawAngle, c.DrawScale)
	}
	c.AngleSet(180)
	if c.DrawAngle != 180 || !c.AngleDrawingEnabled {
		t.Fatal("later angle update did not reach enabled frame")
	}
	c.SetAngleDrawing(engine.AngleDrawing{HasValue: true, Value: -math.MaxFloat32, Scale: [2]float32{1, 1}})
	if c.DrawAngle != 180 {
		t.Fatal("omitted-angle sentinel overwrote register", c.DrawAngle)
	}
}
