# Helper defender contacts

M4an adds ten native helpercontact programs: 204 numeric observations and 40
CNS/AIR/CMD/stage hashes. The production Match runs the same inputs. These
controls cover ordinary one-on-one HitDefs against helpers, not full combat.

## Candidates, damage and targets

Combat now selects live opposing roots and helpers separately from Enemy and
NumEnemy queries, which remain root-only. Both normal and deprecated player
helpers take the measured ten damage, enter their own state 5000 and become
acquired targets under HitDef ID 7. Their first get-hit CNS sees Life=1000 and
pending damage 10; completed life is 990. Two normal helpers can both be hit by
one ordinary HitDef and receive distinct target identities. With a root and
helper overlapping, both become targets; the root ID precedes the helper ID.
An opposing helper takes damage while an overlapping same-team helper retains
Life=1000 under the default enemy-team rule.

The target ledger and hit guards key on EntityID, so helpers sharing an input
slot do not suppress one another. Helpers retain their own state/variables and
get-hit data. The separate root opponent list still drives enemy selectors and
root-only counts; combat does not turn helpers into extra NumEnemy entries.
Nested legacy projectile updates retain their prior opponent-list handling;
proper projectile contacts/lifecycle remain a separate gate.

## Birth timing and the misleading counter difference

Helpers can emit attacks after their birth pass, but the first incoming
contact in these controls occurs two world ticks after creation. A root and
its newly born helper do not first receive the overlapping attack together:
root contact is at GameTime 131, helper contact at 132 after birth at 130.
The latter resets the attacker's MoveHit/MoveContact clocks, producing 69 in
its GameTime-200 CNS rather than the initial implementation's 70.

Early/late helper-pair controls reverse which newly created helper attacks;
both first incoming hits are at 132. This distinguishes the measured defender
activation interval from an earlier-helper-only ordering rule. The simulation
tracks creation on the shared timeline and applies the incoming contact delay
without delaying outgoing attacks. Paused creation, unusual registration,
long-running clock edges and all birth variations need additional controls.

The counter difference initially suggested a negative-state phase problem.
Independent cases compare -3/-2/-1/positive reads of enemy Time, integer/float
variables and PlayerID values with either root attacking. They confirm the
existing complete per-entity attacker-first ordering; no negative-pass scheduler
change was made. Contact metadata is available before the receiver's negative
and positive passes, before pending life reduction. A separate first-contact
packet also confirms that GetHitVar(damage) clears after damage consumption;
that existing behavior required no change.

## Validation and remaining work

The pre-fix root-only defender failures are retained in
temp/m4an-before-contacts.log. Adding helper candidates fixes damage/state/target
selection but leaves the one-tick counter differences in m4an-helper-first-race.log.
The nine defender/phase/pair programs pass races in m4an-helper-second-race.log;
the tenth friendly-helper control passes m4an-friendly-race.log. Full scoped
races pass in m4an-full-race.log. Four fresh native replays reproduce all input
hashes and observations: m4an-replay-player, pair-late-attack, phase-p2-attack
and own-team.

Seven real 1280x720 captures under temp/app-runs/helpercontact-two-go/
(3/5/7 seconds) and helpercontact-stock-throw-go/ (2/3/4/7) pass provenance,
dimensions, nonblack and runtime/resource checks. Stock recovery retains two
fighters without permanent dust clones. The helper scene shows both victims;
its native/Go white clothing masks contain 19,993/19,692 pixels and differ at
1,507 pixels. This is a smoke check, not framebuffer parity. Existing mirrored/
clipped placement, native debug magenta versus Go black, background filtering
and HUD differences remain renderer obligations. Measurements:
temp/m4an-app-comparison.json.

Sources/goldens: tests/testdata/reference/helpercontact-*. Authors/catalogues:
temp/author-helper-contact*.py and helpercontact-*-cases.json. All programs use
version 1.1, a fixed camera, neutral input and PlayerPush=0 to isolate contact
from displacement. Sources query a missing optional helper/target in some
controls; their expected warnings/bottom writes are not missing-asset errors.

Still required: hitonce (including throw defaults), explicit friendly/both-team
HitDefs, helpers hitting more helpers in trades, cancellation/priority/reversal,
helper KO/destruction, pause/foreign/mixed-unit contacts, active AIR/size/Width
boundaries, and complete projectile and team-mode behavior. No whole Helper or
HitDef row is promoted to Verified.
