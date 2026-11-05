# MUGEN Go port — active instructions

## Authorization and scope

The user selected **complete remaining effects/controllers (item3)** on
2026-11-04 and repeatedly authorized continuation. Continue autonomously through
ITEM-3-PLAN.md with documented local checkpoints and commits. Do not stop after
two checkpoints. Stop after item3 or a concrete issue needing user input. Item4
(modes/teams/AI/BGM, assets, remaining triggers and final renderer/font/platform
conformance) remains unallocated. Earlier allocation stop conditions are history.

Author changes only under `game/mugen/`. No pushes, binaries, subagents or goals.
Preserve unrelated root work and incoming upstream font/module changes. Commit
source/text only; audit staged paths, UTF-8 content and absence of binary data.
The tracked `mugen.sh` is a Bash launcher. Reference resources and build outputs
remain ignored. New engine files may require explicit `git add -f` because of a
historical root ignore rule; never force-add the whole tree or temp assets.

Target Elecbyte **M.U.G.E.N1.1 beta1**, using immutable `temp/mugen-1.1b1/docs/`
and its stock data as primary reference. C++ `src/mugen/` is incomplete prior art.
Use the scoped mugen.mod/mugen.sum overlay and readonly module flags. Verify
`sha256sum -c game/mugen/temp/upstream-post-merge-module.sha256`.

## Immediate state — M4cy validated, local commit next

M4cx is committed as3524cdf0f823b9bd35c8f7ec61bb61298d1ea94d. M4cy now adds
world/global context reservations, pre-evaluation gates, invisible-current draw,
blank palette aging, recorded palette snapshots and history sprite-cache life.
48new native programs/576values/194source-probe paths; combined189programs/
2244raw values/2100stored comparisons/nine rejects/758original hashes.

Entire finalchain70209 completed/awaited1; strict matrix FAILS for TWO UNWAIVED
45degree controls(current-only534RGB/17maskXOR;trail2285RGB/65maskXOR,max148).
All48cases collected;46exact23,404,800RGB pixels. Current-only hasNOAfterImage;
90degree trail exact. Older12M4cq/sevenM4cv also remain UNWAIVED. Do not relax
comparisons or call this full family/framebuffer parity. ReadAFTERIMAGE-ACTORS.md.

Full scoped races PASS4243results/36packages(25tested/11compiled-only),no failures,
individual skips/races;Match1858.841s,total1943.381s. Four old hit/fall masks pass.
Stock21win/26rematch/45win visually inspected;82balanced owned events/1,920,802
private PCM frames. Final audit87805 completed/awaited0, then separate15,999
source/native/driver/client guard PASS. Audit:m4cy-final2-evidence-audit.json;
stockvisualreview recorded. Modules/inventory/native/resource/binary/config/
PNG/audio provenance pass. No active clients or runtime sessions remain.

Native originals/goldens are IMMUTABLE. Initial failing numeric/baseline/provenance
and complete43/46matrix outputs remain. All48final native config recipes/final
bytes were exactly reconstructed:FirstRun1->0,LF->CRLF only. Separate ledgers
retain bothSHA without changing original recorder/evidence/expectations.

NEXT:finalize docs,refresh explicit proposed source/text paths,run scoped UTF8/
noNUL/noBinary Git staged audit and LOCALcommit. Dormant stage-m4cy-final.py
expects m4cy-proposed-commit-paths.json(listofpath/sha records). Preliminary265
paths in m4cy-proposed-commit-draft.json are NOT final hashes and NOT staged.
Preserve unrelated root/index work; no push. Record actual commit/clean scope.

THENcontinue ALLitem3 with projectile nested afterimage fields/shared shadows.
49ignored projectile drafts remain UNOBSERVED/UNPROMOTED. Original pureCNS/AIR
intent87cases(actor+projectile) verified, but native observations are still needed.
Readignoredtemp/m4cz-projectile-study-plan.md and m4cy-native-research-draft.md.
Add capacity/lifecycle/pause/shadow controls before inferring actor semantics.
Keep Go experiments ONLYtemp/_tmp. No agents/goals/binaries. No checkpoint stop.

## Native and validation workflow

1. Freeze separately named original fixtures/recipes before native observation.
   **Never edit/delete/move original native inputs, captures or goldens**, including
   failed diagnostics. Never relax comparisons/tolerances to make a test pass.
2. Inspect an **all-case native-only consumer/layer table before implementation**.
   Do not infer uninspected matrix branches from manual prose. Native beta quirks
   can differ from its manual. Check actual controller kinds per state.
3. Implement typed schemas/defaults/errors, evaluation order and real production
   consumers. Unknown controllers/invalid triggers must not silently succeed.
   Keep file/state/controller/expression context in diagnostics.
4. Test counterexamples/dependencies first. Collect every strict RGB mismatch
   in a batch, retain each public probe's failure status, and fail the complete
   matrix on any mismatch. Never describe a partially passing matrix as exact.
5. Use focused races/apps during revisions and full scoped races at meaningful
   integration boundaries and final allocation closure. Run required checks;
   broaden/repeat only for changes/failures/unresolved concerns. Full races use
   `--check -race -p 1 -count=1 -timeout=40m`.
6. Freeze source/test/probe/fixture/module inputs for each runtime run. **Native,
   actual-app and display-dependent Go runs are strictly serial.** Await every
   exec session, then a separate client guard before edits/another runtime.
   Never edit frozen inputs or the active driver while a run is executing.
7. Record findings, failures, exact limits, tests and remaining work at each local
   checkpoint. Update README, COMPATIBILITY, COVERAGE and HANDOFF as relevant.
   Full stock selection/fight/results/rematch and required private audio remain
   acceptance gates. Source routing and behavioral evidence are separate counts.

## Tools and private environment

Use `rg` / `rg --files --no-ignore` to discover source paths before reading them;
root ignores hide some engine files. Disposable Go packages/builds belong under
`temp/_tmp` (underscore), preventing recursive package discovery. No ordinary Go
experiments anywhere else under temp. Keep the Go module readonly.

Private runtime/Git wrapper from repository root:
`python3 game/mugen/temp/round-task-env.py ...` (elevated tool execution uses the
already authorized wrapper prefix). It checks clients and sets private display,
audio, Go cache/_tmp, scoped modules, Wine32 shims and local dev headers/libs.
Xvfb:97/Pulse are task-owned; metadata is temp/item3-private-servers.json. Check
selective process output before inferring a server died; full listings truncate.
Do not print private audio cookies. Do not restart/kill unrelated user services.

Byte-identical completed private Go app binaries and stock SFF/SND/font copies
may share storage with their paths/content/hash/permissions preserved; no native
resources, screenshots or expectations are candidates. Prior record:
temp/private-go-asset-sharing.json. Keep all diagnostics and evidence intact.

## Remaining allocation and historical record

AfterImage/AfterImageTime variants and projectile trails/shadows; remaining
Explod creation/ownership/remapping/random/angle/trans/shadow/capacity; all
AssertSpecial consumers; GameMakeAnim/MakeDust; clipboard formatting; force
feedback and actual target/device boundary; residual typed/literal schemas,
HitAdd HUD, StateTypeSet and FVar aliases. `-nojoy` alone cannot prove a hardware
limitation. VarSet literal7.9 must reject natively; fix conflicting old unit
inputs to7.9+0.0 expressions without changing native goldens.

Read current HANDOFF and topic notes. Old detailed status is preserved in
`docs/AGENT-STATE-HISTORY-M4CU.md` for historical lookup only; its old stop
conditions are superseded. No engine-completion percentage is currently justified.
