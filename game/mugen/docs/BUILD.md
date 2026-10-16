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
