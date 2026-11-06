# Clipboard controller closure plan

This is preparation for item 3, not additional native coverage. Finish the active
MakeDust integration and its separate frozen-input/client guards before authoring
or running the new probes.

## Existing gaps

DisplayToClipboard and AppendToClipboard currently forward raw `text`, including
its source quotes. They do not compile/evaluate `params` or format expressions.
Character storage overwrites on Display, prepends a newline on every Append, and
clears on ClearClipboard. The production fight renderer does not display that
buffer. Config already loads Debug, AllowDebugMode and AllowDebugKeys.

The supplied beta manual describes six numeric parameters, signed integer and
floating conversions, escaped newlines/tabs/backslashes/quotes, literal percent,
and default formatting on a type mismatch. These are authoring guidance until
native controls establish the exact acceptance, evaluation and visible behavior.
Static executable guidance also identifies six parameter slots. Do not implement
from the manual or transfer another controller's integer schema by assumption.

## Observer boundary

The existing reference recorder injects Display/Append numeric pages into CNS or
CMD and then decodes those pages. It would overwrite the clipboard being tested.
Keep that recorder and its numeric decoder unchanged. Add a separately named
recorder that retains the exact authored clipboard controllers and captures the
owned reference window and load errors without injecting another clipboard.

The bundled `font/f-4x6.fnt` has 94 mapped printable characters and **68 distinct
bitmap masks**. Each uppercase/lowercase letter pair shares a mask. This was
verified from its original map and PCX bitmap during read-only MakeDust validation;
it is a resource fact, not new controller coverage. A full glyph observer must
preserve equivalence classes and positions rather than pretending to distinguish
case from pixels. Spaces, tabs, line starts, blank lines and clipping require
geometry observations. Pin the original font hash. Do not add binary font assets.

Use exact numeric payloads and full glyph-mask/position observations, with
original PNGs retained for independent review. Compare production debug drawing
against the native clipboard region; changing frame/FPS diagnostics must not be
mistaken for clipboard differences. Select that region from native-only evidence
before the first Go baseline. Do not relax old comparators or repurpose their
numeric pages.

## Native experiments before implementation

| Group | Required distinctions |
| --- | --- |
| Buffer | Once-only Display and delayed capture; repeated Display; Append on empty/nonempty buffers; explicit trailing newline plus Append; Clear; state transitions and round reset. |
| Schema | Missing/empty/quoted/unquoted text; omitted/empty/extra params; zero through seven params; ignored fields; Clear's ignored fields; actual source-context load errors. |
| Formatting | `%d`, `%i`, floating conversions and literal `%%`; signed values; width/precision/flags; escapes; integer/float mismatch defaults; unsupported conversion and length tokens. |
| Evaluation | Whole parsed controller intent; ordered assignment/Random sentinels; unused versus required expressions; bottom/nonfinite values; fixed six-slot limits. |
| Ownership/clocks | Roots, helper and selected debug actor; same-pass writers; Pause/SuperPause and movement allowance; buffer retention while frozen. |
| Drawing | Real debug-enabled production window; stock font metrics, tab stops, newline/blank lines, clipping and physical-window scaling; debug toggle/selection where measured. |

Separate invalid setups, unreached observers and renderer ambiguity from actual
controller findings. Review every native-only source/numeric/glyph/geometry/error
row and image before promoting fixtures or implementing Go behavior. Freeze the
baseline, retain failures, then add typed formatting, measured storage semantics
and a real production renderer. Validate the new corpus and old effect/stock
input/private-audio gates; document bounds and commit source/text only.


## Deterministic observer research

The supplied `docs/readme.txt` hotkey table documents Pause and Scroll Lock
frame stepping; `docs/mconsole.html` also documents `pause` and `framestep`.
This is native-tool design guidance, not an exercised recorder yet. A separately
named owned-window recorder should verify those controls against observed game
counters before using them to inspect exact animation/formatting boundaries.
Keep all existing recorders and expectations unchanged. Engine debug pause is
distinct from the CNS Pause controller, which intentionally leaves some clocks
running. This may avoid relying on wall-clock capture timing for future probes.

In one existing immutable numeric capture, the P0 and P6 prefix glyphs occur at
(10,706) and (10,712) in a 1280x720 window. Both authored text formats end in a
newline. That example has no intervening blank glyph row; it does not by itself
determine Append's behavior for empty buffers or strings without trailing newlines.
