# Implementation and validation workflow

The goal is a working Go MUGEN 1.1 beta engine. Progress is recorded once in
[PROGRESS.md](PROGRESS.md), with checkmarks for completed work and concrete next
actions for partial or missing work. The small JSON catalog behind that page is
implementation metadata, not a test-data bank.

Implement a connected feature batch using the existing evidence and beta docs.
Update the feature status after connecting its actual production consumer.
Validate the affected state-machine segment at the end of the batch and record
its completion. Commit locally for reversal. Do not stop for checkpoint approval
unless user input is actually needed.

Existing tests are consolidated into five scenario suites: state-flow, combat,
entities, effects-pause and match-flow. Completed suites are excluded from normal
checks. A covered source change or a specific regression reopens only the related
suite. A final playable acceptance traverses selection, input-driven combat,
round results and rematch. Use focused diagnostics only to resolve real failures.

Do not expand parameter matrices or run every legacy test after each edit. A
parameter permutation without evidence of a functional defect is not a separate
project task. Controller routing and exact image comparisons are useful facts,
but they are not a measure of project completion.

Use stock assets in place, one shared build cache, and temporary per-run scratch
that is removed on success and failure. No generated test data or binaries go in
Git. Keep a short failure note instead of complete copied distributions and logs.
The 2026-11-06 cleanup removed nine generated run banks and recovered126.58 GiB;
canonical assets and working tools remain. Removed raw comparisons are historical
evidence, not something to regenerate for routine development.

The former matrices and repeated checkpoint transcripts are historical. Their
instructions do not override this workflow. The full legacy suite is available
only through explicit --check-all; artifact-dependent image checks can no longer
run after cleanup. Small checked-in regression fixtures remain unchanged.
