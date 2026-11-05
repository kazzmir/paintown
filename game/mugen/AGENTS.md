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

## Immediate state — read HANDOFF.md

Latest commit **M4cu b8654fca3**; prior M4ct a048effa3. M4cv EnvShake is
implemented and in validation. All72 original programs/289 source+probe hashes
are immutable;66 accepted/792 values/six rejects. Native-only all-case table was
inspected before implementation. Missingtime defaults0;ampl raw-4;runtime ampl
fractions persist;frequency abs, exactzero=>.001, nonzero tiny magnitudes survive.
Nonpositive standalone requests clear pending only, retaining active shake.

**M4cw frequency checkpoint ACCEPTED; local commit next.** No runtime active.
Native8530finished/awaited,49hash/clientguardPASS; ALL12table inspected before
implementation.12accepted/144numeric values. Contact/fall usesABSfreq/exactzero
fallback.001/tiny nonzero retained; defaultphase follows normalizedfreq. Shared
cns/hit_shake.go corrected. Baseline66979failedexpected (oldfreq.001/native90,
phase0/native90); originalsource verifiedmatchesM4cv99a5. No oldgolden edits.

Acceptance94455finished/awaited0; separateclient/sourceguardPASS15,035inputs.
Fourcore races +145native programs(152results/93.361s),four originalcontact/fall
render masks and stockwin/rematch/win visuallyinspected82events/1,915,680PCM PASS.
Final audit completed0:m4cw-final-evidence-audit.json. Native/source/resource/
binary/config/PNG/audio/module/inventory guards pass. No newfullsuiteclaim:
latestfullM4cv4,033/36(25tested/11compiled-only),Match1736.103s,14,998inputs.
SevenM4cv profiles and12M4cq rasterfailures remain UNWAIVED. Read SHAKE-FREQUENCY.md.
Sourcecounts72typed/17fallback/5stubs/9partial/15bounded/60route-only unchanged;
no variantcomplete families/whole-engine percentage. No sampler change.

Next audited localM4cwcommit, separateidle guard, execute PREPARED ONLY
prep-m4cx-pilot.py (12AfterImagefirstlook controls), run generatednativebatch,
await/guard and inspect ALLnative rows/captures BEFORE implementation/additional
controls. No AfterImagefixtures/Go yet. Continue ALLitem3; do not checkpointstop.
M4cw originalauthor assertion caught ambiguousFVar10/11snapshot beforeexecution;
24alreadywrittenpaths preserved, separate correctedauthorcompleted12/49hashes.
See docs and m4cw-author-partial-inputs.json; originals remain immutable.

M4cu acceptance:76 native/68 accepted/816 values/eight rejects/305 hashes;
68 exact crops31,715,200 pixels/2,516 samples; core+189 native races,ten regression
apps and visually inspected stock82events/1,972,229 private PCM frames. Audit:
temp/m4cu-final-evidence-audit.json. Latest full **M4cs**3,857 results/36packages
predates M4ct/M4cu/M4cv. Current source72typed/17fallback/5stubs/9partial/15bounded/
60route-only is a source-routing count, not behavioral completion.

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

EnvShake; AfterImage/AfterImageTime and projectile trails/shadows; remaining
Explod creation/ownership/remapping/random/angle/trans/shadow/capacity; all
AssertSpecial consumers; GameMakeAnim/MakeDust; clipboard formatting; force
feedback and actual target/device boundary; residual typed/literal schemas,
HitAdd HUD, StateTypeSet and FVar aliases. `-nojoy` alone cannot prove a hardware
limitation. VarSet literal7.9 must reject natively; fix conflicting old unit
inputs to7.9+0.0 expressions without changing native goldens.

Read current HANDOFF and topic notes. Old detailed status is preserved in
`docs/AGENT-STATE-HISTORY-M4CU.md` for historical lookup only; its old stop
conditions are superseded. No engine-completion percentage is currently justified.
