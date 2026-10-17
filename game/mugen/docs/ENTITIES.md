# Entity identity, redirection and current ownership limits

M4x separates Character.InputSlot, EntityID, TeamSide and HelperID. The former
Target opponent list is named Opponents. A simulation-owned registry assigns
IDs and resolves live roots/helpers independently of keyboard slots. Match owns
the World; existing root phases and child tick adapters remain the state runner.
This is an identity foundation, not complete helper/projectile/target scheduling.

Nine stable native programs cover thirteen cases: 240 numeric observations and
one malformed-expression rejection. Production Character reproduces them all.
The numeric subset includes bottom behavior documented in BOTTOM.md. Two other
power programs retain unresolved native behavior separately and are not passing
Go conformance goldens. See REFERENCE.md and tests/README.md for reproduction.

## Measured identity and relationships

The effective HelperMax is clamped to 4..56. Root IDs start at that number:
stock roots are 56/57, the first helper is 58; limit 32 gives roots 32/33 and
helper 34. Configured 0 and 128 give roots 4/5 and 56/57 respectively. IDs are
script int32 values, with an allocator local to the match. Helper controller ID
777 is a separate lookup label, not its EntityID or input slot.

ID, TeamSide, IsHelper, NumEnemy, NumPartner, NumHelper and PlayerIDExist are
bound to this context. Enemy/EnemyNear and their zero index resolve the other
root in the one-on-one probes. EnemyNear(-1) resolves index zero; an out-of-range
opponent, absent partner/helper, negative PlayerID or Parent/Root on a root
produces bottom. Root on a root does not redirect to itself in this beta.
PlayerID(ID) resolves the live registry entry.

Helpers retain their real parent and original root. Their Parent/Root redirects
and ParentDist/RootDist use their own local coordinate/facing context. In the
basic case, both distances are -10, parent/root ID is 56, and their birth CNS
sees parent/root Time=1. This matches the existing child adapter after the
parent's tick clock increments. No complete global child phase order follows
from that observation.

NumHelper and Helper selectors use the root's helper family, including the
querying helper itself and nested descendants. A root, child and grandchild
all report two helpers in the nested fixture. Parent IDs are 56 then 58,
while both helpers' root is 56. The grandchild's first Helper selector finds
the earlier child, ID 58. Normal helpers do not add to NumEnemy/NumPartner.

DestroySelf unregisters a leaf helper, removes it from active child lists and
stops subsequent controllers that frame. The retirement fixture's following
ParentVarSet does not execute. NumHelper and PlayerIDExist report zero after
removal; recreation gets ID 59 rather than reusing 58. Root DestroySelf remains
a no-op. Complete recursive/orphan/audio/effect/binding destruction behavior is
still open.

## Integration and remaining work

Input and sound/pause owner identities remain independent of EntityID. HitGuard
now keys by EntityID, so shared input slots do not imply shared contact guards.
Match round reset retires helpers and rebinds persistent root IDs to fresh round
state; an authored test checks lookup cleanup and non-reuse. Native ID continuity
across rounds/matches still needs a dedicated trace.

Acquired hit targets use a separate ordered generation ledger, extending M4v's
defence ledger. HitDef ID is compiled as an integer expression without rewriting
shared source. Target/NumTarget bind to that ledger instead of the opponent list.
The native identity cases establish empty-target behavior only. Acquisition,
multiple/replaced HitDef IDs, guards, expiry/recovery/custom states, Target
controllers and full target ownership must be compared next. The current H/
generation retention rule is not a completed target schema.

Helper still uses a partial legacy parameter handler. Complete typed arguments,
postypes/facing/size/remap defaults, keyctrl/negative-state eligibility, input,
pause allowances and inheritance require native probes. Projectiles remain
legacy child adapters, not proper world player entities. They are excluded from
this registry's helper counts, but their full lifecycle is unfinished.
Team selectors have only one-on-one native evidence; multi-opponent ordering,
nearest-distance/KO/tie rules, neutral entities and team modes remain open.
Current EnemyNear ordering by horizontal distance is provisional outside those
fixtures. Names and state/animation ownership are separate remaining bindings.

Full scoped races and pure engine/evaluator races pass. Fresh limit32, nested,
operator and malformed-expression replays match their native packets/rejection.
No whole controller or trigger is promoted to Verified by this checkpoint.
