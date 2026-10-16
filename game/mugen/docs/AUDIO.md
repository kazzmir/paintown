# Sound implementation and reference evidence

M4l is committed as the controller/mixer foundation; production playback, HUD
and contact integration is the next checkpoint. `sound/` has a display-independent ordered command queue and a
thread-safe signed-16 stereo PCM reader. Character sound controllers now compile
typed tuples/parameters and emit commands; source banks remain immutable CPU
assets. Helpers/projectiles receive independent audio owners on the shared
queue, separate from their currently conflated gameplay/input IDs. Full entity
ownership and lifecycle behavior remains a separate obligation.

Native audio is captured through a task-owned PulseAudio 16.1 server with only
a null sink and an authenticated Unix socket under `temp/audio-reference/`.
No physical audio route or desktop audio configuration is changed. The server
was started in exec session 77895; stop only that task-owned process when this
work ends. Xvfb :97 remains the task-owned display (see BUILD.md).

Pulse server runtime/state/cookie/config/log files all stay under that temp
directory. Never commit or print the cookie. `PULSE_SERVER` points to
`unix:/home/juvinious/projects/paintown/game/mugen/temp/audio-reference/native`;
`PULSE_COOKIE` points to that directory's cookie. The server configuration loads
module-null-sink at 44100 Hz stereo and module-native-protocol-unix with the
explicit cookie path. No host Pulse server is stopped or reconfigured.

`tests/sound_probe.py` requires an explicit Unix server under MUGEN temp and
records only its `mugen_reference.monitor`, never a microphone or host monitor.
It delegates native/window ownership to reference_probe.py, saves PCM under
temp/audio-runs/, and extracts persistent tone amplitudes using Hann-windowed
Fourier measurements. Short transitions are excluded; equivalent plateaus around
transient gaps are merged. Source/AIR/SND/executable/config/PCM hashes, sound
settings and measurements are pinned in each fixture's JSON. Go tick checks
sample the authored cue plateaus; wall-clock startup offsets are not compared.
These are controlled tone/control checks, not exact full-waveform parity.

The original tone bank is reproducible with `tests/generate_sound_bank.py`.
Its groups 1/2/3 loop at 441/882/1323 Hz, group 4 is a one-second 441 Hz sample,
and group 5 has separate 441/882 Hz stereo channels. No native sound assets are
modified. The first authored draft had an extra NUL in its SND signature;
sound-pan-native is a diagnostic rejection, superseded by repaired captures.

Twenty-three programs cover 24 cases (twelve audio and twelve grammar cases),
with 55 stable native segments and 63 Go tick checks. Full races pass; a fresh
channel replay matches amplitudes and relative cue onsets.

Current native observations:

- Sound channels are per owner; equal channel numbers on two players coexist.
  A new numbered voice replaces the old one unless the new request has
  lowpriority=1. Automatic voices coexist; StopSnd -1 stops all players' voices.
- Native panning converts to integer 240p offset, removes a +/-40 center zone,
  scales the remainder by 128/(PanningWidth-20), truncates and clamps to +/-127.
  Width <=20 centers it. The right byte starts at 127; left is 255-right. The
  extreme positive byte skips 254 to 255. Panning/volume each truncate signed
  PCM. At native MasterVolume=100/WavVolume=80, channel volume is 102/128.
- PlaySnd offsets use common-game pixels. SndPan converts its offsets from
  character-local space. This is why an absolute PlaySnd offset of 120 is inside
  the center zone at common width 1280, while SndPan 120 is audible to the right.
  Relative offsets sample the player position at controller time; complete
  facing/configuration edge cases remain open.
- Both pan and abspan are accepted. Pan supplies the numeric value if present;
  abspan's presence still selects absolute mode. SndPan with neither defaults
  to relative zero, despite the manual labeling an offset required.
- Deprecated volume is parsed and evaluated (including assignments and bottom)
  but its result does not change gain. Volumescale is applied; negative values
  mute, while 200 can boost above WavVolume before the effective gain clamps.
- freqmul .5/1.5/2 do not change pitch in these beta captures. The 1.0 character
  version also ignores 1.5/2. Nonpositive values warn but still play a new sample
  at its source pitch. The parameter expression still executes.
- Leading F/S are consumed as resource prefixes before parsing. Facing+3 is
  rejected as acing+3; unparenthesized FVar becomes common Var. Parentheses
  protect the original expression. Name validation is currently opt-in for the
  sound schema; full trigger/arity validation elsewhere remains incomplete.

The supplied binary's read-only call paths explain the panning arithmetic:
0x444bf0 converts the pixel offset, 0x46ec50 creates the SDL panning byte, and
0x46eb80 sets integer channel volume. Mix_SetPanning imports through IAT
0x4de434. These addresses apply only to executable SHA-256
d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e.
Disassembly is disposable under temp/audio-reference/, not source or a modified
executable. Runtime PCM observations remain the conformance evidence.

Open: device/production cues, channel-0 interruption on hit versus guard,
complete helper/entity/custom-state sound ownership and cleanup, capacity
fallbacks, arbitrary source-rate/libresample fidelity, extreme/bottom parameter
fallbacks, stereo/reverse/disabled/config edges, BGM and storyboard audio. Queue
capacity is a diagnosed provisional 16384 undrained commands; the CPU renderer
uses linear sample-rate interpolation and a bounded voice pool. None of these
foundations promote a whole controller/asset row to Verified.
