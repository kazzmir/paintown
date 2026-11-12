# Sound implementation

The sound subsystem has an ordered command queue over immutable source banks
and a thread-safe signed-16 stereo PCM mixer. One streaming device consumes the
production mixer. Root fighters, helpers and projectiles have independent audio
owners; equal channel numbers on different owners can coexist.

## Ownership and lifecycle

A new numbered voice replaces that owner's existing channel unless lowpriority
protects it. Automatic voices can coexist. StopSnd with channel -1 stops the
applicable voices. Destroying a helper retires its entity identity while its
existing sounds can continue; another owner's same-numbered channel is separate.

Match entry binds character and common SND banks before controllers run. HUD
announcements and power-level cues use fight resources. Scene cleanup handles
round reset and fight exit. Live volume changes do not restart active voices.
Disabling sound avoids the device player and discards new playback commands.
Unavailable sample playback reports a bounded warning instead of ending a match.

## Parameter behavior

Panning uses an integer offset in common game units and the configured panning
width. The beta mapping has a central dead zone and bounded channel gains;
ordinary PlaySnd and controller SndPan offsets retain their distinct coordinate
conversions. Relative offsets sample the actor position when the controller runs.

Both pan and abspan are admitted. An explicit pan supplies the numeric value,
while abspan presence chooses absolute mode. SndPan with neither uses relative
zero. Deprecated volume still parses and evaluates but does not adjust gain;
volumescale controls playback gain. Nonpositive frequency multipliers warn while
preserving the established beta playback behavior. Source-rate/device fidelity
remains part of the native compatibility scope.

Resource prefixes are consumed before expression parsing. Parenthesize an
expression whose identifier would otherwise be mistaken for a prefix. Keep this
legacy sound rule separate from the admission policy of other controllers.
Malformed authored sound expressions retain state/source diagnostics.

## Combat cues

Hit and guard sound choices are compiled before contact application. Character
and common banks remain separate and are not rewritten by cue selection.
Omitting a new HitDef cue clears the earlier choice. An ordinary hit interrupts
the defender's numbered channel zero before get-hit controllers; guarded contact
preserves it. Fatal guard contacts follow their established fatal-hit cue policy.

KO voices and ordinary echoes are integrated with match progression. Scoped cue
checks distinguish channel interruption, relative onset and expiry; they do not
establish sample-exact device synchronization or complete waveform parity.
See [fatal guard cues](FATAL-GUARD-CUES.md) and the [progress ledger](PROGRESS.md)
for the current implementation boundary.

## Music and verification

Background music follows active menu, fight and storyboard states. NoMusic can
suspend playback without losing its cursor. Supported format dependencies and
font/audio environment setup are described in [BUILD.md](BUILD.md) and the
[project README](../README.md).

Use isolated generated tones when diagnosing ownership, gain or cue timing.
Audio recording must target an explicitly isolated output and must not capture a
microphone or unrelated desktop audio. Keep recordings, environment setup and
external asset identities out of the documentation and repository. Extend a
connected scene/combat lifecycle instead of creating a parameter matrix.
