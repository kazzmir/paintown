# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Local checkpoint commits are authorized without individual approval. Recap about
every five commits; ask only for needed input. Never track user configuration.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

065f5fe40 fixes finite TargetBind expiry during attacker hitpause: targets follow
the anchor's position with zero velocity, preventing premature victim drift.
The mixed-coordinate throw/recovery/reset scenario matches beta; four affected
segments passed with race detection and Linux was rebuilt. Earlier helper/Pause,
PosFreeze, camera and body-push observations remain covered.

Current font checkpoint uses the shared resource resolver for nested Windows
separators, mixed-case components and absolute TrueType paths. Present corrupt
or ambiguous paths fail with context; absent fonts retain the reported fallback.
Invalid bank values share fallback glyph textures; TrueType banks share one set.
The connected storyboard flow now renders local UTF-8 faces across scenes and
checks corrupt-font rejection. Only match-flow reopened and passed with race
detection. Scratch is automatic; no new asset banks or user configuration files.
Next concrete asset gap: storyboard clearcolor=-1 is parsed but its canvas is
unconditionally cleared while drawing. Preserve scene pixels and reuse fade
resources; validate through the same connected scene lifecycle. Rebuild Linux
at the end of this implementation batch. Root modules and local YAML stay intact.

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
