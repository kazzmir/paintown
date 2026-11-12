package character

import (
	"testing"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/engine"
)

func TestFallEnvShakeConsumesPositiveDurationAndKeepsMetadata(t *testing.T) {
	clock := &engine.Timeline{}
	player := &Character{LocalCoord: coord.Space{Width: 640, Height: 480}, Timeline: clock}
	player.GetHit.FallEnvShake = cns.HitShake{Time: 8, Amplitude: 9.5, Frequency: .001, Phase: 90.5}
	clock.BeginFrame()
	player.FallEnvShake(nil)
	if player.GetHit.FallEnvShake.Time != 0 || player.GetHit.FallEnvShake.Amplitude != 9.5 || player.HitFall {
		t.Fatal("stored metadata changed or controller required fall", player.GetHit.FallEnvShake)
	}
	clock.EndFrame()
	if clock.ShakeOffset() != -5 {
		t.Fatal("fractional receiver-local amplitude was truncated or scaled twice", clock.ShakeOffset())
	}
	for _, time := range []int{0, -1} {
		clock.BeginFrame()
		player.GetHit.FallEnvShake.Time = time
		player.FallEnvShake(nil)
		clock.EndFrame()
		if player.GetHit.FallEnvShake.Time != time || clock.ShakeOffset() != -5 {
			t.Fatal("nonpositive stored request consumed or replaced active shake", time)
		}
	}
}
