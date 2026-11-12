# Tests

Start with [SEGMENTS.md](SEGMENTS.md). Existing connected scenarios are consolidated
into five suites: state-flow, combat, entities, effects-pause and match-flow.
Their completion is recorded once in [project progress](../docs/PROGRESS.md).

```bash
# See current completed/pending suites without running tests.
game/mugen/mugen.sh --check --list

# Run only pending/reopened suites; completed suites are excluded.
game/mugen/mugen.sh --check

# Explicit end-state acceptance after an implementation batch.
game/mugen/mugen.sh --acceptance
```

Supply the external reference content in the fixture layout expected by the
stock test helpers. Segment runs read it in place, reuse the build cache and
automatically delete their own scratch output.
A display is required by Ebitengine-dependent packages. No Wine/native runs,
asset copies, capture matrix or new generated data are part of these suites.

The old low-level tests, probes and tracked numerical fixtures remain available
for diagnosing a specific failure. They are excluded from normal segment runs.
--check-all explicitly requests that entire historical suite. Optional image diagnostics may require separately supplied capture inputs;
they are not required for routine segment validation.

Do not commit generated fixtures, screenshots, recordings, copied distributions
or binaries. Do not add an individual test/doc for each parameter. Extend a
connected scenario when its feature actually needs new coverage.
