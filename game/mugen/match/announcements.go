package match

// AnnouncementTimes are simulation ages, independent of the current match
// phase and of a graphic's display lifetime. A negative age is not ready yet.
// The HUD owns real-frame animation/expiry after a message starts.
type AnnouncementTimes struct {
	Round, Fight, Finish, Win int
}

const announcementInactive = -2

func (m *Match) resetAnnouncements() {
	m.Announcements = AnnouncementTimes{announcementInactive, announcementInactive, announcementInactive, announcementInactive}
}

func (m *Match) advanceAnnouncements(paused bool) {
	if paused && m.Phase != PreOver && m.Phase != Over {
		return
	}
	for _, age := range []*int{&m.Announcements.Round, &m.Announcements.Fight, &m.Announcements.Finish, &m.Announcements.Win} {
		if *age >= -1 {
			(*age)++
		}
	}
}

// Enter the result state before this frame's CNS pass. In particular,
// over.wintime=0 makes StateNo180 observable on the first RoundState4 pass.
func (m *Match) startWinPose() error {
	if m.Phase != Over || m.winPoseStarted || m.PhaseTime < max(0, m.Setting("Round", "over.wintime", 45)) {
		return nil
	}
	m.winPoseStarted = true
	for _, p := range m.Actors() {
		if !p.Character.IsAlive() {
			continue
		}
		state := 170
		if m.Winner < 0 {
			state = 175
		} else if p.Character.TeamSide-1 == m.Winner {
			state = 180
		}
		p.Character.ChangeState(state, 0, -1)
		if p.Character.RuntimeError != nil {
			return p.Character.RuntimeError
		}
	}
	return nil
}
