# Remaining implementation plan

Track completion only in [PROGRESS.md](PROGRESS.md). The user authorized the
remaining functional work on 2026-11-07 and deferred presentation/audio precision.
Keep implementation-led batches, connected end-state checks and local commits.

| Batch | Deliverable | End-state scenario |
| --- | --- | --- |
| Music and stage context | WAV/MP3/Ogg BGM, NoMusic, scene transitions and StageVar. | Menu → fight/helper assertion → resume/rematch → storyboard inheritance/stop. |
| Teams and context | Simul/turns rosters, partner lifecycle, team outcomes and remaining team triggers. | Team selection → multi-root combat or roster handoff → results/reset. |
| Modes and AI | Arcade/survival progression, training/watch/co-op rules and authored AI command flow. | Selection → ordered opponents → continue/ending or survival reset. |
| Assets and platforms | Remaining font/media/resource compatibility and supported builds. | Representative complete game flow using the supported content. |
| Deferred precision | Remaining color/shadow/alpha and KO-echo playback matching. | Allocate after functional coverage, without historical matrices. |

Completed scenarios stay complete until related source changes or an actual
regression reopens them. Generated media/captures stay out of Git; reuse assets
and the shared cache, clean scratch on success and failure, and use Git history
for detailed checkpoint history.
