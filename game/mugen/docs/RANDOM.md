# Random stream and VarRandom (M4cd)

Predecessor/reversal: 750f0076. Target: the supplied Elecbyte M.U.G.E.N
1.1 beta 1 executable and its Random/VarRandom manual entries.

The beta uses a Park–Miller stream: `state = state * 16807 % 2147483647`.
Read-only examination of the immutable executable identified separate session
and match states; 21 independently seeded native controls establish the actual
CNS draw sequence. This is not the older Paintown C++ RNG or Go's math/rand.
`Random` returns a typed integer, `next() % 1000`, from 0 through 999. The first
12 observations of randomdraw-sequence match the first 12 draws from the logged
Match RNG seed, without a fitted/skipped prefix.

Roots, helpers, Enemy/Root/Parent redirects and VarRandom consume the same match
stream. A missing redirect returns bottom without a draw. The helper control
observes four root draws, four helper/redirect draws in its creation pass, then
four root draws on the following pass. Existing eager IfElse and ordinary
operators consume both operands/branches; Cond evaluates only its selected
branch. In particular, `0 && Random` and `1 || Random` still draw. This checkpoint
does not alter that already measured evaluator behavior.

VarRandom compiles required `v` and optional `range` expressions. Defaults are
0,1000 inclusive, unlike the Random trigger's maximum 999. A single range value
means 0,value. Reversed endpoints swap. Singleton ranges still draw. Integer
fields reject float literals at load; evaluated floats truncate with warnings.
Bottom becomes zero. Indices below zero or above 59 do nothing, without
evaluating either range endpoint or consuming RNG. Valid evaluation order is
index, low endpoint, high endpoint, draw. Malformed tuples, trailing commas,
extra components and missing v reject with source/state/controller context.
The legacy Environment entry point uses the same compiler.

World owns the match stream and preserves it when replacing roots for a new
round. It does not share state with other matches/worlds. Production Engine owns
a separate session stream, seeded at startup from Unix seconds, and passes its
next value into each new match, including rematches. `match.Options.RandomSeed`
allows explicit deterministic replay; zero chooses a session-derived seed for
standalone callers. Bare state-test Worlds default to seed 1. `World.RandomSeed`
reports the initial match seed, not its changing state. TargetDrop's existing
random choice now draws from this same stream; its exact native selection/draw
order remains an explicit interaction gate rather than a newly proven claim.

## Evidence and validation

The immutable randomdraw-* CNS/CMD/AIR/stage originals were frozen before
observation: 84 authored hashes, 16 accepted controls / 192 numeric observations
and five load rejections. JSON captures are copied unchanged from native output;
added seed/log-hash metadata comes from each run's mugen.log. Native PNG hashes
and exact bitmap-font decoding are independently checked. The Go harness seeds
World from that log before replaying the same authored program. No numeric
golden is recomputed by the implementation.

Controls cover raw sequence, redirects, expression evaluation, default/single/
signed/reversed/equal ranges, runtime floats, bottom index/endpoint, invalid
indices, side effects and malformed inputs. Character tests check world isolation
and round replacement. The actual title/selection/fight/round/result/rematch test
checks deterministic session-seed handoff through production construction.
Engine tests check widened multiplication and seed normalization boundaries.

Core race checks pass: engine 1.462s, evaluator 1.022s, Character 11.401s,
System 9.575s. Native Random + HitPauseTime Match replays pass; final native,
projectile/juggle regression and playable-app provenance results are recorded
in HANDOFF.md. The last entire scoped race suite is M4cb; the allocated final
projectile/reversal integration will run it again.

Broader seed consumers in menus/modes/AI, rollback/network synchronization,
extreme overflowing range arithmetic and all simultaneous pause/foreign/team
combinations remain unverified. The generator accepts an int64 seed API and
normalizes out-of-range seeds to 1; zero-as-time is handled by session callers.
Wide range subtraction is safe in Go; full int32-span native behavior is not
claimed. The source-routing inventory is 111/127 families (87.4%), with 16
unbound; this is not a whole-engine completion percentage.
