# Compatibility references

The target is Elecbyte M.U.G.E.N 1.1 beta 1. Use its documentation and an
independently obtained reference runtime when resolving engine semantics.
Reference assets and executables are external prerequisites, not repository data.

A content pack may bundle a renamed, repackaged or modified runtime. A matching
version banner does not establish identical behavior. Findings from a bundled
runtime describe that pack until confirmed against the target engine. Keep
public notes about the behavior and compatibility pattern, without recording
local package names, executable fingerprints or private asset inventories.

Use small connected scenarios to distinguish engine behavior from content
assumptions. Examples include mixed coordinate systems, palette ownership,
legacy parser syntax, round initialization and motif layout. A script defect
that also reproduces in the reference runtime is a content issue; preserve the
engine contract rather than adding a pack-specific workaround.

Validation statements apply only to their stated scenario. Loading a roster,
observing a fight or matching a rendering sample does not establish arbitrary
content or complete native equivalence. The current completion boundary is the
single [progress ledger](PROGRESS.md).

Keep temporary comparison output out of Git and clean it after review. Public
notes should describe the finding, its implementation and the remaining limits;
they should not depend on a contributor having another person's local files.
