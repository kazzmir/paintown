# Current handoff

Follow [AGENTS.md](../AGENTS.md). Author only game/mugen/; preserve unrelated
work, external assets and user YAML. Local source/text checkpoints are authorized;
no pushes, binaries, fixture banks, subagents or goals. Keep documentation portable.
[PROGRESS.md](PROGRESS.md) is the single completion ledger.

K1-K10 are closed for the bounded implementation scope. U1 runtime, U2 supported
font/content, U3 Options/Training, U4 AI and U5 precision acceptance pass. The
ledger has 34 implemented categories and one partial category; those counts are
not whole-engine equivalence percentages. The implementation phase is closed;
continue with maintenance and content compatibility from demonstrated issues.
H1 physical unplug/reconnect is a non-blocking manual test note.

## Input maintenance checkpoint

Keyboard/gamepad editing now shares the Options motif background, heading style
and small utility lettering, including capture hints and reset confirmation.
Bindings fit below the heading using window-sized Options layout. Capture has a
four-second wall-clock timeout. F4 tests raw signals and device-specific actions;
F6 raw capture bypasses the standard mapping, including omitted raw axes.
F5 saves both players' inputs into the active YAML without replacing other
settings/comments; the complete prior file is backed up before atomic replacement.
Leaving changed inputs offers Save, Session only (retain live edits), or Back.
Main Options Load/Save still uses memory snapshots. YAML/backups are ignored.
The user verified setup, selection and combat with an 8BitDo controller; another
detected standard-layout controller supplies no captured presses in the reported
session. Physical diagnosis remains open; the tester separates missing signals
from legacy-binding mismatches. Do not infer a hardware fix from injected devices.
The affected combat/match-flow segments pass with race detection; other segments
stay checked. The connected edit/test/session/save/reload path verifies complete
backups, preserved unrelated settings/comments/anchors and safe validation/backup
failures. Linux build and themed countdown/test/save-dialog inspection pass.
Owned overlay, captures and binaries are removed; shared assets/cache remain.

Profile investigation confirms the pinned SDK already embeds platform-specific
SDL controller mappings. Its [mapping API](https://pkg.go.dev/github.com/hajimehoshi/ebiten/v2#UpdateStandardGamepadLayoutMappings)
accepts additional GUID/platform entries from the public
[SDL database](https://github.com/mdqinc/SDL_GameControllerDB). The next profile
step is an opt-in SDK-default binding preset and optional local mapping overrides;
preserve legacy CFG precedence until explicitly changed. No extra database or
automatic profile replacement is added in this checkpoint.

## AI closure

K8/U4 now connect the complete scoped AI path. Controller-scoped successful
command reads attribute actual root transitions. Separate bounded ground/air
banks collect geometry, entry contexts and startup/active/control/end statistics;
timing groups are whiff/guard/hit outcomes. Actor hitpause and shared pause stop
learning time. Compatible optional beta learned-data files seed the banks.

Opponent recognition feeds predictive held guard, startup/range/reversal counter
selection, source-pose preparation and walk/jump waypoints through normal CMD/CNS.
Prediction can interrupt navigation; counters are selected from predictive guard.
Physical learned plans own their attempt until completion or an actual hit;
fallback defence cannot cancel them. Waypoints retain their starting axes and
moving jumps finish on horizontal passage, preventing repeated jump loops.

Completed observations survive rounds/rematches/reload in bounded session memory.
Live actors have independent banks; definition/source revisions, language and
game width isolate profiles. Independent seat random streams, timed non-cheating
plans and session-only retention are explicit port policies. No automatic binary
cache writes or identical native random sequences are claimed.

Target handler/table review and direct operation reach all five decision contexts.
The existing connected AI/combat journey covers human whiff/air/hit, guarded
contact, native timing/smoothing/hitpause, cache decoding, physical and admitted
counters through contact/recovery, walking/jumping, helper/human ownership and
round/reload retention. All five segments pass with race detection for current
source. Linux rebuild and live two-CPU normal/special combat pass with damage and
advancing clocks. Owned runtime prefixes, overlays, captures and binaries are
removed; shared cache and external content remain. No new binary fixtures or
user configuration are committed.

## Accepted rendering scope and followups

K10 connects Linux FreeType automatic hinting and true monochrome masks through
the existing font consumer. Bitmap bearings preserve trimmed-border placement;
native faces borrow pinned bytes and have per-face synchronization/cleanup. An
unavailable backend reports the portable outline fallback. No new Go dependency
or FreeType development headers are needed. See BUILD.md for runtime details.

The user accepted the demonstrated U5 edge differences on 2026-11-24. Shared
native/Go regular/italic Unicode
captures have identical metrics and all ten text-block bounds/positions. Blended
coverage has mean absolute ink error 0.687/255 (isolated maximum 127); solid masks
differ at 23 of 3106 ink pixels. Differences lie within two pixels of the reference
silhouette. The existing asset/menu/fight/reload segment passes with races, font
vet is clean, and the final Linux build reproduces the shared capture exactly.
Normal-match smoke passes. U5 is closed under the accepted rendering policy;
preserve the checked metrics/placement and reopen for a demonstrated regression.
Portable outline fallback and other-platform precision remain outside that
acceptance. Keep earlier layout, camera/sprite/alpha/KO-echo, U2 format acceptance
and K9 palette mapping/reservation closed. Review scratch is removed after acceptance.

H1 now requires only physical unplug/reconnect testing, including retained player
ownership and released input after disconnect. Leave this unverified note open
while maintenance and compatibility work proceeds. Physical CD, unavailable
licensed/CJK payloads, other-platform runtime acceptance and standalone music CLI
remain deferred. Unknown permutations are not matrices, and scripts that fail
natively do not need engine workarounds.

Validate affected connected segments at coherent batch boundaries; completed
segments remain checked until covered source changes. Keep native/application
runs serial and await owned clients before edits or another run. Verify writable
overlay leaves resolve inside owned scratch, including their ancestor directories.
Clean only owned output on success/failure and retain the shared build cache.
