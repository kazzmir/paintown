# Tests and reference fixtures

From any working directory, run `game/mugen/mugen --check` using the launcher's
absolute path if needed. It keeps build/test temporary files under
`game/mugen/temp/` and prevents changes to the root Go module.

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

The original reference is never executed in place. Reference experiments use
copies under `temp/reference-runs/` and a Wine prefix under `temp/wine-reference`.
Reference-generated files, extracted images and captures stay under `temp/`.

Run headless language fixtures with:

```
game/mugen/mugen --test expressions --input game/mugen/tests/testdata/expressions.jsonl
```

Each JSONL fixture has a name, initial state time, and ordered expression steps
with expected kind/value and optional variable assertions. The runner emits
deterministic observations (including warnings and variable changes) and exits
nonzero on malformed fixtures or mismatches. These use the production typed
evaluator; world-bound trigger semantics require the forthcoming engine runner.
