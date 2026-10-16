# Connected one-on-one match

The normal launcher now reaches a real FightState through title, character
selection, stage selection and versus. Confirm both players in versus, then
choose a stage with P1 left/right and confirm. Start confirms a character;
a/b/c/x/y/z also select palette numbers 1..6, with Start held selecting 7..12.
Palette numbers are retained and exposed as PalNo. Stock SFF v2 characters now
apply them through common CNS RemapPal; external SFF v1 palN selection still
needs its own resource/default fixtures. Character names, DEFs, selected stage, mode and music override
survive the handoff. Music playback is not implemented yet.

Stock keyboard controls: P1 arrows, comma/period/slash, L/semicolon/quote,
Enter; P2 W/S/A/D, F/G/H, R/T/Y, U. At match result, P1 Start rematches and
Escape returns to title. Escape also returns to title during a fight. Team,
arcade progression, training options, watch and survival rules remain M7 work;
the current match is one player per side. Single-player selection uses a basic
opponent policy through logical command inputs, not a reference-equivalent AI.

A direct production match can be started with:

```
GOPROXY=off game/mugen/mugen --game --data-dir game/mugen/temp/mugen-1.1b1 --quick-match chars/kfm/kfm.def,chars/kfm720/kfm720.def --stage stages/kfm.def
```

The Go runtime reads the reference resources; executable reference probes and
smoke captures always make a disposable copy first. App captures can be repeated:

```
python3 game/mugen/tests/app_smoke.py --name UNIQUE_NAME --motif data/mugen1/system.def --match chars/kfm/kfm.def chars/kfm720/kfm720.def --seconds 2 6
```

`match.Match` is shared by production and tests. It owns the roster, stage,
fight definition, phase, round, wins, timer and camera. Script environments retain
their local coordinates. Spawn/bounds/camera values translate explicitly.
Character input accepts a logical source without polling the keyboard first;
the physical adapter is the default for old diagnostic applications. The production root tick runs all state passes before collecting contacts,
then snapshots eligible hits, enters/processes get-hit positive states before
damage, and advances physics/time. This supports equal-priority trades and
matches focused pre/post-damage and newly-started hitpause probes. Negative
states run once; engine transitions continue through the same positive executor.
The character diagnostic uses the same group tick. Isolated Character.Update
retains a single-character adapter over the shared state/physics methods.

Target still conflates opponent and acquired target, and entity identity,
priority/guard/overrides, multi-contact rules, helper/projectile and push/physics
phases remain incomplete. Match tests open no window but Character/asset imports
still initialize Ebitengine; fully display-independent extraction is pending.

Current round phases cover pre-intro, Intro assertions, round/fight announcements,
control grant, active timer, pre-over, win/lose poses, round reset and match
result. CNS/common states run through the same executor. Reset characters keep
power and configured persistent variables, while helpers/effects/commands/timers
are fresh. Native timing, max draw games, ResetBG and complete round persistence
rules need further comparison. Round settings are read from fight.def but their
exact offsets/defaults and interaction are not yet native-verified.

The renderer uses actual fight SFF/AIR/fonts for bars, faces, names, timer,
announcements and win icons. Stage aspect cropping uses the declared high/low
cut suggestions. Mixed character and portrait sizes convert to the drawing
space. HUD alpha/scale/angle/interpolation, delayed life bars, color/font-height
variants, shadows, zoom, tall-stage overdraw and precise native camera framing
remain incomplete. Guard state ownership/recovery and movement Ctrl gates now match focused native
fixtures. Received hit vectors/defaults now convert across spaces with focused native
coverage. Complete guard/combat/default schemas and phase ordering remain next
fixes before calling the full M4 milestone playable. Character/common PlaySnd and fight announcements now reach the production audio
stream (M4m). Contact hit/guard/KO voice cues, BGM and common hit effects remain
open; scene/round cleanup is provisional. See AUDIO.md.

Validation: full regression and race checks pass. Match tests cover stock mixed
spawn/intros/control, KO/round reset/two-win completion, life-fraction time-over,
and actual logical walk/punch/hit/recovery. Lifecycle tests set life at phase
boundaries to isolate KO/time-over; they do not prove complete combat. Production
flow tests inject stock keyboard bindings through title/select/stage/versus/fight,
complete a result, rematch and exit. Owned-window captures under
`temp/app-runs/first-mixed-fight/` show equal KFM/KFM720 sizes and exposed a
portrait scaling mismatch, corrected in the following capture. No native match,
round timing or screenshot parity is claimed. Focused native sound behavior is
recorded separately in AUDIO.md.

HitOverride now closes the stock compile gap: all 108 states compile per KFM
variant. Native-backed slots/expiry/forceair/basic entry and damage behavior work;
custom-state exclusions, invalid fallbacks and complete ownership/projectile
rules remain pending. Standard get-hit entry selects 5000/5010/5020 from hit
context. Override entry retains that intermediate state in PrevStateNo, and air
hit data stays available even when the destination Statedef changes state type.
This remains partial combat compatibility, not the full M4/M5 gates.
