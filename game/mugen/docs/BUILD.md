# Build and font dependencies

Use `game/mugen/mugen.sh --check` or `game/mugen/mugen.sh --game`. The launcher uses
the repository module with alternate metadata at `game/mugen/mugen.mod` and
`mugen.sum`, via Go's `-modfile` option. Both metadata files remain inside the
approved project boundary; the launcher keeps root module metadata read-only. This is not
a separate nested module. When running Go directly from the repository root,
include `-modfile=game/mugen/mugen.mod -mod=readonly`.

The user-requested integration of `origin/go-rewrite` at `172d3fb02` and the
previously pushed dependency commit `dc3423cf1` raises the minimum
to Go1.26 and reconciles both module graphs to Ebitengine2.10.6, purego0.11.3,
x/image0.47.0 and x/text0.43.0. MUGEN's YAML, MP3 and Vorbis dependencies are
retained in the merged root metadata and scoped overlay. Root metadata is
read-only during normal builds; requested upstream merges can update it.
Future repository dependency updates must also be reconciled into the overlay.

On this verification host Go1.26.9 is already cached. Prefix the private wrapper's
command with `env GOTOOLCHAIN=go1.26.9` to select it without downloading Go1.26.0.
Normal installations can use any Go1.26-or-newer toolchain. All five connected
segments, incoming Paintown/parser/simulator tests, both Linux builds and a
stock MUGEN owned-window startup passed on the reconciled graph.

The launcher keeps build caches and disposable files under `temp/`. Font and
Character packages still import Ebitengine and need DISPLAY even for CPU tests;
the engine/evaluator/SFF/parser core is display independent.

Normal validation is `mugen.sh --check`: only reopened connected segments run.
Completed segments are not repeated. Use `--acceptance` for an explicitly
requested match-flow rerun. Old native matrices and artifact-dependent pixel
diagnostics are optional historical tools, not completion gates.

Linux legacy MIDI/tracker playback dynamically loads optional system
`libfluidsynth.so.3` / `libopenmpt.so.0`; no development headers or copied
libraries are needed. MIDI also needs a GM SoundFont, optionally selected by
`MUGEN_SOUNDFONT`. WAV/MP3/Ogg use existing Go decoders. Physical CD playback is
unavailable, and other platform validation is deferred by the user.

## Font lookup and fallback

FNT v2 loads its bitmap SFF or TrueType file relative to the definition. For
TrueType, the search then checks directories in `MUGEN_FONT_DIRS` (the platform
path-list separator), Windows `%WINDIR%/Fonts` when available, and Linux's common
`/usr/share/fonts/truetype/msttcorefonts` directory. Filename matching in each
font directory is case insensitive. Corrupt existing files fail; missing files
use x/image's embedded Go Regular. `Font.Fallback` records the substitution and
motif loading reports it through the logger.

This gives a deterministic portable fallback for the stock Arial/MS Gothic
definitions, whose actual Windows font payloads are absent from the archive.
Go Regular does not reproduce their glyphs/metrics or provide full CJK coverage.
Place the requested licensed font in a configured font directory for accurate
coverage of its supported characters. Exact Windows rasterizer parity and
reference text placement have not been demonstrated. No proprietary fonts are
copied into this repository.

Motif fonts are searched in the order documented by the supplied system.def:
its own directory, the program root, data/, then font/. Bitmap font SFF children
resolve relative to their font DEF, honoring Windows separators and case. Declared
missing/malformed motif fonts now fail loading. A missing Windows TTF referenced
inside a valid font definition still uses the observable fallback above.

## Private verification display

During M4h the desktop X server :0 stayed reachable but xrandr reported every
output disconnected. Ebitengine then failed initializeGLFW with "no monitor was
found" and native probes stalled in fade-in. Do not alter the user's monitor or
focus configuration to work around this.

A temporary Xvfb archive was downloaded with apt-get download into
`temp/virtual-display/` and extracted with dpkg-deb -x into its root/ directory.
No system package was installed. Package: Ubuntu noble-updates xvfb,
2:21.1.12-1ubuntu1.8 amd64. Archive SHA256:
`712429d9342c7d6cef63afb33ccfcd2bd477a69371b5c9941a9c6dde31a4cd3c`.
Existing host libraries satisfy its dependencies. A mode-0600 Xauthority file
contains a private random cookie; never print or commit it. The server is local
only, started with:

```
game/mugen/temp/virtual-display/root/usr/bin/Xvfb :97 -screen 0 1280x960x24 -nolisten tcp -noreset -auth game/mugen/temp/virtual-display/Xauthority -fbdir game/mugen/temp/virtual-display
```

While that task-owned server runs, checks and probes use:

```
DISPLAY=:97 XAUTHORITY=/home/juvinious/projects/paintown/game/mugen/temp/virtual-display/Xauthority LIBGL_ALWAYS_SOFTWARE=1 GOPROXY=off game/mugen/mugen.sh --check -race
```

The same environment can prefix reference_probe.py or app_smoke.py. Captures
still target only their spawned process's visible window. Server/auth/framebuffer
and downloaded binaries are ignored temporary artifacts. Check that a display
number is free before creating a new server and stop only the server created by
this task when verification work ends. Fully display-independent Character /
Match extraction remains open; the virtual server is a verification adapter.

For actual audio smoke, the task-owned Pulse server is described in AUDIO.md.
ALSA_CONFIG_PATH must point to temp/audio-reference/alsa.conf, with task-local
pcm.!default and ctl.!default pulse definitions whose server is the private Unix
socket. Set PULSE_SERVER and PULSE_COOKIE to the private paths as well. This
prevents Oto/ALSA's default PCM selection from reaching a desktop audio route.
No package installation or system/user audio configuration change is required.
`tests/app_smoke.py --audio` requires explicit private paths and records only the
private mugen_reference.monitor. The server uses installed PulseAudio 16.1 and
Wine's existing 32-bit pulse driver; all runtime/cookie/state files stay in temp.

Private validation uses `temp/_tmp` for GOTMPDIR. Go ignores underscore-prefixed
directories during recursive package discovery, so interrupted CGO build files
cannot become extra packages. Disposable source snapshots should still use a
non-Go suffix. The connected runner selects the affected packages explicitly.

## Standalone Linux acceptance

Build once using the README command, then pass
`--binary game/mugen/temp/bin/mugen-go` to `tests/app_smoke.py`. The smoke tool
creates a symlink overlay over the supplied resources and copies only its edited
configuration text. It deletes scratch at exit on success/failure by default;
`--keep-artifacts` is an explicit short-lived inspection override. Inspect and
remove that directory before finishing a checkpoint. One executable in
`temp/bin/` is reused; per-run asset/binary copies are no longer created.
