# Sound implementation and reference evidence

M4l provides the controller/mixer foundation. M4m connects the production device,
menu/character commands, match banks and HUD cues. M4n adds contact/guard cues and
channel-0 interruption. M4o adds root KO voices/echoes; complete KO lifecycle and fidelity remain open. `sound/` has a display-independent ordered command queue and a
thread-safe signed-16 stereo PCM reader. Character sound controllers now compile
typed tuples/parameters and emit commands; source banks remain immutable CPU
assets. Helpers/projectiles receive independent audio owners on the shared
queue, independently of gameplay entity/input IDs. Full entity
ownership and lifecycle behavior remains a separate obligation.

M4al adds two helperaudio controls for ordinary/recursive destruction. Root,
parent helper and descendant coexist on channel 1 at 441/882/1323 Hz. Their
numbered channels are independent. DestroySelf retires entity IDs but leaves
their sounds playing; root StopSnd channel 1 removes only the root tone. Go
already retained the measured voices, so this checkpoint adds conformance
evidence rather than a sound cleanup change. See RETIREMENT.md for the entity
side. Complete sound lifecycle/capacity/pause/round behavior remains open.

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

Open: complete KO/helper/revival/pause lifecycle, camera/zoom/mixed-coordinate contact panning,
complete helper/entity/custom-state sound ownership and cleanup, capacity
fallbacks, arbitrary source-rate/libresample fidelity, extreme/bottom parameter
fallbacks, stereo/reverse/disabled/config edges, BGM and storyboard audio. Queue
capacity is a diagnosed provisional 16384 undrained commands; the CPU renderer
uses linear sample-rate interpolation and a bounded voice pool. None of these
foundations promote a whole controller/asset row to Verified.

Production integration (M4m): one streaming Ebitengine player consumes the mixer.
The match queue is bound before entry to each root's own SND plus fight common
SND; helpers/projectiles inherit bank/queue bindings with independent owners.
HUD announcements and power-level cues use fight.snd. Ordered scene clears retire
voices/cache at round reset and fight exit. Native lifecycle parity is pending.
Live SFX volume changes do not restart active voices. Sound=0 creates no device
player and discards new Play commands. Missing/unsupported sample playback warns
without terminating the game; warning text is deduplicated/bounded.

The new hud-sound capture replaces only disposable fight announcement settings
and the bank reference. Its original one-second 441-Hz tone matches the Go HUD's
3998.8/3967.8 native amplitudes within eight signed-16 units. Tone expiry is also
checked. The recipe and modified fight hashes are pinned; absolute phase timing
is not compared. Replay: sound_probe.py --fixture hud-sound --name NEW --seconds 9.

Actual device smoke uses app_smoke.py --audio with PULSE_SERVER/PULSE_COOKIE and
ALSA_CONFIG_PATH=temp/audio-reference/alsa.conf. That private ALSA file directs
pcm.!default and ctl.!default to the private Unix server using the pulse plugin;
no system ALSA config is changed. The nonzero sound-match-playback capture and
owned-window images prove the real Go player path runs, without claiming native
waveform or hardware latency parity. The ordinary Go tests create no device;
they advance the exact shared mixer by reading PCM per tick.

Stock [Sound] settings now load, including SampleRate=44100, BufferSize=11,
WavChannels=32, MasterVolume=100 and WavVolume=80. BufferSize means 2^n frames.
YAML sound.enabled/master_volume/wav_volume/bgm_volume are presence-aware optional
overrides. BGM volume and resampling method/quality are retained for future work;
the SFX renderer still uses linear interpolation. Device configuration bounds
are explicit implementation limits, not measured native fallback rules.

Contact integration (M4n): hitsound/guardsound compile integer expressions before
legacy HitDef application. S chooses attacker SND; F/unprefixed chooses common.
A group alone defaults item zero. Both prefixes are consumed before expression
parsing, so raw Facing is rejected as acing. Unknown/malformed sound expressions
fail state compilation before execution. Other HitDef parameters are still a
separate incomplete schema. The shared source map is never rewritten.

Nine new audio programs (32 native segments, 45 Go tick checks) and five grammar
programs measure normal hit/guard/HitOverride, cue panning, bank choice, omission,
assignment and syntax. Initial omitted cues produce no sound; a new HitDef that
omits an earlier choice clears it in the measured scene. A hit interrupts only
the defender's numbered voice channel 0 before new get-hit controllers; guard
preserves it, including while entering MoveType H. An override to MoveType I still
interrupts channel 0. Fresh override/common-single audio replays pass with full
races, and real KFM combat now requires the common contact cue and audible PCM.

Contact sound panning uses defender screen X from the left edge, unlike ordinary
PlaySnd's center-relative position. Symmetric scenes at defender -70 and -40
produce effective 240p offsets 90 and 120. The beta's read-only paths at 0x43ed37
and 0x43f9c3 load its cached screen coordinate before calling 0x444b20. This
supports the measured behavior; complete camera/zoom/mixed-profile cases remain
open. Common-bank recipe hashes are pinned after patching only disposable fight
DEFs; original reference assets remain untouched. Optional --contact/--common
tone generation preserves the default generator's original bytes.

Root KO audio (M4o): sound 11,0 comes from the player's own bank and uses the
measured screen-X pan. The post-damage shared pass stops numbered channel 0 and
emits one initial voice. A KO counter begins at one on that pass; KO.echo schedules
50% and 25% voices at counter 20/50. The supplied read-only paths 0x43cfec and
0x43d035 compare those counter values; 0x4fd830/0x4fd8d8 contain .5/.25 scale
constants. These explain the measured gain, not a full native timing proof.
Any current root nokosnd assertion suppresses each root's due voice. Releasing
an initial-only assertion permits the later echoes, as the native manual says.

Four source-pinned complete-constant fixtures produce 21 native segments and
25 Go tick checks, including opponent/global suppression. The original quarter-
second KO tone permits independent gain/expiry checks. Capture analysis uses
100-ms windows/50-ms steps; selected Go windows are six ticks. Full races and a
fresh initial-suppression replay pass. Exact first-sample/tick boundaries,
Pause/SuperPause, helper KO behavior, complete kill flags, state transitions and
revival latching remain open. The current positive-life reset is provisional.

To isolate voices, --quiet-hud now routes disposable fight definitions to a valid
original empty SND bank and records its hash. Negative announcement IDs alone
left the spoken KO announcement in initial diagnostic captures; those are not
the promoted evidence. The reference copy and desktop are unchanged. The sound
harness also retains analysis-window settings and fixture-owned constants for
replay. Default tone-bank generation remains byte-identical; --ko adds group 11.


SuperPause cues (M4v) use automatic channels, own/common prefixes and the measured
screen-X pan. Three original tone programs produce seven native segments and
seven Go PCM checks: distinct 1323/2646-Hz banks, group-only common selection,
channel-0 voice preservation, finite expiry and silence. Final serial captures
are temp/audio-runs/superpause-sound-*-isolated; earlier *-native drafts may
contain overlapping Go HUD output and are diagnostic only. A fresh isolated
superpause-sound-replay matches. Existing eight-unit/.35-second tolerances apply.
The shared mixer continues streaming while simulation pauses. See SUPERPAUSE.md
for clock/combat/effect evidence and remaining schema/ownership limits.

## Helper channel ownership and retirement (M4al)

helperaudio-ordinary/recursive use original 441/882/1323 Hz tones for the root,
parent helper and nested descendant, all on channel 1. Parent DestroySelf at
Time=120 removes one or both helpers; root StopSnd channel 1 at Time=240 removes
only the 441 Hz voice. Native helper tones continue across both retirement
variants, including after their IDs vanish. Parent/child numbered channels are
independent of each other and the root. This is focused lifetime evidence;
capacity/stealing, pause, borrowed-state banks, round/exit and remaining helper
sound behavior still need their full controls.

The programs add 24 numeric observations and eight PCM windows (48 stereo tone
amplitudes). Tick 30 precedes destruction, 150 follows it, and 270/300 follow
root StopSnd. Native plateaus are approximately 4530.2/3436.3 per audible tone,
within the existing eight signed-16-unit tolerance. Both initial recordings and
fresh replays match. Closing silence caused by terminating the native process
is retained in raw evidence, but is not an in-game stop event or a Go tick check.
The numeric packet includes GameTime=430 at root Time=300. State-only PCM replay
starts its timeline at 130, the independently pinned Match control boundary.

The PCM runner now checks authored CMD hashes and the recorded language version,
and explicitly shares the timeline. All 29 listed programs/30 cases pass race
comparison in temp/m4al-pcm-first-race.log, including all older sound cases.
Production already keeps these voices; only a retirement comment and reference/
test coverage change. The last full scoped suite is M4ak's m4ak-full-race.log,
with the final renderer's additional m4ak-system-final-race.log.

Actual device/window evidence: temp/app-runs/helperaudio-recursive-go/ captures
3/5/7/9 seconds at 1280x960 on the private display/audio server. All four PNG
hash/dimension/nonblack and runtime-log checks pass. PCM has 446,321 stereo
frames, peak 11,307 and RMS 3836.95. Tone measurements match native plateaus to
the reported decimal: all three before root StopSnd, then only helper tones.
The S16 stream and measurements remain under temp; no desktop audio is recorded.
This verifies the production device path, not arbitrary waveform/latency parity.

Replay with the private PULSE_SERVER/PULSE_COOKIE and display environment:

```
python3 game/mugen/tests/sound_probe.py --fixture helperaudio-ordinary --name UNIQUE_NAME --seconds 9
python3 game/mugen/tests/sound_probe.py --fixture helperaudio-recursive --name UNIQUE_NAME_2 --seconds 9
```

Each JSON pins the quiet-HUD recipe, configuration, executable, CNS/AIR/CMD and
synthetic SND inputs. Recreate probe.snd with tests/generate_sound_bank.py; its
SHA-256 is 58b6beecae0ad5b26566d89fee1fc53ef1b0f0e9cc453b8f81899ee55d41b543.
The first rejected empty-state diagnostic is only in temp/reference-runs/;
the corrected recordings use helperaudio-*-native-final and replays use *-replay.
