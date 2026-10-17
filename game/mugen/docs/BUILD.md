# Build and font dependencies

Use `game/mugen/mugen --check` or `game/mugen/mugen --game`. The launcher uses
the repository module with alternate metadata at `game/mugen/mugen.mod` and
`mugen.sum`, via Go's `-modfile` option. Both metadata files remain inside the
approved project boundary; root `go.mod` and `go.sum` are unchanged. This is not
a separate nested module. When running Go directly from the repository root,
include `-modfile=game/mugen/mugen.mod -mod=readonly`.

The overlay preserves the existing dependency versions and adds x/image v0.31.0
for TrueType/OpenType/TTC parsing and CPU rasterization, with x/text v0.29.0 for
its character maps. These versions were already cached/transitive dependencies
of Ebitengine. No dependency download or root module change was required.
Future repository dependency updates must be reconciled into this scoped overlay.

The launcher keeps build caches and disposable files under `temp/`. Font and
Character packages still import Ebitengine and need DISPLAY even for CPU tests;
the engine/evaluator/SFF/parser core is display independent.

The growing native fixture corpus takes about nine minutes for a full Match
race run. Use `game/mugen/mugen --check -race -timeout=20m` for full race
validation so Go's default ten-minute package deadline does not become a false
failure as coverage grows.

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
DISPLAY=:97 XAUTHORITY=/home/juvinious/projects/paintown/game/mugen/temp/virtual-display/Xauthority LIBGL_ALWAYS_SOFTWARE=1 GOPROXY=off game/mugen/mugen --check -race
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
