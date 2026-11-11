# State-machine test segments

The normal test interface is five connected scenario suites, selected from
existing tests. This consolidates execution without moving production packages
or duplicating their test bodies and data.

| Segment | Connected behavior |
| --- | --- |
| state-flow | State entry, negative levels, chained transitions, persistence and loop/missing-state diagnostics. |
| combat | Logical input, attacks, contact/damage continuation, jump and authored landing recovery. |
| entities | Helper creation, binding/retirement, orphan-effect expiry and round identity cleanup. |
| effects-pause | Pause/SuperPause across actor/background/effect clocks, power/resources and reset. |
| match-flow | Title/selection, stock input-driven fights, KO/time-over, rematch/exit, teams, modes, authored CMD/AI and storyboard/music asset lifecycles. |

[PROGRESS.md](../docs/PROGRESS.md) is the only completion ledger. Completed suites
stay checked and are omitted from normal runs. The runner compares a compact
fingerprint of each suite's covered source with its last passing version; a
related change reopens that suite. Documentation edits do not reopen gameplay.

```bash
game/mugen/mugen.sh --check --list
game/mugen/mugen.sh --check --plan
game/mugen/mugen.sh --check                      # pending only
game/mugen/mugen.sh --check --segment combat     # explicit targeted rerun
game/mugen/mugen.sh --acceptance                 # explicit match-flow end state
```

If a reported regression reopens a completed segment without a source change:

```bash
game/mugen/mugen.sh --check --reopen combat --reason 'sweep recovery regression'
```

The small feature catalog maps these suites to their existing test functions;
--plan shows the exact selection. A checked scenario records acceptance for its covered source version, not
completion of the broader partial features in the project ledger.

Run affected segments after a coherent implementation batch. A successful run
updates its checkmark and source fingerprint; failed or incomplete suites remain
pending. Other completed suites are not rerun. Scratch is removed after success
and failure; assets are read in place. Full legacy diagnostics are --check-all
only and may require separately supplied diagnostic inputs.

Future scenario extensions should span a meaningful state path, such as attack
→ hit pause → knockdown → recovery. Fold related assertions into that scenario;
do not add parameter permutations or a new documentation page for each field.
