# Current handoff

Use [PROGRESS.md](PROGRESS.md) as the single completion ledger and
[WORKFLOW.md](WORKFLOW.md) as the implementation/test policy. User resumed work
after successful stage and tracker-music playtesting. Linux remains the current
target. Local source/text commits only; no pushes, binaries or generated banks.
All work stays inside game/mugen/. Completed segments stay out of normal runs.

Latest compatibility/color batch closes legacy ACT loading: honor DEF pal1..pal12
with Windows/case-aware resolution and contextual errors, retain private PCX
colors and shared/linked sprite identity, select an outfit before first CNS and
restore it on round reset/turns entry. Benched members receive their outfit too.
Modern1.x characters, including V1 sprite users, retain authored
state5900/RemapPal per the beta upgrading guide. Missing V1 outfits use pal.defaults then
an available bank; unrelated directory ACTs are no longer guessed.

One original1.1b1 control rendered two declared outfits. It established default
V1 HUD portraits versus selected body colors: top crop1152 default/0 alternate
pixels; body576 default/576 alternate. The implementation distinguishes initial
legacy selection from explicit RemapPal and preserves that state in helper
clones. Control preparation needed valid native CNS constants/CMD state blocks
and SFF1.0.1 metadata; Go's exact invalid-content rejection parity is unproven.
No native PNG/PCM/overlay or authored fixture bank is retained.

All five connected segments pass with race detection; the Linux standalone
executable was rebuilt after final review. The existing asset flow
now spans V1 outfit/portrait masking, shared/own helper palettes, explicit remap
and actual KO→round reset. The music scenario now accepts the user's blank track
and optional stage author metadata; those content differences were not runtime
regressions. State-flow stayed checked. Only one shared cache/ignored Linux
executable is retained; scratch/client cleanup passed.

Local content: active data/select.def lists ten stages. Its music/ symlink points
to existing data-new/mugen/music; deejay.def uses music/tr3deejay.it. User confirms
tracker playback. Those local assets/configs stay ignored; originals elsewhere
were not changed. Several other stage MP3 names remain absent.

Next: turns/survival recovery amount/rounding and remaining native rendering/
audio calibration. Healing still uses the explicitly provisional time-scaled
27.5%-of-max baseline. Native AI decision policy, broader legacy palette/control
combinations and strict invalid-asset rejection are not certified. Physical CD
playback and other platform validation remain open/deferred; licensed Windows
font payloads are absent. Reuse supplied assets in place and the isolated serial
host wrapper from AGENTS.md. Never regenerate the removed precision matrices.
