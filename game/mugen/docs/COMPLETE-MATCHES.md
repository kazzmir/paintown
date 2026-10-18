# Complete stock matches and rematches (M4bw)

Validation checkpoint after ddb48b18. The production Go implementation is
unchanged from M4bv; this checkpoint strengthens tests and adds an optional
owned-window input replay to the application smoke tool. Final full scoped race integration passes. The three-checkpoint round-lifecycle
allocation is complete; whole-engine compatibility remains unfinished.

## Deterministic production scenarios

`match/complete_match_test.go` uses both stock KFM profiles, original spawn
positions, full life, original state/animation files and production command,
collision, damage, sound and round scheduling. It never injects life loss,
forces a state or repositions an actor. Inputs walk toward the opponent and
press X through the logical input provider. The provider is retained across
round reconstruction through Options.Input.

| Scenario | Rounds | Wins | Outer ticks |
| --- | ---: | --- | ---: |
| P1 wins by actual attacks | 2 | 2/0 | 4,615 |
| P2 wins by actual attacks | 2 | 0/2 | 4,615 |
| P1/P2/P1 alternating winners | 3 | 2/1 | 6,895 |
| Time-over draws | 3 | 2/2 | 1,398 |

Every combat round verifies actual damage, common contact sound commands, a
KO rather than time-over, the intended winner and natural post-KO readiness.
The maximum pre-over count is 70 for combat and 45 for time draws; none uses
the 900-pass fallback. Each round reconstruction checks full life, root identity,
RoundsExisted and cleared result flags. Three further fresh P1 rematches finish
with independent world/effect ownership, fresh clocks and reset scores.

`system/flow_test.go` now traverses title, both character selections, stage,
versus and fight, then wins two rounds through the configured keyboard adapter
using the stock L key and directional movement. It verifies results, common PCM
output, the round-two/KO HUD lifecycle, Start rematch and Escape to title. This
replaces its former injected life-zero/death-state setup.

Together these tests complete eight production matches and 18 rounds, including
three repeated rematches and one all-draw match. The four primary Match cases
plus rematches pass race detection in 16.153s; the first full keyboard flow pass
is 6.351s including the race harness. This is bounded one-on-one stock coverage,
not proof of every character, input timing combination, AI or mode.

## Rendered keyboard replay

`tests/app_smoke.py --stock-input` sends key events only to its spawned window;
it never moves the pointer or changes desktop focus. The disposable configuration
uses 2% life and one required win for a short one-hit demonstration. It preserves
stock character/state/animation resources. Walk pulses end before each attack;
Start at 23 seconds creates the second match. Full-life conformance remains in
the deterministic scenarios above.

The accepted run is temp/app-runs/m4bw-stock-input-one-hit. Visually inspected
captures show the terminal win/rematch prompt at 21s, the fresh fight at 26s,
and the second terminal win at 45s. Four PNGs, 82 events targeting one PID/window,
config/fight hashes, binary and private PCM pass provenance. PCM contains
1,953,229 stereo frames, peak 32767 and RMS 1647.98. These checks prove rendered
interaction and device output, not native whole-frame or waveform parity.

The initial held-forward replay produced movement without attacks. Releasing
forward before button pulses produced actual damage but did not finish on the
fixed 20%-life schedule. Both runs and their snapshots remain diagnostic; they
are not counted as completed-match captures. The final independently configured
2%-life run verifies both terminal screens. No numeric golden was changed.

Source manifest: temp/m4bw-final3-production-inputs.json (278 Go/test/probe
hashes). All 154 production Go files are byte-identical to the M4bv manifest.
App audit: temp/m4bw-app-evidence-audit.json. Application SHA256:
8bd3896ddb04b376a4e80ca510a15b51d0320b3ce4b77fce643e32970775f034.
Earlier source snapshots and diagnostics are retained.

Reproduce on a private display/audio server (from the repository root):

```bash
python3 game/mugen/tests/app_smoke.py \
  --name stock-input-review --motif data/mugen1/system.def \
  --match chars/kfm/kfm.def chars/kfm720/kfm720.def \
  --stage stages/kfm.def --width 1280 --height 720 \
  --seconds 3 21 26 45 --stock-input --audio
```

Supply DISPLAY/XAUTHORITY and the private PULSE_SERVER/ALSA_CONFIG_PATH as in
[BUILD.md](BUILD.md). The tool requires the stock reference resources under temp/;
compiled binaries, PNGs and PCM remain ignored. Use a fresh run name. For ordinary
interactive play, use [the README launcher instructions](../README.md).

## Remaining original-target gates

The native GameTime observations include the menu origin; current isolated Match
instances own their own Timeline, and Engine.Update has no cumulative clock to
hand into a new match. Passing the native engine-wide origin through menus and
rematches remains an integration gate. The standalone fresh-clock ownership
assertions and M4bv's origin-aligned traces do not close it.

The legacy HitVel trigger counterexamples from M4bv remain open, as do remaining
controller/trigger schemas and bindings, full recovery/input timing, projectile/
reversal/team behavior, modes/AI/progression, BGM and format/font/render parity.
The six M4az alpha and two M4bd spark differences remain unwaived. HitDef typed
coverage stays 81/81; no overall behavioral completion percentage is claimed.
The full scoped command `--check -race -p 1 -count=1 -timeout=20m -json` passes
in 1,037.599 seconds. There are 2,707 passing test/subtest results, 24 tested
packages and 12 additional compiled application packages, with no failed or
skipped individual tests. Match takes 968.597s, Character 11.496s, System 7.234s,
Stage 1.041s, engine 1.460s and stock/resource tests 6.738s. The progress monitor
abbreviated package names and conflated two application names with production
packages; the final audit uses all 36 full package identities. Numeric test
results and the successful exit code are unchanged.

Final audit: temp/m4bw-final-evidence-audit.json; full log:
temp/m4bw-full-scoped-race.jsonl and its result JSON. All 278 source hashes and
root module checksums pass after the run. No test/application clients remain.
The local checkpoint contains source/text only; no push or binaries.
