# Coverage

[PROGRESS.md](PROGRESS.md) is the single current project completion ledger.
It contains the full breakdown, completed scenario checkmarks, partial/missing
mechanics and controller/trigger gaps. Update that page through the coverage tool;
do not maintain a parallel list here.

```bash
game/mugen/mugen.sh --coverage
game/mugen/mugen.sh --coverage --gaps
game/mugen/mugen.sh --coverage --write
```

Source routes are checked without building or loading test assets. Scenario
completion is retained until related source changes. See [test segments](../tests/SEGMENTS.md)
and [workflow](WORKFLOW.md). Historical per-checkpoint measurements are in Git
history and optional topic research notes; they do not define current progress.
