package match

import "math"

// The native zoom-one horizontal camera keeps a tension region, eases after
// each root update and again in the shared late phase, and clamps at its goal.
// Its single-precision left edge matters even when scripts expose the center.
func (m *Match) updateHorizontalCamera() {
	stageScale, _ := m.Stage.StageInfo.LocalCoord.ScaleTo(m.Options.GameSpace)
	left, right := math.Inf(1), math.Inf(-1)
	leftVelocity, rightVelocity := 0.0, 0.0
	participants := false
	verticalParticipants := false
	for _, player := range m.Players {
		verticalParticipants = verticalParticipants || player.Character.CameraMovementEnabled(1)
		if !player.Character.CameraMovementEnabled(0) {
			continue
		}
		participants = true
		scale, _ := player.LocalCoord.ScaleTo(m.Options.GameSpace)
		x := player.Character.X * scale
		velocity := player.Character.VelX * float64(player.Character.Facing) * scale
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
	currentEdge := float32(m.CameraX - half)
	wantedEdge := float32(wanted - half)
	if currentEdge != wantedEdge {
		margin := 2 * float64(m.Options.GameSpace.Width) / 320
		direction := math.Copysign(1, float64(wantedEdge-currentEdge))
		step := float32(.95*float64(currentEdge) + .05*(float64(wantedEdge)+margin*direction))
		if direction > 0 {
			// The actor beyond the active tension edge also imposes a speed
			// floor. An interior moving actor does not impose that constraint.
			if right > m.CameraX+threshold && rightVelocity > 0 {
				step = max(step, float32(float64(currentEdge)+rightVelocity))
			}
			step = min(step, wantedEdge)
		} else {
			if left < m.CameraX-threshold && leftVelocity < 0 {
				step = min(step, float32(float64(currentEdge)+leftVelocity))
			}
			step = max(step, wantedEdge)
		}
		m.CameraX = float64(step) + half
	}
	m.Viewport.CameraX = m.CameraX
	for _, player := range m.Players {
		scale, _ := m.Options.GameSpace.ScaleTo(player.LocalCoord)
		player.Character.CameraX = m.CameraX * scale
	}
}
