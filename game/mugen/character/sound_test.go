package character

import (
	"bytes"
	"crypto/sha256"
	"encoding/binary"
	"encoding/hex"
	"encoding/json"
	"math"
	"math/cmplx"
	"os"
	"strconv"
	"strings"
	"testing"

	"github.com/kazzmir/paintown/game/mugen/air"
	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/coord"
	"github.com/kazzmir/paintown/game/mugen/snd"
	"github.com/kazzmir/paintown/game/mugen/sound"
)

type audioReferenceCase struct {
	CNSHash     string            `json:"cns_sha256"`
	AIRHash     string            `json:"air_sha256"`
	ExtraHashes map[string]string `json:"extra_sha256"`
	Captures    []struct{ Variables map[string]float64 }
	Audio       struct {
		Frequencies []float64      `json:"frequencies"`
		Tolerance   float64        `json:"amplitude_tolerance"`
		Settings    map[string]int `json:"settings"`
		Segments    []struct {
			Levels map[string][2]float64 `json:"levels"`
		} `json:"segments"`
		Checks           []struct{ Tick, Segment, WindowTicks int } `json:"checks"`
		MissingResources int                                        `json:"missing_resources"`
	} `json:"audio"`
}

func checkedAudioInput(t *testing.T, path, want string) []byte {
	t.Helper()
	data, err := os.ReadFile(path)
	if err != nil {
		t.Fatal(err)
	}
	hash := sha256.Sum256(data)
	if hex.EncodeToString(hash[:]) != want {
		t.Fatal("sound fixture changed since native capture", path)
	}
	return data
}

func audioLevels(data []byte, rate int, frequencies []float64) map[string][2]float64 {
	frames := len(data) / 4
	result := map[string][2]float64{}
	for _, frequency := range frequencies {
		var sums [2]complex128
		var total float64
		for frame := 0; frame < frames; frame++ {
			window := .5 - .5*math.Cos(2*math.Pi*float64(frame)/float64(frames-1))
			oscillator := cmplx.Rect(window, -2*math.Pi*frequency*float64(frame)/float64(rate))
			for channel := 0; channel < 2; channel++ {
				value := int16(binary.LittleEndian.Uint16(data[frame*4+channel*2:]))
				sums[channel] += complex(float64(value), 0) * oscillator
			}
			total += window
		}
		result[strconv.FormatFloat(frequency, 'g', -1, 64)] = [2]float64{2 * cmplx.Abs(sums[0]) / total, 2 * cmplx.Abs(sums[1]) / total}
	}
	return result
}

func TestNativeSoundControllerPCM(t *testing.T) {
	for _, name := range []string{"sound-pan", "sound-pan-ramp", "sound-pan-options", "sound-volume", "sound-channels", "sound-pan-start", "sound-frequency", "sound-frequency-invalid", "sound-pan-common-units", "sound-stereo", "sound-prefix-audio", "contact-sound", "guard-sound", "override-sound", "contact-sound-close", "contact-sound-common", "contact-sound-default", "guard-sound-default", "contact-sound-common-single", "contact-sound-retained", "ko-sound", "ko-sound-echo", "ko-sound-suppressed", "ko-sound-initial-suppressed",
		"superpause-sound-own", "superpause-sound-common", "superpause-sound-common-single"} {
		t.Run(name, func(t *testing.T) {
			stem := "../tests/testdata/reference/" + name
			data, err := os.ReadFile(stem + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var catalogue struct{ Cases map[string]audioReferenceCase }
			if err := json.Unmarshal(data, &catalogue); err != nil {
				t.Fatal(err)
			}
			for caseName, reference := range catalogue.Cases {
				t.Run(caseName, func(t *testing.T) {
					source := checkedAudioInput(t, stem+".cns", reference.CNSHash)
					frames := checkedAudioInput(t, stem+".air", reference.AIRHash)
					bankData := checkedAudioInput(t, stem+".extra/probe.snd", reference.ExtraHashes["probe.snd"])
					bank, err := snd.Parse(bytes.NewReader(bankData))
					if err != nil {
						t.Fatal(err)
					}
					common := bank
					if hash, present := reference.ExtraHashes["common-probe.snd"]; present {
						data := checkedAudioInput(t, stem+".extra/common-probe.snd", hash)
						common, err = snd.Parse(bytes.NewReader(data))
						if err != nil {
							t.Fatal(err)
						}
					}
					file, err := cns.ParseNamed(bytes.NewReader(source), stem+".cns")
					if err != nil {
						t.Fatal(err)
					}
					file.Data.Life = 1000
					actions, err := air.Parse(bytes.NewReader(frames))
					if err != nil {
						t.Fatal(err)
					}
					settings := reference.Audio.Settings
					if len(settings) == 0 || len(reference.Audio.Checks) == 0 {
						t.Fatal("native audio settings/tick checks are missing")
					}
					mixer, err := sound.NewMixer(sound.Options{SampleRate: 44100, MaxVoices: settings["WavChannels"], Volume: float32(settings["MasterVolume"]*settings["WavVolume"]) / 10000,
						Stereo: settings["StereoEffects"] != 0, Reverse: settings["ReverseStereo"] != 0, PanningWidth: float32(settings["PanningWidth"])})
					if err != nil {
						t.Fatal(err)
					}
					queue := &sound.Queue{}
					players := []*Character{NewCharacter(file, actions, nil), NewCharacter(file, actions, nil)}
					for slot, c := range players {
						c.GameCoord = coord.Space{Width: settings["GameWidth"], Height: settings["GameHeight"]}
						c.BindSound(queue, bank, common)
						c.PlayerID, c.Facing, c.X = slot+1, 1, -70
						if slot == 1 {
							c.Facing, c.X = -1, 70
						}
						c.Target = []*Character{players[1-slot]}
						c.ChangeState(0, -1, -1)
						c.Ctrl = true
					}
					last := 0
					for _, check := range reference.Audio.Checks {
						if check.Segment < 0 || check.Segment >= len(reference.Audio.Segments) {
							t.Fatal("invalid native audio check segment", check)
						}
						last = max(last, check.Tick)
					}
					const tickBytes = 44100 / 60 * 4
					pcm := make([]byte, (last+13)*tickBytes)
					for tick := 0; tick < last+13; tick++ {
						if err := TickCharacters(players); err != nil {
							t.Fatal(err)
						}
						for _, command := range queue.Drain() {
							if err := mixer.Apply(command); err != nil {
								t.Fatal(err)
							}
						}
						if _, err := mixer.Read(pcm[tick*tickBytes : (tick+1)*tickBytes]); err != nil {
							t.Fatal(err)
						}
					}
					for _, check := range reference.Audio.Checks {
						window := check.WindowTicks
						if window == 0 {
							window = 12
						}
						if window < 1 || window > 12 {
							t.Fatal("audio check window outside 1..12 ticks")
						}
						levels := audioLevels(pcm[check.Tick*tickBytes:(check.Tick+window)*tickBytes], 44100, reference.Audio.Frequencies)
						for frequency, want := range reference.Audio.Segments[check.Segment].Levels {
							got := levels[frequency]
							for channel := 0; channel < 2; channel++ {
								if math.Abs(got[channel]-want[channel]) > reference.Audio.Tolerance {
									t.Errorf("tick %d tone %s channel %d: amplitude %.2f native %.2f", check.Tick, frequency, channel, got[channel], want[channel])
								}
							}
						}
					}
					for _, capture := range reference.Captures {
						for key, want := range capture.Variables {
							index, err := strconv.Atoi(key)
							if err != nil {
								t.Fatal(err)
							}
							if got := players[0].FloatVars[index]; float32(got) != float32(want) {
								t.Errorf("fvar(%d)=%v native=%v", index, got, want)
							}
						}
					}
					missing := 0
					for _, c := range players {
						for _, warning := range c.RuntimeWarnings {
							if strings.Contains(warning, "PlaySnd sound ") {
								missing++
							}
						}
					}
					if missing != reference.Audio.MissingResources {
						t.Errorf("missing sound warnings=%d native case expects %d", missing, reference.Audio.MissingResources)
					}
				})
			}
		})
	}
}

func TestNativeSoundParameterGrammar(t *testing.T) {
	for _, name := range []string{"sound-prefix-paren", "sound-prefix-lower", "sound-prefix-own", "sound-prefix-fvar", "sound-prefix-ffvar", "sound-prefix-facing", "sound-ignored-volume", "sound-both-pan", "sound-missing-value", "sound-pan-missing", "sound-stop-missing", "sound-prefix-protected", "hit-sound-fprefix", "hit-sound-expression", "hit-sound-single", "hit-sound-invalid", "hit-sound-facing"} {
		t.Run(name, func(t *testing.T) {
			stem := "../tests/testdata/reference/" + name
			data, err := os.ReadFile(stem + ".json")
			if err != nil {
				t.Fatal(err)
			}
			var evidence struct {
				CNSHash      string `json:"cns_sha256"`
				AIRHash      string `json:"air_sha256"`
				ExpectedFail string `json:"expected_load_error"`
				Captures     []struct{ Variables map[string]float64 }
			}
			if err := json.Unmarshal(data, &evidence); err != nil {
				t.Fatal(err)
			}
			source := checkedAudioInput(t, stem+".cns", evidence.CNSHash)
			frames := checkedAudioInput(t, stem+".air", evidence.AIRHash)
			file, err := cns.ParseNamed(bytes.NewReader(source), stem+".cns")
			if err == nil {
				err = NewStateMachine().CompileState(file.States[700])
			}
			if evidence.ExpectedFail != "" {
				if err == nil {
					t.Fatal("accepted native-rejected sound parameters")
				}
				return
			}
			if err != nil {
				t.Fatal(err)
			}
			actions, err := air.Parse(bytes.NewReader(frames))
			if err != nil {
				t.Fatal(err)
			}
			c := NewCharacter(file, actions, nil)
			c.ChangeState(0, -1, -1)
			c.Ctrl = true
			for tick := 0; tick < 40; tick++ {
				c.Update()
				if c.RuntimeError != nil {
					t.Fatal(c.RuntimeError)
				}
			}
			for _, capture := range evidence.Captures {
				for key, want := range capture.Variables {
					index, err := strconv.Atoi(key)
					if err != nil {
						t.Fatal(err)
					}
					if got := c.FloatVars[index]; got != want {
						t.Errorf("fvar(%d)=%v native=%v", index, got, want)
					}
				}
			}
		})
	}
}

func TestSoundBanksAndOwnerDoNotFollowInputSlot(t *testing.T) {
	file, _ := cns.Parse(strings.NewReader("[Data]\nlife=1000\n"))
	queue := &sound.Queue{}
	own, common := &snd.SoundBank{Map: map[string]*snd.Sound{"1-0": {}}}, &snd.SoundBank{Map: map[string]*snd.Sound{"1-0": {}}}
	a, b := NewCharacter(file, nil, nil), NewCharacter(file, nil, nil)
	a.BindSound(queue, own, common)
	b.BindSound(queue, own, common)
	// Helpers may share a keyboard/player slot while owning independent voices.
	a.PlayerID, b.PlayerID = 1, 1
	if err := a.PlaySound(sound.PlayParameters{Group: 1, Channel: 0, VolumeScale: 100, Frequency: 1}); err != nil {
		t.Fatal(err)
	}
	if err := b.PlaySound(sound.PlayParameters{Common: true, Group: 1, Channel: 0, VolumeScale: 100, Frequency: 1}); err != nil {
		t.Fatal(err)
	}
	commands := queue.Drain()
	if len(commands) != 2 || commands[0].Bank != own || commands[1].Bank != common || commands[0].Owner == commands[1].Owner {
		t.Fatal("sound bank/owner aliases input slot", commands)
	}
}
