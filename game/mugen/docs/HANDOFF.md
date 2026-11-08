# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Local checkpoint commits are authorized without individual approval. Recap about
every five commits; ask only for needed input. Never track user configuration.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

Current known-difference batch (user requested closure before compatibility):

- Camera return is corrected. The clipped region controls zoom fitting; the
  unclipped actors' midpoint controls centering before final stage limits.
  One bounded beta trace isolates the mixed offscreen-return tick. All eleven
  connected camera/zoom samples match, including time120 X80.6977 vs the prior
  Go80.8480. Native scratch was inspected/removed. Batch segment checks pending.
- Next close the documented sprite/shadow RGB, finite/overlap alpha and KO-echo
  playback differences. Unverified binding/pause/AI/content combinations belong
  to the later compatibility batch; do not invent parameter matrices for them.
- Prior405ef3951 fixes legacy font comments/diagnostics, PCX row decoding and
  V1 archive budgets. Linux font banks and connected assets pass; beta authored
  glyphs remained blank, so native font compatibility is still unverified.
- Priora51b2146b retains mapping-specific texture revisions, shared fighter/HUD
  pixels and AfterImage interpolation/outfit snapshots. Priorca131d22c retains
  byte-premultiplied RGBA uploads, filtering and shared/indexed sprite resources.

Prior 764918190 retains bounded WAV/SFX playback, extensible PCM/float support,
Linux KO/rematch/KO and completed decoder/mixer/media race checks.

Prior51f7ae0b3 bounds AIR collision allocation and adds the camera motion
sequence now extended through the corrected return.

Previous 88428c4d9 batch provides ZIP character selection/rosters/quick matches,
selective portraits, CMD [Remap] for human/helper/AI, pack retirement and live
Game Speed. The user has approved continuing without individual commit review.

Ledger stays 32/34 checklist items, not native conformance or effort. Remaining:
other pause/hitpause and bindings, crop/raster/color/
shadow/alpha, KO echo, native AI policy, strict asset compatibility, physical CD,
legacy-font equivalence and absent licensed fonts. Linux is the target; other
platforms are deferred.
Historical matrices/raw renderer gates are not mandatory acceptance.

Local mugen.yaml remains ignored/untracked by 8eda46f84, with contents preserved.
The user will notify the contributor to back up YAML before pulling that removal.
Standalone music-file playback CLI remains deferred; in-stage music works.
Both upstream histories retain Go 1.26/Ebitengine 2.10.6/purego 0.11.3; root module
files stay read-only. Original go.sum backup remains at
`temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked`.
