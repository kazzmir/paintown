package match

import (
	"github.com/kazzmir/paintown/game/mugen/character"
	"math"
)

func (m *Match) cameraActors() []*character.Character {
	actors := make([]*character.Character, 0, len(m.Players)+len(m.World.Helpers))
	for _, player := range m.Players {
		actors = append(actors, player.Character)
	}
	return append(actors, m.World.Helpers...)
}

// The native zoom-one horizontal camera keeps a tension region, eases after
// each entity update and in the shared late phase. Both endpoints and the
// movement delta round independently before the center/zoom are published.
func (m *Match) updateHorizontalCamera() {
	stageScale, _ := m.Stage.StageInfo.LocalCoord.ScaleTo(m.Options.GameSpace)
	left, right := math.Inf(1), math.Inf(-1)
	leftVelocity, rightVelocity := 0.0, 0.0
	participants := false
	verticalParticipants := false
	for _, actor := range m.cameraActors() {
		verticalParticipants = verticalParticipants || actor.CameraMovementEnabled(1)
		if !actor.CameraMovementEnabled(0) {
			continue
		}
		participants = true
		scale := float64(m.Options.GameSpace.Width) / float64(actor.LocalCoordinateWidth())
		x := actor.X * scale
		velocity := actor.VelX * float64(actor.Facing) * scale
		if x < left {
			left, leftVelocity = x, velocity
		}
		if x > right {
			right, rightVelocity = x, velocity
		}
	}
	// The beta freezes camera updates if either axis has no participants,
	// even when the other axis still has eligible actors.
	if !participants || !verticalParticipants {
		return
	}
	half := float64(m.Options.GameSpace.Width) / 2
	threshold := half - m.Stage.Camera.Tension*stageScale
	low, high := right-threshold, left+threshold
	wanted := m.cameraGoalX
	if low > high {
		wanted = (left + right) / 2
	} else {
		wanted = math.Max(low, math.Min(high, wanted))
	}
	wanted = math.Max(m.Stage.Camera.BoundLeft*stageScale, math.Min(m.Stage.Camera.BoundRight*stageScale, wanted))
	m.cameraGoalX = wanted
	wantedLeft, wantedRight := float32(wanted-half), float32(wanted+half)
	bias := float32(float64(float32(.1)) * float64(m.Options.GameSpace.Width) / 320)
	advance := func(currentEdge, wantedEdge float32) float32 {
		if currentEdge == wantedEdge {
			return currentEdge
		}
		direction := math.Copysign(1, float64(wantedEdge-currentEdge))
		delta := float32(.95*float64(currentEdge) + .05*float64(wantedEdge) - float64(currentEdge) + float64(bias)*direction)
		if direction > 0 {
			if right > m.CameraX+threshold && rightVelocity > 0 {
				delta = max(delta, float32(rightVelocity))
			}
			return min(currentEdge+delta, wantedEdge)
		}
		if left < m.CameraX-threshold && leftVelocity < 0 {
			delta = min(delta, float32(leftVelocity))
		}
		return max(currentEdge+delta, wantedEdge)
	}
	m.cameraLeft, m.cameraRight = advance(m.cameraLeft, wantedLeft), advance(m.cameraRight, wantedRight)
	width := float64(m.Options.GameSpace.Width)
	if float64(m.cameraRight)-float64(m.cameraLeft) > width {
		if m.cameraRight <= wantedRight {
			m.cameraLeft = float32(float64(m.cameraRight) - width)
		} else if m.cameraLeft >= wantedLeft {
			m.cameraRight = float32(float64(m.cameraLeft) + width)
		}
	}
	m.CameraX = float64(float32((float64(m.cameraLeft) + float64(m.cameraRight)) / 2))
	m.Viewport.Zoom = float64(float32(width / (float64(m.cameraRight) - float64(m.cameraLeft))))
	m.Viewport.CameraX = m.CameraX
	for _, player := range m.Players {
		scale, _ := m.Options.GameSpace.ScaleTo(player.LocalCoord)
		player.Character.CameraX = m.CameraX * scale
	}
}
