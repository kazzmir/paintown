# Current handoff

Follow AGENTS.md; PROGRESS.md is the single ledger. Author only game/mugen/.
Local checkpoint commits are authorized without individual approval. Recap about
every five commits; ask only for needed input. Never track user configuration.
No pushes, binaries, assets or generated banks. Use the private wrapper with
GOTOOLCHAIN=go1.26.9. Runs stay serial: await exit, guard, then edit/run.
Reuse assets/cache; remove disposable output.

ZIP checkpoint a010d8e2d implements the beta's pack.zip and
pack.zip:member.def selection syntax, including spaced filenames and case-folded
Windows paths. Engine-owned expansions are shared across portraits, alternate
DEFs, rosters, quick matches, rounds and rematches. Engine.Close stops readers
before removing expansions; main now unwinds startup errors and handles Ctrl-C/
SIGTERM through the normal game exit. Source ZIPs and user configuration stay intact.

The connected authored asset lifecycle includes packed selection/portraits,
arcade intro, versus/fight, outfits and round reset, direct roster reload, ending,
missing member/path errors and cleanup. Match-flow passes with race detection;
other segments stay checked. No stock assets were copied into fixtures; segment
scratch removes both ZIP input and expansions.

Current portrait checkpoint adds SFF v1/v2 selective loading for 9000,0/9000,1
and linked pixel sources. V1 retains palette sequencing while reading only the
needed palette trailers/pixels; V2 resolves required link chains. Selection no
longer decodes all combat frames. Gameplay still loads the full archive. The
connected asset flow covers v1 linked/shared/private colors and stock v2 selection,
then full fights/reset and ZIP cleanup. Match-flow passes with race detection.
Linux rebuild/playable acceptance
will run at the end of this implementation batch. Prior 848b00caf and its four
predecessors cover retained storyboard rendering, fades, font paths and PCX data.

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
