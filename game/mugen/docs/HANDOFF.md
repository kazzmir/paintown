# Current handoff

Follow [AGENTS.md](../AGENTS.md). Author only game/mugen/ and preserve unrelated
work. Local source/text checkpoints are authorized; no pushes, committed game
assets, binaries, user configuration, subagents or goals. Keep documentation
portable and use [PROGRESS.md](PROGRESS.md) as the single completion ledger.

The latest compatibility batch covers deferred storyboard admission, victory
portrait windows, font RGB across menus/VS/HUD/results, inset quote wrapping,
fades, result-screen settings, continuing completed-fight poses and automatic
campaign stage assignment. Connected scenarios cover continue, localized
victory, ending/no-ending completion, Survival results and co-op handoff. Linux
build and the affected race-checked segments pass; see the ledger for scope.

Keep RGB adjustment separate from font-bank selection and shared glyph caches.
Victory clipping uses full-canvas scaling; wrapping uses available aligned
space. Result poses advance clocks without changing the committed score, round
or timer. Campaigns can use assigned stages absent from the manual stage pool.
Unused character and chained motif storyboards resolve only when played.

Earlier compatibility work covers legacy definition precedence, mixed coordinate
systems, portrait requests, palette alpha, sparse HUD backgrounds, legacy
projectile syntax and zero-duration empty effects. Preserve strict fighter-loop
diagnostics while empty effects retire on their first pass. Options remains
usable without title-menu fonts, and input edits remain session-only. Debug
activation persists across round restart/reload without changing actor identity.

The project retains three broad partial areas: arbitrary content/font/media
acceptance, native runtime/render/audio/AI/menu fidelity, and physical controller
acceptance. Concrete result consumers still needing work include text layering
and Survival pose-threshold policy. Physical CD, unavailable licensed fonts,
other-platform runtime validation and standalone music CLI playback are deferred.

Use target documentation and an independently obtained reference runtime for
normative behavior. Bundled runtimes may differ despite a matching version
banner. Describe observed compatibility patterns without naming local content
or recording private asset inventories. Do not repair more third-party scripts.

Use the scoped module overlay, reuse external data and the build cache, and
validate affected connected scenarios after a coherent batch. Native, display
and audio runs stay serial; await owned clients before edits or another run.
Discover the current environment rather than documenting a private machine's
paths, processes, server addresses or authentication files. Clean owned scratch
and preserve external data and tools.
