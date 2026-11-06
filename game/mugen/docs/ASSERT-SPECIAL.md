# AssertSpecial — native flags and global drawing

M4dh native study: all 19 documented literal flags and the undocumented `noko`
are accepted by the supplied beta. Quoted tokens, comma lists, numeric/unknown
flags and `noguard` reject. The primary flag is required even when flag2 exists;
flag2 validates; flag4 is ignored; case is insensitive. The 32 initial programs
include 24 accepted cases, eight rejections and 288 numeric values.

All original controller/state/AIR/CMD/stage intents were verified before native
observation. Session 76071 completed/awaited zero; a separate 2,352-original/
client guard passed. All 32 numeric/color/full-scene rows and four review sheets
were inspected before promotion. Every accepted case has post-Pause fv9=1,
proving that the asserting owner continues beyond the frozen snapshot.

Sixteen separately verified global lifetime/combination cases add 192 values.
Session 95040 completed/awaited zero; separate 2,484-original/client guard passed.
All 16 rows and two full-scene sheets were reviewed before promotion. Global
noBG, noFG and nobardisplay clear under frozen Pause and SuperPause owners.
One-shot assertions expire while the owner continues. NoBG clears black while
preserving the foreground. Front/under EnvColor and AllPalFX continue applying;
the noBG clear color receives both global palette passes. NoFG suppresses the
foreground above an under EnvColor. All 48 programs were promoted ONCE after
native-only review; sources, captures, config finalizations and goldens immutable.

Exact drawing comparisons use two new native-only consumer regions, chosen
before promotion: world [160,140,1120,670] and upper-right HUD [330,0,1120,140].
They include the actor, distinct foreground, stage and HUD portrait/name/bars/
timer while excluding the native diagnostic glyphs. Full original images are
retained; there is no image threshold or relaxed comparison. Other flags in
these scenes establish literal acceptance, not their unexercised consumers.

The Go baseline 32254 completed/awaited one; separate 18,844 main and 18,842
nested input/client guards passed. Eight individual rejection subcases fail,
plus their parent/package. All seven app comparisons fail. All seven baseline
pairs were reviewed before implementation.

The correction adds typed literal validation, including accepted undocumented
noko and ignored flag4; global rendering flags are reset each game tick,
including stopped owners. NoBG suppresses base stage drawing while retaining
foreground and palette passes; noFG suppresses the foreground. NoBarDisplay
suppresses bars, portraits, names and timer separately from round announcements.
The latter distinction still requires a dedicated native consumer observation.

Revision 74037 completed/awaited one; separate 18,845 main and 18,843 nested
input/client guards passed. 869 core race results and all 48 native replays
pass. All seven world regions compare exactly. Two complete multi-region crops
are exact; five retain strict upper HUD failures of 14,395 / 13,749 / 13,691 /
14,395 / 13,501 RGB pixels (maxima 251 / 254 / 251 / 251 / 216). The no-special-
flags baseline already has the first failure, before any flag implementation.
Native foreground occludes default HUD components, empty win-icon backgrounds
are absent in Go, and other stock HUD raster/layout differences remain. These
are actual open gates, not waived as unrelated work. Existing native-only
consumer regions and strict comparisons remain unchanged.

Final serial session 75098 completed/awaited **one**, solely for the strict
image matrix. All **40/40 world crops match exactly** across **20,352,000 RGB
pixels**. Across both fixed regions (24,776,000 compared pixels), **5/40 complete
crops are exact (12.5% of this corpus)** and **35 HUD failures remain UNWAIVED**,
13,501–14,395 differing pixels each. All 40 application runs succeed. Focused
races pass **869 core / 228 selected native results**. Five older palette/order
apps, four contact/fall masks and visually inspected stock win/rematch/win pass,
with **82 balanced inputs / 1,971,877 private PCM frames**. The other 23 final
gates exit zero; no partial matrix is called exact.

A separate elevated main+nested frozen-input/client guard passes after the
entire session: 18,846 + 18,843 paths / 37,689 hash checks. Passive audit verifies
18,847 unique final inputs, 2,464 immutable original inputs, all 48 exact native
configurations, 41 canonical resources, 1,968 native resource copies and all 109
manifest files. All 40 pairs/eight sheets, nine older images and three stock
captures were reviewed. Records: temp/m4dh-final-evidence-audit.json,
m4dh-final-visual-review.json and m4dh-final1-separate-guard.json. App SHA256:
`7706d01fed12b03fe3b73dc316cf8c3de5b03a680b0fff11eedb13044a56bed4`.
Latest full integration remains M4df 4,743 results / 36 packages at its preceding
source boundary. This checkpoint uses focused acceptance; no full rerun claimed.

Remaining dedicated consumers include guard posture, unguardable, walk/input,
KO prevention, timer/intro/end clocks and audio. NoMusic has no native BGM/audio
observation yet. Round-announcement behavior under nobardisplay is unmeasured.
Broader modes/AI/assets/BGM remain unallocated item 4. Shared renderer differences
from earlier checkpoints remain unwaived. New source inventory has 75 typed /
14 fallback routes (84.3% routing), two stubs, nine partial, 18 bounded and 60
route-only families; these are not whole-engine or behavioral percentages.
