# Compatibility scope

The target is Elecbyte M.U.G.E.N 1.1 beta 1. The single current completion
checklist, implementation gaps and checked scenarios are in
[PROGRESS.md](PROGRESS.md).

All documented controller families and trigger routes are present. The engine
supports authored states, combat/recovery, helpers/projectiles/bindings, effects,
rounds/rematches, teams, campaign modes, input configuration and supported audio
and asset formats. Route presence and scoped acceptance do not establish full
behavioral or arbitrary-content equivalence.

Compatibility work includes legacy parser spellings, first-field precedence,
mixed resolution/coordinate systems, palette ownership, storyboard scaling and
artwork-driven motifs without menu fonts. High-resolution screenpacks may use
nonstandard portrait sizes, sparse HUD components and explicit text colors;
consume their authored definitions rather than substituting layout assumptions.

Some older character scripts omit a later-round initialization exit. If the same
stall occurs in the target runtime, it is a content defect rather than a reason
to add an engine timeout. Missing optional media and unavailable font payloads
must be distinguished from parser/renderer failures. A bundled executable with
a familiar version banner does not establish stock-runtime behavior.

Broader font/media/content acceptance, native timing/render/audio/AI fidelity
and physical controller acceptance remain partial. Run connected scenarios after
related implementation work; do not create a parameter matrix for every unknown
permutation or claim whole-engine compatibility from a roster load or screenshot.
