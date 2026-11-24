# Build and runtime dependencies

Run commands from the repository root. The launcher uses the repository module
with the scoped [mugen.mod](../mugen.mod) and [mugen.sum](../mugen.sum) metadata.
Direct Go commands should include `-modfile=game/mugen/mugen.mod -mod=readonly`.
The overlay is not a separate nested module; dependency changes must keep it
consistent with the repository module while ordinary builds leave root metadata
unchanged.

Use Go 1.26 or newer. See the [README](../README.md) for Linux libraries, build
commands, controls and external data layout. Build tools, game content and user
configuration are supplied separately; no contributor-specific environment is
assumed.

Normal validation is `game/mugen/mugen.sh --check`, which selects pending or
reopened connected segments. Use `--acceptance` for an explicit match-flow run.
Core parser/evaluator/asset packages can run without a display, while packages
that import Ebitengine need a working graphical environment.

For a headless Linux environment with Xvfb installed, a portable example is:

```bash
xvfb-run -a -s '-screen 0 1280x960x24' game/mugen/mugen.sh --check -race
```

Create any isolated audio/display services in your own environment and stop
only services you created. Audio recording requires an explicitly isolated
output; never use a microphone or an unrelated desktop monitor. See each
optional probe's `--help` for its required inputs rather than relying on saved
machine-specific server names or authentication paths.

## Fonts

Bitmap fonts load their SFF relative to the font definition. TrueType lookup
starts beside the definition, then checks `MUGEN_FONT_DIRS` using the platform
path-list separator and standard font locations. Windows separators and
case-insensitive matching apply to nested resource names. Absolute references
are honored when explicitly authored. Existing corrupt or ambiguous resources
remain errors.

When a requested TrueType file is unavailable, the engine reports a substitution
to embedded Go Regular through `Font.Fallback`. This does not reproduce another
font's glyph coverage or metrics. Supply an appropriately licensed font for
content requiring it; the fallback does not provide complete CJK coverage.
No proprietary font payloads are included in the repository.

TrueType text measures the whole surface, including negative bearings and
italic overhangs. Centering truncates after alignment; right anchors include the
last surface pixel. Bitmap fonts retain their own placement rules. The current
Go rasterizer does not execute TrueType glyph hint instructions, so native
glyph edges, baselines and hinted advances can still differ. U5 tracks that
precision gap separately from supported font loading and text layout.

## AI move models

Characters may seed live learning with compatible beta `[Files] ai` data.
Missing or incompatible data reports a contextual warning and uses an empty
model. The cache format stores game-pixel estimates without a resolution tag;
use data authored for the selected logical game width.

Both human and CPU command episodes teach bounded ground/air banks. Completed
estimates survive rounds, rematches and reloads in the running application;
definition/source revisions, selected language and logical game width isolate
profiles. Live actors keep independent banks. Session storage is bounded to 64
profiles, each with at most 80 ground and 40 air moves. No generated binary
cache is written to the content directory. See the README for AI.Cheat and
the independent-stream/timed-input port policies.

## Audio and acceptance

WAV/MP3/Ogg playback uses Go decoders. Linux MIDI/tracker playback optionally
loads `libfluidsynth.so.3` and `libopenmpt.so.0` at runtime. MIDI also needs a
General MIDI SoundFont, selected with `MUGEN_SOUNDFONT` when necessary. Physical
CD playback and runtime acceptance on other platforms remain outside the
current verified scope.

The application smoke runner can use a standalone build and external content.
It reuses assets in place through an overlay and removes scratch on success or
failure. Keep only output explicitly needed for a short review, then remove it.
Generated assets, recordings, captures, user configuration and binaries do not
belong in Git.
