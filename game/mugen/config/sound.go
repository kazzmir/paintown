package config

import "fmt"

// SoundConfig retains the 1.1 sound settings. Resampling method/quality and BGM
// are retained for later fidelity work; the current SFX mixer uses linear PCM.
type SoundConfig struct {
	Sound, StereoEffects, ReverseStereo    bool
	SampleRate, BufferSize, WavChannels    int
	PanningWidth                           int
	MasterVolume, WavVolume, BGMVolume     int
	SFXResampleMethod                      string
	SFXResampleQuality, BGMResampleQuality int
}

func DefaultSoundConfig() SoundConfig {
	return SoundConfig{Sound: true, StereoEffects: true, SampleRate: 44100, BufferSize: 11, WavChannels: 32,
		PanningWidth: 240, MasterVolume: 100, WavVolume: 80, BGMVolume: 75, SFXResampleMethod: "libresample"}
}

func (s SoundConfig) Validate() error {
	// Resource bounds are explicit implementation limits, not measured native
	// invalid-configuration fallback rules.
	if s.SampleRate < 8000 || s.SampleRate > 192000 {
		return fmt.Errorf("SampleRate must be in 8000..192000")
	}
	if s.BufferSize < 8 || s.BufferSize > 15 {
		return fmt.Errorf("BufferSize must be in 8..15")
	}
	if s.WavChannels < 1 || s.WavChannels > 1024 {
		return fmt.Errorf("WavChannels must be in 1..1024")
	}
	for _, volume := range []struct {
		name  string
		value int
	}{{"MasterVolume", s.MasterVolume}, {"WavVolume", s.WavVolume}, {"BGMVolume", s.BGMVolume}} {
		if volume.value < 0 || volume.value > 100 {
			return fmt.Errorf("%s must be in 0..100 but was %d", volume.name, volume.value)
		}
	}
	return nil
}

func (s SoundConfig) EffectsVolume() float32 {
	if !s.Sound {
		return 0
	}
	return float32(s.MasterVolume) * float32(s.WavVolume) / 10000
}
