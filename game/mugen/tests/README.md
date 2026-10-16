# Tests and reference fixtures

From any working directory, run `game/mugen/mugen --check` using the launcher's
absolute path if needed. It keeps build/test temporary files under
`game/mugen/temp/` and prevents changes to the root Go module.
It uses scoped `mugen.mod`/`mugen.sum`; direct Go commands need
`-modfile=game/mugen/mugen.mod -mod=readonly`. See [build notes](../docs/BUILD.md).

Required reference content is the supplied, unmodified M.U.G.E.N 1.1 beta 1
distribution at `game/mugen/temp/mugen-1.1b1/`. Set `MUGEN_REFERENCE_DIR` to use
another extracted copy. Missing files and mismatched hashes fail rather than
skip; `testdata/reference-manifest.json` pins 109 content/spec/tool files without
committing the distribution itself. No test reads the old `data-new/` tree.

The `physics_check`, `character`, `stage`, `air_player`, `snd_player`, and
`fetch-all-data` directories are executable diagnostic applications. Launch one
with `game/mugen/mugen --test NAME`; pass its `--data-dir` explicitly when needed.
Their output is not a substitute for semantic assertions.

Pure parser/evaluator tests can run without DISPLAY. Packages importing
Ebitengine currently require a display even when their tests do not open a
window; the simulation extraction in M2 will remove that coupling from state
tests. Do not describe the current entire test tree as headless.

The original engine is never executed in place. Engine experiments use
copies under `temp/reference-runs/` and a Wine prefix under `temp/wine-reference`.
Reference-generated files, extracted images and captures stay under `temp/`.
Tiny original reference-compressed fixtures and numerical pixel hashes may be
committed; Elecbyte art remains untracked.

Run headless language fixtures with:

```
game/mugen/mugen --test expressions --input game/mugen/tests/testdata/expressions.jsonl
```

Each JSONL fixture has a name, initial state time, and ordered expression steps
with expected kind/value and optional variable assertions. The runner emits
deterministic observations (including warnings and variable changes) and exits
nonzero on malformed fixtures or mismatches. These use the production typed
evaluator; world-bound trigger semantics require the forthcoming engine runner.

Run the authored reference animation probe with
`python3 game/mugen/tests/reference_probe.py --name UNIQUE_NAME`. It requires
Wine, DISPLAY, xdotool, ImageMagick `import` and Pillow; it copies the supplied
distribution and uses a private Wine prefix under `temp/`. It captures only the
spawned process's window, sends no input, and makes no focus/pointer changes.
Each capture reads two clipboard lines by exact matching against the bundled
debug font, rejects ambiguous/missing glyphs, and checks the pinned observations.
Captures/logs/hashes remain in its run directory. Each run name must be new.
The Go clock regressions use the committed numerical observations without Wine.

Named state probes use `--fixture state-order --pages 0` or
`--fixture persistence --pages 0 12`, each with a new `--name`. `--record` saves
new observations only in that disposable run directory; it does not change the
committed expected data. A fixture may supply its own neutral CMD state program.
Engine and Character tests run both programs against the pinned variables.

Float-valued probes select `--bank fvar`. `constants-a --bank fvar --pages 0 12 24`
and `constants-b --bank fvar --pages 0 12 24 28` cover the documented constant
catalogue; the one reference-rejected name is an explicit unavailable slot.
The exact glyph reader accepts signed decimals/exponents, preserving nine-digit
float32 round trips. Kind assertions follow the documentation's declarations.

Build the optional error reader with `game/mugen/mugen --reference-tools`.
It reads only the reference MUGEN error dialog inside the private Wine prefix,
without input/clipboard/focus operations. The runner uses it to record and check
expected load rejections. For example, `--fixture constant-unavailable --pages 0
--mugenversion 1.1` checks the pinned invalid-constant error. `--bare-constants`
is an omission/load-error experiment using empty constant groups; the supplied
engine rejects incomplete Data groups. It is not a source of valid fallback data.
