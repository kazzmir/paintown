package match

// ForceTimeOver uses the ordinary result pipeline, including team life fractions,
// announcements, pose entry and result commitment. It cannot end a second round
// while the previous result is already being displayed.
func (m *Match) ForceTimeOver() bool {
	if m.Phase != Active {
		return false
	}
	m.finishRound()
	return true
}
