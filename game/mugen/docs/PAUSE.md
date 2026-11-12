# Shared timeline and Pause

M4u adds typed Pause parameters, a shared simulation Timeline, separate pause
actor identities and a real GameTime binding. Thirteen native programs pin 264
float observations from two roots; Character reproduces every packet. Pure
timeline tests also use these native elapsed-clock observations without a
display. This is root Pause coverage, not complete pause/world compatibility.

## Measured rules

The issuing tick finishes normally. Pause time=3 with default movetime freezes
the following three ticks. Time=1 first runs at relative GameTime=4, while its
position and animation show only one completed local tick. GameTime continues
through the pause. Negative/positive CNS and ignorehitpause controllers do not
run while world-frozen; visit counts remain one. Hitpause exemptions are a
different mechanism.

With movetime=1, the caller runs Time=1 at GameTime=1, then waits until GameTime=4
to run Time=2. Movetime equal to the duration allows continuous caller updates.
Omitted Pause time is accepted as zero despite the manual calling it required.
Negative time acts as zero. Movetime below zero or above time falls back to zero;
it does not clamp to the duration. Time zero has no subsequent frozen ticks.

The other root can issue the pause with the same measured result. A replacement
issued by an allowed-moving caller replaces the old duration. A zero-duration
replacement cancels it. If both roots issue a Pause in the same tick, the later
root's request wins in the measured stable idle-root order. These requests take
effect after the issuing frame, preserving its CNS/physics completion.

## Runtime integration and evidence limits

Timeline advances GameTime once per outer tick; nested helper/projectile
adapters share that frame. Movement allowance belongs to an identity distinct
from input slots and script EntityID. Frozen roots skip CNS, local clocks, physics and timed
defence aging. M4v corrects the initial contact assumption: a frozen attack box
can hit a moving defender, and a moving attacker can hit a frozen defender.
Only pairs with both actors frozen skip collision. A frozen receiver enters
the hit state immediately, but its CNS and pending damage wait for resumption.
See SUPERPAUSE.md for the native traces and remaining entity/overlap limits.

Match uses the same Timeline across round resets, clears pause requests when
new round actors replace old ones, freezes phase/timer/default contact-effect
clocks during a pause, and honors pausebg for stage updates. Authored production
Match tests check player/global/timer/background/effect clocks and reset. These
background/effect/round lifecycle checks are documentation-based; the 264 native
observations establish player/global clocks, not full native presentation parity.
Absolute GameTime origin across menus/matches/rounds remains unverified.

Endcmdbuftime collects matching commands in the final buffer window and makes
them visible on the first resumed CNS pass. Authored tests exercise a button
pressed/released during that window versus buffering disabled. The native harness
currently supplies no input events, so command grammar, release/hold timing,
buffer lifetimes and out-of-range buffer values still need native coverage.
Those tests do not promote the full controller to Verified.

The separate superpause-three fixture began as a pure Timeline pilot. M4v now
replays it through production Character alongside the typed SuperPause controller,
preemption, protection, target defence, power and presentation probes. See
[SUPERPAUSE.md](SUPERPAUSE.md); the pilot no longer describes a production stub.

Validation: full scoped races and display-independent engine races pass; fresh
pause-pair replay reproduces both native packets. Source/AIR/executable hashes
are pinned. The original reference, root module files and desktop configuration
remain unchanged.
