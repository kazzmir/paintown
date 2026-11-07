# MUGEN implementation instructions

## User-directed workflow — 2026-11-06

The user replaced exhaustive native matrices and repeated small-test batteries
with implementation-led progress and connected state-machine scenarios. These
instructions supersede old research plans and checkpoint test requirements.

- Use docs/PROGRESS.md as the SINGLE project completion ledger: the full project
  breakdown, checkmarks, partial/missing features and completed scenario coverage.
- Implement substantial, coherent mechanics batches from unchecked items. Use
  existing code, evidence and the supplied beta documentation. Do not create
  native parameter matrices, new per-piece documents or asset-copy experiments.
- Tests cover segments spanning entry/order/transitions, combat/recovery,
  helper/binding/retirement, effects/pause, or complete match flow. Consolidate
  existing scenarios through tests/segments.py. Do not add a test per parameter.
- Completed segments stay checked and out of default runs. Reopen only when
  their covered production source changes, a regression identifies that segment,
  or the user explicitly requests it. Run affected segments/end-state acceptance
  after a coherent batch, never broad suites after every edit.
- Update the small feature catalog after implementing its actual consumer and
  refresh the single progress page with mugen.sh --coverage --write. Routes alone
  do not establish complete semantics. Mark the stated scenario complete after
  it passes; do not call known partial features complete.
- Keep handoffs and plans short. Historical detailed research is optional
  background and Git history, not a queue of mandatory experiments.
- Do not add generated testing data, captures, PCM, copied assets, binaries or
  new fixture matrices to Git. Reuse the supplied assets and shared build cache.
  Segment scratch output is automatically deleted on success AND failure; keep
  only a concise failure explanation or explicit user-requested artifact.

## Scope and execution

The user authorized the remaining functional work on 2026-11-07: BGM/NoMusic,
teams/modes/AI, remaining stage/team context, assets and platform support. Defer
color/shadow/alpha and KO-echo precision until functional coverage is finished.
Continue autonomously with local source/text checkpoints. Author only game/mugen/.
No pushes, committed binaries, subagents or goals. Preserve unrelated root work.

Use the readonly mugen.mod/mugen.sum overlay and preserve root module files.
Prefer rg/rg --files --no-ignore; historical ignores hide source. Do not recurse
into temp for source discovery. Temporary Go helpers belong under temp/_tmp/.

For this host use python3 game/mugen/temp/round-task-env.py with its authorized
elevated prefix for Xvfb:97/Pulse/Go. Native/app/display-dependent runs stay serial;
await the entire process and confirm owned clients exited before edits/new runs.
Never expose cookies or unrelated process arguments or reset shared servers.

The user authorized temp cleanup. Nine generated run directories were removed,
recovering126.58 GiB; native originals/captures/prepared copies in those directories
are intentionally no longer retained. This supersedes earlier raw-evidence
retention instructions. Canonical temp/mugen-1.1b1, build tools/cache and old
tracked fixtures remain. Never regenerate removed matrices just to pass legacy
pixel diagnostics. --check-all is explicit historical diagnostics only and some
artifact-dependent tests now require unavailable captures.

## Checkpoints

Before each local commit audit explicit staged paths and UTF-8/no-NUL text; add no
temp/testdata/binary or outside-scope files. Avoid force-adding directories. No
new native data was promoted. State/controller closure, startup constraints,
vertical edges and stock bitmap placement are complete at abf8a2d4a. The first
functional item4 batch implements WAV/MP3/Ogg BGM, NoMusic, scene transitions
and StageVar; all five affected connected segments pass with race detection.
Simul/turns rosters, all team trigger bindings, partner combat/power and team
selection/HUD now pass four affected connected segments with race detection.
Continue mode progression/AI and recovery policies, then broader assets/media/
platforms. Legacy MIDI/tracker/CD backends are still unimplemented. Precision
remains partial and deferred. Read docs/PROGRESS.md and docs/HANDOFF.md.
