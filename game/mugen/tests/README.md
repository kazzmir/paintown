# Tests and reference fixtures

From any working directory, run `game/mugen/mugen --check` using the launcher's
absolute path if needed. It keeps build/test temporary files under
`game/mugen/temp/` and prevents changes to the root Go module.
It uses scoped `mugen.mod`/`mugen.sum`; direct Go commands need
`-modfile=game/mugen/mugen.mod -mod=readonly`. See [build notes](../docs/BUILD.md).

Required reference content is the supplied, unmodified M.U.G.E.N 1.1 beta 1
distribution at `game/mugen/temp/mugen-1.1b1/`. Set `MUGEN_REFERENCE_DIR` to use
another extracted copy. Missing files and mismatched hashes fail rather than
skip; `testdata/reference-manifest.json` pins 109 content/spec/tool files without
committing the distribution itself. No test reads the old `data-new/` tree.

The `physics_check`, `character`, `stage`, `air_player`, `snd_player`, and
`fetch-all-data` directories are executable diagnostic applications. Launch one
with `game/mugen/mugen --test NAME`; pass its `--data-dir` explicitly when needed.
Their output is not a substitute for semantic assertions.

Pure parser/evaluator tests can run without DISPLAY. Packages importing
Ebitengine currently require a display even when their tests do not open a
window; the simulation extraction in M2 will remove that coupling from state
tests. Do not describe the current entire test tree as headless.

The original engine is never executed in place. Engine experiments use
copies under `temp/reference-runs/` and a Wine prefix under `temp/wine-reference`.
Reference-generated files, extracted images and captures stay under `temp/`.
Tiny original reference-compressed fixtures and numerical pixel hashes may be
committed; Elecbyte art remains untracked.

Run headless language fixtures with:

```
game/mugen/mugen --test expressions --input game/mugen/tests/testdata/expressions.jsonl
```

Each JSONL fixture has a name, initial state time, and ordered expression steps
with expected kind/value and optional variable assertions. The runner emits
deterministic observations (including warnings and variable changes) and exits
nonzero on malformed fixtures or mismatches. These use the production typed
evaluator; world-bound trigger semantics require the forthcoming engine runner.

Run the authored reference animation probe with
`python3 game/mugen/tests/reference_probe.py --name UNIQUE_NAME`. It requires
Wine, DISPLAY, xdotool, ImageMagick `import` and Pillow; it copies the supplied
distribution and uses a private Wine prefix under `temp/`. It captures only the
spawned process's window, sends no input, and makes no focus/pointer changes.
Each capture reads two clipboard lines by exact matching against the bundled
debug font, rejects ambiguous/missing glyphs, and checks the pinned observations.
Captures/logs/hashes remain in its run directory. Each run name must be new.
The Go clock regressions use the committed numerical observations without Wine.

Named state probes use `--fixture state-order --pages 0` or
`--fixture persistence --pages 0 12`, each with a new `--name`. `--record` saves
new observations only in that disposable run directory; it does not change the
committed expected data. A fixture may supply its own neutral CMD state program.
Engine and Character tests run both programs against the pinned variables.

Float-valued probes select `--bank fvar`. `constants-a --bank fvar --pages 0 12 24`
and `constants-b --bank fvar --pages 0 12 24 28` cover the documented constant
catalogue; the one reference-rejected name is an explicit unavailable slot.
The exact glyph reader accepts signed decimals/exponents, preserving nine-digit
float32 round trips. Kind assertions follow the documentation's declarations.

Build the optional error reader with `game/mugen/mugen --reference-tools`.
It reads only the reference MUGEN error dialog inside the private Wine prefix,
without input/clipboard/focus operations. The runner uses it to record and check
expected load rejections. For example, `--fixture constant-unavailable --pages 0
--mugenversion 1.1` checks the pinned invalid-constant error. `--bare-constants`
is an omission/load-error experiment using empty constant groups; the supplied
engine rejects incomplete Data groups. It is not a source of valid fallback data.

Resource matrix cases run with `--fixture resource-load --case st9-version11`
and a fresh run name. Authoring options `--file key=value`, `--omit-file key` and
`--clipboard-in-cmd` are recorded in evidence and checked on replay. The character loader now exercises the pinned load/precedence rules.
`--fixture resource-order --case merge-numbered-first` verifies declaration-order
independence. An optional `.extra` directory contains authored auxiliary files;
its hashes are pinned. `--state-file-order st0 st` records reordered declarations.

`TestReferenceCharacterResources` compares all twenty loader cases directly.
Common probes wait for the initial round-control grant before forcing entry,
and Go starts at that same point. See REFERENCE.md for the earlier startup
difference and its isolation. Missing/malformed resources, source filenames,
Windows/case names and independent common loads have separate regressions.

`--fixture entry-control --mugenversion 1.1 --pages 0 12` replays transition versus
entry parameter precedence. Pure engine and Character tests compare all 24
recorded variables, including incoming-control reads by entry expressions.

`--fixture state-binding --mugenversion 1.1 --clipboard-in-cmd --pages 0`
checks cosmetic controller labels and first-definition selection; `state-orphan`
checks controllers before any definition. Both are replayed by pure engine and
Character tests.

Stock asset-loader tests cover all four motif main archives/fonts and all three
stages; they assert loaded resources, not merely successful DEF parsing. Separate
regressions exercise missing/malformed declared assets, source locations, the
documented motif font search order and Windows/case font-child names. These are
not screenshot/rendering parity tests or per-background/storyboard coverage.

Background archive tests check KFM's separate title archive, matching CPU element
bindings, sprite-ID collisions, masked/opaque caches and foreground selection.
All four supported motif background override declarations reject missing or
malformed archives with section/key/line context. Full screenshot parity remains
separate work.

Storyboard loader tests cover all eight stock cutscenes, CPU-only resource
construction and bounded playback, plus authored scene/index/action/resource
failures and layer 99. Application tests cover failure propagation and resolved
selection paths. Configured-duration assertions are regression checks, not
native timing or screenshot parity. Text/audio and full scene drawing remain
incomplete.

Run `python3 game/mugen/tests/app_smoke.py --name UNIQUE_NAME` for production
startup inspection. It builds with the scoped module, copies the reference into
temp/app-runs/, selects the KFM motif, excludes ambient YAML overrides, and
captures its own process window at 2/6/16 seconds. Go, DISPLAY, xdotool and
ImageMagick import are required. It sends no input or focus changes, records
hashes and commit/dirty state, and terminates its own process. Use --motif or
--seconds for other inspection points. Live keyboard/window timing can affect
results; it does not assert native image parity or deterministic gameplay.

Input regressions use the stock SDL key-number corpus and injected keyboard
sources; no OS key events are generated. They cover all 22 stock bindings,
explicit KeyA=0 and disabled presence, YAML overrides, per-player confirmation,
and the actual facing-relative gameplay input path. See INPUT.md for remaining
command timing, layout and gamepad work.

Fight resource tests cover both stock fight definitions, main SFF/SND/fonts,
embedded AIR, fightfx SFF/AIR, common sounds and retained layout attributes.
Missing/malformed declared files are checked with source context. HUD drawing
and live match behavior require separate M4 integration tests.

Coordinate tests retain all stock profiles, distinguish game space from window
size, verify width-based translation and compare float scaled constants against
five native cases. Replay with `--fixture coordinate-constants --case fractional640`
and a fresh run name. `--localcoord width,height` is an authored-probe override;
its value is pinned. Mixed-player geometry and complete camera/rendering parity
still require Match integration.

Mixed-player geometry: `reference_probe.py --fixture player-distance --case
240-720 --name UNIQUE_NAME` replays an independent opponent profile. The
`--p2-localcoord` override copies the authored probe player into a second private
character directory; the original reference is unchanged. Character tests pin
both coordinate pairings and compare all recorded float variables.

Production fights: see docs/MATCH.md. The system flow test injects keys through
title, selection, stage, versus, fight, result, rematch and menu. match tests use
the same lifecycle with stock KFM/KFM720 and logical walk/punch input. For owned
window inspection, app_smoke.py accepts --match CHAR_DEF CHAR_DEF and --stage.

Guard/control probes: --fixture guard-end, guard-start or control-gating with
--mugenversion 1.1 --pages 0 and a fresh --name replay native state/time traces.
These fixtures wait for round control before entering the measured state; the
Go runner starts at the corresponding control boundary.

Received hits: --fixture hit-coordinates --case 240-720 and --fixture
hit-defaults --case smallgame replay local/common coordinate cases. Infinity
observations use positive_infinity/negative_infinity strings in native JSON;
JSONL expression traces use a nonfinite field and value=0 as a JSON placeholder.
float-overflow, float-math-overflow and scientific-accepted are numeric native
probes (--bank fvar --mugenversion 1.1 --pages 0). scientific-notation instead
pins the native rejection of 1e2. Decimal forms such as 1.0e2 are supported.

Palette integration: palette/ tests cover mapping semantics and selected pixels
in both stock archives. Character tests cover cache refresh/removal; match tests
verify common CNS applies PalNo again after reset. selected-palette-fight is an
owned-window Go capture of the actual renderer; no native remap-edge parity is
claimed from it.

Contact ordering: --fixture hit-trade-phases or hit-trade-pause --case default
with a fresh --name replays same-frame/pre-damage and next-frame/post-damage
values. Character tests use the same TickCharacters entry as production. The
fixture tests do not open a window, but current Character imports still need
Ebitengine display initialization; only engine/evaluator are display-independent.

HitOverride: override-* fixtures are pinned cases replayed with --case default;
clipboard output is appended to the negative CMD state so every destination can
report. The Go suite now gates all 108 stock states compiling and compares twelve
native override cases. Full controller parity remains open. When desktop outputs
are disconnected, use the private display environment described in docs/BUILD.md.

Contact/guard reference regressions also cover prior-tick HitDef activation before
negative states, attacker hitpause versus receiver hitshake, HitOver's -1 edge,
guard posture/flags/damage/vectors/timers/defaults, guard-end eligibility and
HitOverride precedence. Twelve source-pinned programs compare production group
ticks; pause-release-replay repeats native clock packets. M2b/M4g timing
assumptions are superseded; see docs/REFERENCE.md. These do not establish full
priority/protection/helper/projectile or GetHitVar parity.

Protection probes protect-* cover two shared HitBy/NotHitBy slots, independent
mask complements, partial/empty masks, duration/expiry/clearing, replacement,
current attacker posture, both-parameter precedence and missing-parameter load
rejection. Twenty-four accepted programs plus two override posture cases compare
production group ticks; protect-missing checks compile rejection. A dynamic-pose
native replay verifies the captured case. Complete invalid-value, pause and
entity/ownership behavior remains open.

Guard-distance/order/push probes add 23 programs/26 cases for attacker-owned
range, direction/height/default retention, state-entry reset, AttackDist,
attack-first state-pass priority, custom facing, PlayerPush reset/suppression,
near-axis spacing and mixed coordinates. They all use production group ticks.
--fixture-constants selects complete authored CNS constant groups without a stock
prefix and is pinned in case metadata; --bare-constants retains its prior empty-
group mode. Push fixtures use complete numeric KFM defaults with explicit widths.
Native partial Data rejection still needs dedicated loader-validation coverage.

Sound controller/mixer fixtures are `testdata/reference/sound-*`. Twelve audio
cases replay real grouped Character ticks through the CPU mixer and compare
63 plateau checks against 55 native segments. Twelve additional grammar cases
pin accepted/rejected resource-prefix and parameter behavior. Sources, original
bank bytes and native executable/config/PCM hashes are retained. Regenerate an
original bank with `python3 game/mugen/tests/generate_sound_bank.py --help`.

Native sound replay requires the private Pulse server documented in
[../docs/AUDIO.md](../docs/AUDIO.md), plus the private display in BUILD.md:

```sh
DISPLAY=:97 XAUTHORITY="$PWD/game/mugen/temp/virtual-display/Xauthority" \
LIBGL_ALWAYS_SOFTWARE=1 \
PULSE_SERVER="unix:$PWD/game/mugen/temp/audio-reference/native" \
PULSE_COOKIE="$PWD/game/mugen/temp/audio-reference/cookie" \
python3 game/mugen/tests/sound_probe.py --fixture sound-channels \
  --name sound-channels-audio-replay-new --seconds 14
```

Use a new run name. `--record` records a selected case; default replay checks
source/numeric observations, settings, tone amplitudes and relative onsets.
It records only the private null sink, never a microphone or desktop audio.
This is controlled PCM evidence, not full-waveform or device output parity.

M4m adds `hud-sound`: a one-second original tone replaces only disposable native
fight announcement settings. sound_probe.py --fixture hud-sound replays the
pinned recipe automatically. The Go HUD test compares stereo amplitudes and
sample expiry, not absolute round timing. app_smoke.py --audio additionally
records real Go device PCM from the private null sink; set ALSA_CONFIG_PATH to
the task-local pulse route described in BUILD.md. Production-flow tests read the
same mixer without opening a device and verify announcements, bank binding,
round reset, live volume and cleanup.
