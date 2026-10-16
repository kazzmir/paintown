# Implementation checkpoints

The user approved PLAN.md on 2026-10-16 and authorized local commits for
milestones/checkpoints. No remote push is authorized. All source, fixtures,
documentation, and tooling stay under `game/mugen/`.

## Reviewed baseline

Assessment, plan, compatibility inventory, and working instructions captured
before engine changes. The original full test target fails with duplicate main
functions; the 11 aggregate parser/config/asset tests pass using external data.
Reference behavior has not yet been measured. The original distribution remains
untouched and is excluded from commits along with temporary build artifacts.

## M0a — Reproducible build and pinned reference baseline

Moved the console physics application into its own package, resolving the
duplicate-main build failure. Existing corpus tests now require the supplied
1.1 beta distribution, without external data paths or skip-on-missing behavior.
Pinned 109 reference content/spec/tool files by size and SHA-256; assets remain
untracked. Added UTF-8 BOM handling for the supplied font definitions and a
regression test. Configuration tests can disable ambient YAML lookup explicitly.
Diagnostics now default to stderr and accept a caller-owned writer instead of
creating a fragile working-directory-relative log file.

The launcher runs entire packages, adds `--check`, keeps build/test temporaries
under `temp/`, and holds the root module read-only. `tests/README.md` describes
the fixture setup and current Ebitengine/display coupling.

Validation: `bash -n game/mugen/mugen` and
`GOPROXY=off game/mugen/mugen --check` pass. The required reference manifest is
verified during the suite, and all 33 stock DEF files parse. No state-machine
compatibility claim is made by this checkpoint.

Reference experiment: an isolated Win32 Wine prefix can start the executable
far enough to write its status log. `mugen.exe -h` did not exit within 40 seconds;
it is not a successful gameplay verification. The log identifies the supplied
binary as **1.1.0 Beta 1 P1 (2013.08.11)**, while the readme labels the distribution
1.1 beta 1 (2013-07-27). Use the supplied, pinned executable as the oracle and
record this distinction in future comparisons. Quick-match startup is being
investigated. Headless semantic fixture execution and strict controller/trigger
diagnostics remain M0/M1/M2 work.
