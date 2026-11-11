# AssertSpecial noko and retained death state

M4dj studies the supplied beta's accepted undocumented `noko` flag. It can retain
Life0/Alive1 without clamping life to one. Death is latched: a late assertion or
restoring positive life does not revive an actor already marked dead.

The current assertion takes effect in entity execution order. An earlier root
can protect a later root's direct LifeSet0, including without any contact. An
assertion by the later root cannot undo the earlier root's self-KO. Alive remains
latched during an actor's CNS pass and its new death state becomes visible after
that entity completes CNS and pending damage.

Valid Pause/SuperPause controls use time60/movetime60. They record the frozen
target's Time as2 before pause,5 during pause and5 at the later frozen snapshot;
the post-resumption snapshot records16. A frozen actor retains its life/death
state. Its previous assertion does not protect the moving owner in a later
frame. A moving owner can self-KO and cause RoundState3 while the other root
remains frozen. An ongoing owner assertion protects the owner and retains
RoundState2. Ordinary phase/timer freeze and KO detection have distinct gates.

Six explicit state-entry controls distinguish self-KO from later contacts.
Unprotected direct LifeSet0 enters missing state5030 while retaining the prior
S/I attributes, zero velocity and fall0. An active earlier/own noko prevents
that automatic entry. A later HitDef can contact an already dead root during
damage grace: it enters state5000 from5030 and executes a distinctive entry
marker, applies fall1 and velocityY-6/positionY-24, while remaining Alive0 and
gaining no power. Protected zero-life targets also receive the lethal reaction
and metadata while remaining Alive1. No guessed self-KO fall or forced H type
was added; the initial contact fixture's fall1 comes from that later contact.

## Evidence and implementation boundary

All complete source/header/controller/parameter/trigger/AIR/CMD/stage intents
were verified before native execution. Every numeric, clock, contact, life,
Alive, round and color/full-scene row was reviewed before one-time promotion.
Four disjoint ledgers pin44 accepted programs/528 values/44 exact native
configuration recipes: contact8, lifetime22, valid frozen-owner8 and entry6.
Thirty separate fixed world observers use native-only region[160,140,1120,670],
chosen before Go baseline/promotion. Native images and all older regions remain
unchanged. No threshold or sampling change is involved.

The original14 GameTime-based programs remain unpromoted diagnostics: exact
GameTime6/10/12 events could be skipped under KO slowdown or Pause. Their zero
slots were not accepted as observations. The counter study uses actual P1
negative-state execution counts. Four of its controls use an out-of-range
60/99999 movement budget, which normalizes to zero; those samples occur after
the owner resumes. This scope was corrected explicitly and separately measured
with eight60/60 controls. Every original source, failed intent reconstruction,
configuration, capture and interpretation correction remains available in temp.

The first Go baseline completes with failures in30/38 numeric programs and
five of six application crops. A separate entry baseline fails all six added
programs. Every failure and all six image pairs were reviewed before behavior
changes. The implementation adds a current-frame NoKO timeline bit, a retained
per-actor death latch, completion after each actor's CNS/damage, and a paused KO
exception to Match's ordinary clock freeze. It preserves eligible root corpse
reactions during damage grace and suppresses power credit for dead receivers.
The measured zero-life contact still receives lethal metadata and knockback.

Revision92316 completes/awaits zero:869 core and48 native test results pass,
including all44 programs. All six revised application crops are exact across
3,052,800 RGB pixels and visually reviewed. Separate19444/19442 frozen-input/
client guards pass. Final32712 completes/awaits zero with all23 gates passing:

- Full scoped races:4,918 results/36 packages (25 tested,11 compiled-only),
  no individual skips or race failures; Match2,294.916 seconds.
- All30 new world crops exact/15,264,000 RGB pixels; all30 pairs/six sheets viewed.
- Five older palette/order applications and four shake masks pass and are viewed.
- Stock21s win,26s rematch/FIGHT,45s second win viewed;82 balanced keys and
  1,973,087 private PCM frames with nonzero peak/RMS.

The passive auditor completes/awaits zero, checking19,448 unique frozen paths,
3,506 immutable original paths,44 exact native configurations,41 canonical
resources/1,804 native resource copies and109 distribution manifest files. It
explicitly retains/checks the14 unpromoted GameTime diagnostics and correction
records. No region, threshold or earlier expectation changed.

M4dk subsequently measures [NoKO voice/echo behavior](NO-KO-AUDIO.md).
Helper/projectile assertion contexts,
additional round/time-over combinations and broader profiles remain independent
measurements. NoMusic/BGM still belongs to its concrete dependent allocation.
Earlier strict HUD/raster failures remain unwaived. Source routes remain75/89
typed(84.3%); this is not behavioral or whole-engine completion.
