package storyboard

import (
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
)

func TestInvalidPlaybackStartCannotPanic(t *testing.T) {
	for _, story := range []*Storyboard{nil, {}, {StartScene: -1, Scenes: []*Scene{{}}}, {StartScene: 1, Scenes: []*Scene{{}}}, {Scenes: []*Scene{nil}}} {
		player := NewPlayer(story)
		if player.Error == nil || !player.Update(InputState{}) {
			t.Fatal("invalid start was accepted")
		}
		player.Draw(nil)
	}
}

func TestAnimationFailureHasStoryboardContext(t *testing.T) {
	animation := air.NewPlayer(&air.Action{LoopStart: 1, Elements: []air.Element{{Time: 1}, {Time: 0}}})
	story := &Storyboard{Source: "intro.def", Scenes: []*Scene{{Name: "Scene 0", EndTime: 10, Layers: []*Layer{{Index: 4, Player: animation}}}}}
	player := NewPlayer(story)
	for i := 0; i < 3 && player.Error == nil; i++ {
		player.Update(InputState{})
	}
	if player.Error == nil || !strings.Contains(player.Error.Error(), "intro.def [Scene 0] layer4") {
		t.Fatal(player.Error)
	}
}
