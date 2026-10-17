# Diagnostic stage background fill

M4bc implements the supplied beta's `[BGDef] debugbg` behavior. The original
manual (`docs/bgs.html`, BGDef) and stock stage comments describe a magenta clear
before background layer 0. `Stage.Draw` owns that clear, so production fights
and the standalone stage viewer share it. Foreground drawing remains a separate
pass.

## Native controls

Eleven original `stageclear-*` programs record 132 numeric observations and
44 CNS/AIR/CMD/stage hashes. Their unchanged KFM sprite banks are referenced
from the disposable distribution; no reference artwork or executable is
committed. The controls reuse the accepted nonlethal contact program so actors
stay still and round announcements do not obscure the measured regions.

| Setting | Native sampled fill |
| --- | --- |
| Omitted or `0` | Black |
| `1`, `-1`, `2` | Magenta |
| `.5`, `-.5` | Black |
| `1.5` | Magenta |
| `foo` | Black |

The numeric flag truncates toward zero before testing nonzero. A plain float
boolean was wrong for `.5`; flooring was ruled out by `-.5`. Invalid text gives
the disabled result in the measured case. The stage loader now applies that
conversion explicitly.

With the default 1280x720 profile, the stage's uncovered area is magenta while
the outside fit bars stay black. `StageFit=0` and `KeepAspect=0` expose magenta
at the calibrated window-edge points. This is a stage clear, not a whole-window
or HUD clear. Omitted/zero controls establish sampled static black pixels; they
do not establish general framebuffer persistence for backgrounds with holes.

## Exact render gate

`tests/stage_clear_probe.py` checks an unobscured 200x40 solid-color region at
900,660..1100,700 and three viewport points. It records only black/magenta color
metadata and hashes, never stock image pixels. Native recording is restricted
to the authored fixtures. Go verification cannot create native goldens.

The checker validates original source hashes, prepared player/stage/SFF/HUD
inputs, native executable and configuration, Go binary/configuration, captured
window dimensions/hashes, runtime diagnostics and the selected video profile.
Fresh native verification also checks the numeric observations. Native width
and height may be omitted: their documented fallback is the configured game
size, and the captured window must still be exactly 1280x720.

Two failures were preserved before their fixes. `temp/m4bc-before-fill.json`
records 8,000 differing pixels (native magenta, Go black). The first float
boolean implementation passed the seven integer/profile cases but failed the
`.5` control: all 8,000 pixels were magenta instead of black. That capture and
`m4bc-half-before-check.log` remain diagnostic evidence. No precision allowance
or native golden change was used to make Go pass.

All12 final app runs/15 captures pass input, binary, configuration, PNG and
runtime checks. Eleven exact color regions (88,000 pixels) and33 viewport
samples match. Binary48df6ff9c82e06c1f73b459cd3265a0ceaef0edd18ec9310eda8a22e6b2c5548
is identical across the final runs. Results are in m4bc-final-app-audit.json.
Four fresh native programs reproduce numeric and pixel observations: half,
negativehalf,stage0,onehalf (m4bc-replays.log). Sequential scoped races pass
in m4bc-scoped-race.log: System4.713s, stock tests6.502s; stage builds. Pipeline
44482 completed exit0, including root module checks. Stock7s was inspected:
two recovered fighters, no persistent effects. The preceding full M4bb suite
passed before this isolated stage change (Match618.564s). Reversal is9ef4c313.
General background controllers, foreground/tint/pause combinations, six M4az
alpha precision gates, and complete renderer/asset/mode compatibility remain
open.

## Reproduction

From the repository root with the private display/audio environment in BUILD.md,
use fresh run names. This verifies existing goldens without recording them:

```sh
python3 game/mugen/tests/reference_probe.py --fixture stageclear-one --name stageclear-one-check --fixture-stage --bank fvar --mugenversion 1.1 --clipboard-delay 0 --pages 0 --seconds 7
python3 game/mugen/tests/stage_clear_probe.py --fixture stageclear-one --native-run stageclear-one-check
python3 game/mugen/tests/app_smoke.py --name stageclear-one-go-check --reference game/mugen/temp/reference-runs/stageclear-one-check/stock --match chars/kfm/kfm.def chars/kfm/kfm.def --stage stages/probe-stage.def --motif data/mugen1/system.def --width 1280 --height 720 --seconds 5
python3 game/mugen/tests/stage_clear_probe.py --fixture stageclear-one --go-run stageclear-one-go-check
```

Native/app/test clients run sequentially. Explicit video overrides must match
the pinned profile when reproducing stage0/stretch cases.
