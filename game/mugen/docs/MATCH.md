# Connected one-on-one match

The normal launcher now reaches a real FightState through title, character
selection, stage selection and versus. Confirm both players in versus, then
choose a stage with P1 left/right and confirm. Start confirms a character;
a/b/c/x/y/z also select palette numbers 1..6, with Start held selecting 7..12.
Palette numbers are retained and exposed as PalNo; rendering the chosen palette
is still pending. Character names, DEFs, selected stage, mode and music override
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
the physical adapter is the default for old diagnostic applications. The legacy
Character tick still processes players sequentially, including collision and
push, and Target still means both opponent and acquired target. These must be
replaced with reference-backed world phases/ownership rather than duplicated
inside another fight engine.

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
fixes before calling the full M4 milestone playable. Character/
common sounds, fight announcement sounds, BGM and common hit effects are not
consumed yet.

Validation: full regression and race checks pass. Match tests cover stock mixed
spawn/intros/control, KO/round reset/two-win completion, life-fraction time-over,
and actual logical walk/punch/hit/recovery. Lifecycle tests set life at phase
boundaries to isolate KO/time-over; they do not prove complete combat. Production
flow tests inject stock keyboard bindings through title/select/stage/versus/fight,
complete a result, rematch and exit. Owned-window captures under
`temp/app-runs/first-mixed-fight/` show equal KFM/KFM720 sizes and exposed a
portrait scaling mismatch, corrected in the following capture. No native match,
round timing, audio or screenshot parity is claimed.
