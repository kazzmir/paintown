# Current handoff

Use [PROGRESS.md](PROGRESS.md) as the single completion ledger and
[WORKFLOW.md](WORKFLOW.md) as the implementation/test policy. The user authorized
all remaining functional work on 2026-11-07, deferring pixel/color/shadow/alpha
and KO-echo precision until afterward. Continue autonomously with local
source/text checkpoints; no pushes, committed binaries or generated banks.

The latest batch connects streaming WAV/MP3/Ogg BGM to the shared production
mixer. Menu continuity, sample loop points, independent master/BGM/track volume,
selection overrides, stage metadata, storyboard inheritance/explicit stop,
round/scene clearing, match restart and helper-issued NoMusic are integrated.
StageVar now reads loaded name/displayname/author metadata through the World.
Legacy MIDI/tracker/CD audio backends remain explicit media/asset work; the
supplied stage references sound/kfm.mid, but that file is absent locally.

All five affected segments pass with race detection:21 top-level connected
scenarios across four packages. The new media scenario generates only a small
scratch WAV, removed automatically; existing assets/cache are reused. No clients
or segment scratch remain. Root modules are unchanged; three already-cached
Ebitengine decoder dependencies were added only to the scoped module overlay.

Next: team roster and simul/turns lifecycle with the four remaining trigger
bindings, then mode progression/AI, remaining media/assets/fonts and platforms.
Use substantial batches and one affected end-state check. Precision remains
partial/deferred; do not restart historical native matrices.

The supplied distribution is temp/mugen-1.1b1/. Use the isolated wrapper from
AGENTS.md for host-dependent runs, keeping them serial and cleaning scratch.
