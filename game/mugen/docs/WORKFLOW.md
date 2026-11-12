# Implementation and validation workflow

Track current completion once in [PROGRESS.md](PROGRESS.md). The feature catalog
maps implemented consumers and remaining work; it is not a test-data inventory.
Implement a coherent feature batch, validate its affected connected scenarios,
record completion and commit locally for reversal. Recap substantial progress
about every ten to fifteen commits, or sooner when input is needed.

The five segments are state-flow, combat, entities, effects-pause and match-flow.
Completed segments stay checked until covered source changes or a specific
regression reopens them. Source fingerprints are derived from public repository
files and preserve this skip mechanism. They do not identify external assets.
Run acceptance after a coherent batch rather than repeating every legacy test
for each parameter. Extend an existing state path when coverage is needed.

Use external reference data in place, a shared build cache and disposable
scratch. Clean scratch after success and failure. Do not add copied content,
executables, captures, recordings, parameter matrices or per-run diaries to Git.
Optional historical diagnostics may require separately supplied inputs; they
are not completion gates for unrelated work.

Keep contributor documentation portable. Record compatibility patterns and
implementation limits rather than local pack names, asset filenames, binary
fingerprints, machine paths, private server details or exact run inventories.
Use placeholders for user-provided content and configuration examples. Retain
repository-relative source/tool references and source-based coverage metadata.

Content packs can use a runtime that differs from the target engine. A matching
version banner is not sufficient evidence of equivalence. Separate observations
about a pack from verified target behavior. A script defect reproduced in the
reference runtime is a content finding; do not repair further third-party
scripts or introduce engine workarounds for those defects.
