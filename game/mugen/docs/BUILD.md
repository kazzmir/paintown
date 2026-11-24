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

On Linux amd64/arm64, the engine dynamically loads `libfreetype.so.6` for
compatible TrueType metrics and rasterization. FreeType development headers and
additional Go modules are unnecessary. Ordinary TrueType faces use automatic
hinting, matching the reference's legacy path; solid fonts use real monochrome
rendering. Bitmap bearings preserve placement when modern rasterizers trim
empty borders. Font bytes stay pinned while borrowed by native faces, whose
resources are released with the owning Go face.

If the library/backend is unavailable, the existing portable Go outline renderer
remains usable. A contextual diagnostic reports that fallback; `Font.Rasterizer`
and `RasterizerFallback` expose it to callers separately from font substitution.
That path does not establish native metric/raster precision.

TrueType text still measures the complete surface, including negative bearings
and italic overhangs. Centering truncates after alignment; right anchors include
the last surface pixel. Bitmap font placement is unchanged. The shared native/Go
scene now has identical text extents and placement. Small rasterizer-version
edge differences were explicitly accepted by the user under U5. That acceptance
applies to the demonstrated Linux backend behavior; the portable outline fallback
and other-platform precision retain their separate limits. See the
[FreeType load and rendering API](https://freetype.org/freetype2/docs/reference/ft2-glyph_retrieval.html)
and [reference-era SDL_ttf implementation](https://github.com/libsdl-org/SDL_ttf/blob/release-2.0.9/SDL_ttf.c)
for the backend contracts.

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
