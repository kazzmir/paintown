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

Font checkpoint a8ab82ca0 uses the shared resource resolver for nested Windows
separators, mixed-case components and absolute TrueType paths. Present corrupt
or ambiguous paths fail with context; absent fonts retain the reported fallback.
Invalid bank values share fallback glyph textures; TrueType banks share one set.
The connected storyboard flow now renders local UTF-8 faces across scenes and
checks corrupt-font rejection. Only match-flow reopened and passed with race
detection. Scratch is automatic; no new asset banks or user configuration files.

Storyboard checkpoint f660d1640 preserves canvas pixels across ticks/scenes when
clearcolor=-1; an explicit clear replaces them. Colored fades use correct
premultiplied RGB and a reusable one-pixel texture instead of a full-canvas
allocation per draw. The connected scene lifecycle covers fade, animation/text/
sound, retained blank ticks, clear replacement and exit. Match-flow passes with
race detection; completed other segments were not rerun.

Current legacy sprite checkpoint honors the SFF shared-palette flag when decoding
PCX payloads without palette trailers. A pixel 12 at the usual trailer offset
must not discard 769 valid bytes. Masked/opaque views now share immutable decoded
indices, avoiding a second RLE decode and buffer. The existing ACT/helper/reset
asset flow covers the trailerless sprite, palette remapping and GPU drawing;
match-flow passes with race detection. No new fixtures or retained scratch.
Rebuild Linux at the end of this batch. Root modules and local YAML stay intact.

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
