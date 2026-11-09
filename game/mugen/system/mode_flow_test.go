package main

import (
	"image/color"
	"os"
	"path/filepath"
	"regexp"
	"strings"
	"testing"

	"github.com/hajimehoshi/ebiten/v2"
	"github.com/kazzmir/paintown/game/mugen/input"
	"github.com/kazzmir/paintown/game/mugen/match"
	"github.com/kazzmir/paintown/game/mugen/motif"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

// Real round completion drives loss/continue/retry, ordered arcade victory and
// ending, then survival carry/recovery/results and training/watch mode rules.
func TestModeProgressionContinueSurvivalAndTrainingFlow(t *testing.T) {
	e := soundTestEngine(t)
	// Reuse the stock motif's resources without copying banks. Expose the
	// victory canvas so its authored BGDef color must reach the real screen.
	themeDir := t.TempDir()
	sourceDir := filepath.Dir(e.motif.Source)
	entries, err := os.ReadDir(sourceDir)
	if err != nil {
		t.Fatal(err)
	}
	for _, entry := range entries {
		if entry.Name() == filepath.Base(e.motif.Source) {
			continue
		}
		if err := os.Symlink(filepath.Join(sourceDir, entry.Name()), filepath.Join(themeDir, entry.Name())); err != nil {
			t.Fatal(err)
		}
	}
	themeSource, err := os.ReadFile(e.motif.Source)
	if err != nil {
		t.Fatal(err)
	}
	themeText := regexp.MustCompile(`(?im)^\[VictoryBG[ \t]+`).ReplaceAllString(string(themeSource), "[UnusedVictoryBG ")
	themeText = regexp.MustCompile(`(?im)^\[VictoryBGdef\][ \t]*`).ReplaceAllString(themeText, "[VictoryBGdef]\nbgclearcolor=17,34,51\nbgclearcolor=255,255,255")
	themePath := filepath.Join(themeDir, "system.def")
	if err := os.WriteFile(themePath, []byte(themeText), 0600); err != nil {
		t.Fatal(err)
	}
	e.motif, err = motif.LoadMotif(themePath, e.dataDir)
	if err != nil {
		t.Fatal(err)
	}
	e.pendingMatchSeed = 16807
	oldInput := input.GlobalManager
	t.Cleanup(func() { input.GlobalManager = oldInput })
	input.GlobalManager = input.NewInputManager(e.cfg)
	keyboard := &flowKeyboard{}
	input.GlobalManager.Keyboard = keyboard
	step := func(keys ...ebiten.Key) {
		t.Helper()
		keyboard.keys = map[ebiten.Key]bool{}
		for _, key := range keys {
			keyboard.keys[key] = true
		}
		if err := e.Update(); err != nil {
			t.Fatal(err)
		}
	}
	canvas := ebiten.NewImage(1280, 720)
	defer canvas.Deallocate()
	enterFight := func() *FightState {
		t.Helper()
		for n := 0; n < 64; n++ {
			if f, ok := e.currentState.(*FightState); ok {
				return f
			}
			step(ebiten.KeyEnter)
		}
		t.Fatalf("mode did not reach fight: %T", e.currentState)
		return nil
	}
	selectMode := func(name string) *FightState {
		t.Helper()
		e.currentState = NewSelectState(e, name)
		step(ebiten.KeyEnter)
		step(ebiten.KeyEnter)
		return enterFight()
	}
	complete := func(f *FightState, winner int) {
		t.Helper()
		found := false
		for _, section := range f.match.Fight.AST.Sections {
			if section.Name != "Round" {
				continue
			}
			for _, a := range section.Attributes {
				if a.ID.String() == "match.wins" {
					found = true
					if f.match.Request.Mode == "Survival" {
						continue // Exercise the real default, which is two for ordinary matches.
					}
					a.Value = parsers.NumberValue{Val: 1}
					a.RawValue = "1"
					found = true
				}
			}
		}
		if !found {
			t.Fatal("stock round target missing")
		}
		f.match.Options.Input = func(int, int) []string { return nil }
		for _, p := range f.match.Actors() {
			p.Character.InputSource = f.match.Options.Input
		}
		for n := 0; n < 700 && f.match.Phase != match.Active; n++ {
			step()
		}
		if f.match.Phase != match.Active {
			t.Fatal("mode fight did not release control")
		}
		for _, p := range f.match.SidePlayers(1 - winner) {
			p.Character.SetLife(0)
			p.Character.ChangeState(5150, 0, -1)
		}
		for n := 0; n < 800 && e.currentState == f; n++ {
			step()
		}
		if e.currentState == f {
			t.Fatal("completed mode match did not route its result", f.match.Phase)
		}
	}
	f := selectMode("Arcade Mode")
	r := e.modeRun
	if r == nil || len(r.opponents) < 2 || f.match.Request.MatchNo != 1 {
		t.Fatal("arcade schedule/match context missing")
	}
	firstOpponent := f.match.Request.Players[1].DefPath
	complete(f, 1)
	if _, ok := e.currentState.(*ContinueScreenState); !ok {
		t.Fatalf("arcade loss did not offer continue: %T", e.currentState)
	}
	e.Draw(canvas)
	step(ebiten.KeyEnter)
	f = enterFight()
	if f.match.Request.Players[1].DefPath != firstOpponent || f.match.Request.MatchNo != 1 || e.modeRun != r {
		t.Fatal("continue advanced/replaced the current opponent")
	}
	complete(f, 0)
	if _, ok := e.currentState.(*WinScreenState); !ok {
		t.Fatalf("nonfinal arcade win omitted victory: %T", e.currentState)
	}
	win := e.currentState.(*WinScreenState)
	win.winner.Character.VictoryQuote(2)
	e.cfg.Config.Language = "ja"
	if quote := win.victoryQuote(); quote != "十年早いんだぞ" {
		t.Fatal("authored localized VictoryQuote did not reach presentation", quote)
	}
	e.Draw(canvas)
	if got := color.RGBAModel.Convert(canvas.At(0, 0)).(color.RGBA); got != (color.RGBA{17, 34, 51, 255}) {
		t.Fatal("victory BGDef clear color did not reach the screen", got)
	}
	step(ebiten.KeyEnter)
	f = enterFight()
	if f.match.Request.MatchNo != 2 || f.match.World.MatchNo != 2 || r.index != 1 {
		t.Fatal("arcade did not advance MatchNo/opponent schedule")
	}
	complete(f, 0)
	if _, ok := e.currentState.(*StoryboardState); !ok {
		t.Fatalf("selected character ending was not entered: %T", e.currentState)
	}
	for n := 0; n < 64; n++ {
		if _, ok := e.currentState.(*TitleState); ok {
			break
		}
		step(ebiten.KeyEnter)
	}
	if _, ok := e.currentState.(*TitleState); !ok {
		t.Fatalf("ending did not return to title: %T", e.currentState)
	}
	step() // Releases the completed campaign.
	f = selectMode("Survival")
	f.match.Players[0].Character.SetLife(400)
	f.match.Players[0].Character.SetPower(500)
	complete(f, 0)
	if f.match.Round != 1 || f.match.Wins[0] != 1 {
		t.Fatal("Survival repeated an opponent using the ordinary match win target")
	}
	f = enterFight()
	if f.match.Request.MatchNo != 1 || f.match.Round != 2 || f.match.Players[0].Character.RoundsExisted != 1 || f.match.Players[1].Character.RoundsExisted != 0 || f.match.Players[0].Character.Life != 798 || f.match.Players[0].Character.GetPower() != 500 {
		t.Fatal("survival did not carry/recover life and power", f.match.Players[0].Character.Life, f.match.Players[0].Character.GetPower())
	}
	complete(f, 1)
	result, ok := e.currentState.(*GameOverScreenState)
	if !ok || !result.survival || result.score != 1 {
		t.Fatalf("survival loss did not show the actual streak: %T", e.currentState)
	}
	e.Draw(canvas)
	step(ebiten.KeyEnter)
	step()
	// The same campaign path now carries a Turns reserve's original roster
	// position. Losing the first fighter must not respawn him next opponent.
	selectTurns := NewSelectState(e, "Survival")
	e.currentState = selectTurns
	if !selectTurns.teamMenu || !selectTurns.teamReady[1] {
		t.Fatal("Survival omitted its player team menu")
	}
	step(ebiten.KeyArrowDown)
	step(ebiten.KeyArrowDown)
	step(ebiten.KeyEnter)
	step(ebiten.KeyEnter)
	step(ebiten.KeyArrowRight)
	step(ebiten.KeyEnter)
	f = enterFight()
	if f.match.Teams[0].Mode != match.Turns || len(f.match.Teams[0].Members) != 2 {
		t.Fatal("Survival Turns selection did not reach match")
	}
	for _, selected := range f.match.Request.Teams[0].Members {
		if selected.AILevel != 0 {
			t.Fatal("a human Turns reserve was assigned AI")
		}
	}
	f.match.Options.Input = func(int, int) []string { return nil }
	for _, p := range f.match.Actors() {
		p.Character.InputSource = f.match.Options.Input
		p.Character.AILevel = 0
	}
	for n := 0; n < 700 && f.match.Phase != match.Active; n++ {
		step()
	}
	f.match.Players[0].Character.SetLife(0)
	f.match.Players[0].Character.ChangeState(5150, 0, -1)
	for n := 0; n < 800 && f.match.Round == 1 && e.currentState == f; n++ {
		step()
	}
	if e.currentState != f || f.match.Round != 2 || f.match.Teams[0].Active != 1 {
		t.Fatal("Survival did not consume the defeated Turns member")
	}
	reserve := f.match.Request.Teams[0].Members[1]
	f.match.Players[0].Character.SetLife(400)
	f.match.Players[0].Character.SetPower(700)
	complete(f, 0)
	carried := e.modeRun.carry[1].Life
	if carried <= 400 || e.modeRun.template.Teams[0].StartMember != 1 {
		t.Fatal("Survival omitted reserve recovery/roster carry", carried)
	}
	f = enterFight()
	if f.match.Request.MatchNo != 1 || f.match.Round != 3 || f.match.Players[0].Character.RoundsExisted != 1 || f.match.Teams[0].Active != 1 || len(f.match.Teams[0].Members) != 2 || f.match.Request.Players[0] != reserve || f.match.Players[0].Character.Life != carried || f.match.Players[0].Character.GetPower() != 700 || f.match.Players[0].Character.AILevel != 0 {
		t.Fatal("next Survival opponent revived/replaced the reserve or lost its carried state")
	}
	e.Draw(canvas)
	complete(f, 1)
	if result, ok := e.currentState.(*GameOverScreenState); !ok || !result.survival || result.score != 1 {
		t.Fatalf("exhausted Survival roster did not end its streak: %T", e.currentState)
	}
	step(ebiten.KeyEnter)
	step()
	// Native Survival Simul keeps667 maximum life and recovers a defeated
	// partner without reviving it during the result. The next bout restores
	// that recovered amount, including the partial partner and shared power.
	selectSimul := NewSelectState(e, "Survival")
	// A tiny writable CNS/DEF overlay reuses KFM sprites/AIR/CMD/SND in place.
	// Its state5900 attribute requires restored counters/variables/game time,
	// making late restoration observable before the first controller pass.
	directory := t.TempDir()
	original, err := filepath.Abs(filepath.Join(e.dataDir, "chars/kfm"))
	if err != nil {
		t.Fatal(err)
	}
	constants, err := os.ReadFile(filepath.Join(original, "kfm.cns"))
	if err != nil {
		t.Fatal(err)
	}
	text := strings.SplitN(strings.ReplaceAll(string(constants), "\r\n", "\n"), "[Statedef", 2)[0]
	text = strings.Replace(text, "IntPersistIndex = 60", "IntPersistIndex = 58", 1)
	text = strings.Replace(text, "FloatPersistIndex = 40", "FloatPersistIndex = 38", 1)
	text += `[Statedef 5900]
type=S
movetype=I
physics=N
anim=ifelse(var(58)=123 && fvar(38)=2.5 && RoundNo>1 && RoundsExisted>0 && GameTime>0,20,0)
[State 5900, enter]
type=ChangeState
trigger1=1
value=0
[Statedef 0]
type=S
physics=N
movetype=I
ctrl=1
[State 0, idle]
type=Null
trigger1=1
`
	cnsPath := filepath.Join(directory, "carry.cns")
	if err := os.WriteFile(cnsPath, []byte(text), 0600); err != nil {
		t.Fatal(err)
	}
	definition, err := os.ReadFile(filepath.Join(original, "kfm.def"))
	if err != nil {
		t.Fatal(err)
	}
	def := string(definition)
	for key, path := range map[string]string{"cns": cnsPath, "st": cnsPath, "cmd": filepath.Join(original, "kfm.cmd"), "anim": filepath.Join(original, "kfm.air"), "sprite": filepath.Join(original, "kfm.sff"), "sound": filepath.Join(original, "kfm.snd")} {
		pattern := regexp.MustCompile(`(?im)^\s*` + key + `\s*=.*$`)
		def = pattern.ReplaceAllStringFunc(def, func(string) string { return key + " = " + path })
	}
	defPath := filepath.Join(directory, "carry.def")
	if err := os.WriteFile(defPath, []byte(def), 0600); err != nil {
		t.Fatal(err)
	}
	cell := *selectSimul.chars[0]
	cell.DefPath = defPath
	selectSimul.chars[0] = &cell
	e.currentState = selectSimul
	step(ebiten.KeyArrowDown)
	step(ebiten.KeyEnter)
	step(ebiten.KeyEnter)
	step(ebiten.KeyArrowRight)
	step(ebiten.KeyEnter)
	f = enterFight()
	settle := func(f *FightState) {
		t.Helper()
		f.match.Options.Input = func(int, int) []string { return nil }
		for _, p := range f.match.Actors() {
			p.Character.InputSource = f.match.Options.Input
			p.Character.AILevel = 0
		}
		for n := 0; n < 700 && f.match.Phase != match.Active; n++ {
			step()
		}
		if f.match.Phase != match.Active {
			t.Fatal("Survival Simul did not release control")
		}
	}
	settle(f)
	leader, partner := f.match.SidePlayers(0)[0].Character, f.match.SidePlayers(0)[1].Character
	if leader.MaxLife != 667 || partner.MaxLife != 667 {
		t.Fatal("Survival Simul life normalization", leader.MaxLife, partner.MaxLife)
	}
	leader.SetLife(400)
	leader.SetPower(500)
	leader.IntVars[57], leader.IntVars[58] = 11, 123
	leader.FloatVars[37], leader.FloatVars[38] = 1.5, 2.5
	partner.SetLife(0)
	partner.ChangeState(5150, 0, -1)
	for n := 0; n < 30; n++ {
		step()
	}
	complete(f, 0)
	if leader.Life != 664 || partner.Life != 264 || partner.IsAlive() || leader.IsMatchOver {
		t.Fatal("native defeated-partner recovery/result latch", leader.Life, partner.Life, partner.IsAlive())
	}
	random := f.match.World.RandomState()
	wantRandom := random.Next()
	f = enterFight()
	leader, partner = f.match.SidePlayers(0)[0].Character, f.match.SidePlayers(0)[1].Character
	if leader.Life != 664 || partner.Life != 264 || !partner.IsAlive() || partner.GetPower() != 500 {
		t.Fatal("Survival replaced partial partner recovery with full health")
	}
	random = f.match.World.RandomState()
	if f.match.Round != 2 || f.match.World.MatchNo != 1 || leader.RoundsExisted != 1 || partner.RoundsExisted != 1 || leader.IntVars[57] != 0 || leader.IntVars[58] != 123 || leader.FloatVars[37] != 0 || leader.FloatVars[38] != 2.5 || leader.GetAnim() != 20 || random.Next() != wantRandom {
		t.Fatal("Survival counters/declared variables/RNG were not restored before5900", f.match.Round, leader.RoundsExisted, leader.IntVars[57:59], leader.FloatVars[37:39], leader.GetAnim())
	}
	settle(f)
	for n := 0; n < 30; n++ {
		step()
	}
	complete(f, 0)
	f = enterFight()
	if f.match.Round != 3 || f.match.Players[0].Character.RoundsExisted != 2 || f.match.Players[0].Character.Life != 667 || f.match.Teams[0].Members[1].Character.Life != 528 {
		t.Fatal("Survival second recovery/cap", f.match.Players[0].Character.Life, f.match.Teams[0].Members[1].Character.Life)
	}
	complete(f, 1)
	if result, ok := e.currentState.(*GameOverScreenState); !ok || result.score != 2 {
		t.Fatalf("Survival Simul defeat did not finish its streak: %T", e.currentState)
	}
	step(ebiten.KeyEnter)
	step()
	f = selectMode("Training")
	for n := 0; n < 700 && f.match.Phase != match.Active; n++ {
		step()
	}
	root := f.match.Players[0].Character
	root.SetLife(0)
	step()
	if f.match.TimeLeft != -1 || f.match.Phase != match.Active || !root.IsAlive() || root.Life != root.MaxLife || root.GetPower() != root.MaxPower {
		t.Fatal("training timer/KO/refill rules missing")
	}
	step(ebiten.KeyEscape)
	step()
	f = selectMode("Watch Mode")
	for _, p := range f.match.Actors() {
		if p.Character.AILevel <= 0 {
			t.Fatal("watch mode left a human-controlled side")
		}
	}
	step(ebiten.KeyEscape)
}
