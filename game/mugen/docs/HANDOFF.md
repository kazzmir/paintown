# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Local checkpoint commits are authorized without individual approval. Recap about
every five commits; ask only for needed input. Never track user configuration.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

This five-commit batch includes:
- 065f5fe40: finite TargetBind expiry during attacker hitpause transfers zero
  velocity, preventing victim drift; connected throw/recovery/reset matches beta.
- a8ab82ca0: nested Windows/mixed-case and absolute TrueType paths resolve;
  fallback banks share glyph textures and corrupt files retain error context.
- f660d1640: uncleared storyboard scenes retain pixels; colored fades use correct
  premultiplication and reuse one texel.
- b38e0535e: trailerless shared-palette PCX sprites no longer truncate at false
  palette markers; masked/opaque views share decoded indices.
- Current checkpoint: retained layers compose once per simulation tick, so extra
  presentation refreshes cannot accumulate alpha. README and guidance refreshed.

Affected connected segments passed with race detection. The last asset changes
reopened only match-flow; other completed segments stayed checked. Linux rebuilt.
Owned-window KFM/KFM720 keyboard acceptance reached KO/result, rematch and a
second KO/result: 82 balanced events, two inspected captures, no runtime/audio
errors. Scratch overlay, captures and logs removed. Root module hashes and local
YAML contents remain unchanged. No binaries, assets or user configuration tracked.

Ledger remains 32/34 checklist items, not native conformance or effort. Remaining:
camera easing, other pause/hitpause and binding combinations, crop/raster/
color/shadow/alpha, KO echo, AI policy, strict asset compatibility, physical CD
and absent licensed fonts. Linux target; other platforms deferred. Projectile
contacts intentionally do not recoil the owner. Historical matrices and old
raw renderer gates are not required acceptance.

Local mugen.yaml remains ignored/untracked by 8eda46f84, with contents preserved.
The user will notify the contributor to back up YAML before pulling that removal.
Standalone music-file playback CLI remains deferred; in-stage music works.
Both upstream histories retain Go 1.26/Ebitengine 2.10.6/purego 0.11.3; root module
files stay read-only. Original go.sum backup remains at
`temp/merge-backups/go-rewrite-172d3fb02-yb0ldpwe/go.sum.untracked`.
