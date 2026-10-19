# Projectile contacts and cancellation (M4cf)

Predecessor/reversal: 4b3ab498. This coherent contact/cancellation checkpoint passes its gates. Continue
through the authorized projectile/query/reversal scope;
items 3–4 remain deferred. PROJECTILES.md records the completed M4ce boundary.

91 frozen accepted native programs are promoted: projlife (33), projcancel
(25), projdetail (14), projfollow (7), projphase (3), projclock (6), projend (3). The original
input manifests and raw runs are ignored temp/m4cf-* / reference-runs/m4cf-*.
Never edit observed CNS/CMD/AIR/stage files or their numeric outputs. An extra
eight projspace-* profile/lifetime programs have 32 pre-frozen input hashes but
are now captured and promoted, outside this commit. Three additional
projspace-draw-* originals (12 hashes) are frozen but not observed yet. They
belong to the next dependent coordinate slice.

Native findings implemented in the current draft:

- projhits=0 makes a passive counted projectile; negative -1/-2 behave like 1.
  With projremove=0, exhausting hits stops further damage but leaves it counted.
  With removal enabled, only the final hit starts the hit animation/removal.
- Miss time 0/1 allows consecutive contacts. Positive 2/3 separates contacts
  by that many passes. All measured negative values (-1/-2/-3/-99) use 2.
  Miss time starts counting after hitpause ends: pause=4 plus miss=2 gives
  six passes between contacts, pause=4 plus miss=6 gives ten. Hitpause alone
  permits the next contact four passes later. Frozen controls pin every life
  sample rather than inferring only a final count.
- None reactions still consume projectile hits and retire after the specified
  count, despite no damage. The first Go replay found this one missing consumer
  (87/88 controls matched); the corrected consumer passes the unchanged native observation.
- HitOnce=0/1 does not suppress the configured multiple projectile hits.
- Cancellation uses Clsn2 against Clsn2. Clsn1-only projectiles do not cancel.
  Missing HitDef attributes and zero remaining hits do not prevent cancellation.
  HitDef priority/classes do not replace projpriority. Equal priorities cancel
  both; higher survives and loses one priority. Zero/negative priorities retain
  their signed ordering. Each projectile participates in at most one cancellation
  pair per pass, including a survivor. Multi-projectile attrition pins that limit.
- Removal/hit/cancel animations are visual tails: they are excluded from NumProj
  and do not occupy active projectile capacity or count as NumExplod. A new tail
  holds its first animation frame on its creation pass; new active projectiles
  likewise show their first frame. Three frozen positive-CNS clipboard/Pause
  controls independently preserve first-frame PNG masks (4473 pixels each).
  Finite tails finish; missing/negative actions retire immediately.
- Active projectiles share a root-owned pool: default 32, minimum 5, including
  configured zero/one and seven controls. Full controllers still evaluate all
  44 tagged expressions before discarding a creation at capacity. The config
  loader now defaults PlayerProjectileMax to 32 and production passes its limit.
- Helper-created projectiles move on their birth pass. Both roots observe the
  same pre-motion/lifetime state during CNS, so projectiles have a separate
  phase after all root/helper CNS. Existing root/helper scheduling is preserved.
  Animation/lifetime do not age on the birth pass; pause allowance consumption
  begins on following passes. Expiry is prepared before the next CNS, including
  an already-expired projectile becoming frozen. Six clock controls distinguish
  alternating attack/cancellation boxes and creation during an existing Pause.
- AttackMul is captured from the issuer at creation, including a destroyed
  helper; later root/helper changes do not affect its three hits. Power/targets
  credit the root while root MoveHit/MoveContact/HitCount stay zero. p1stateno
  changes the root owner, not the projectile. A p2 idle custom state suppresses
  pending damage while preserving measured power/state behavior.

Production draft: character/projectile.go, projectile_cancel.go, hit_chain.go,
tick.go, character.go; World/config/Match/System capacity handoff; the native
probe has an optional disposable --projectile-max override and the replay harness
reads that metadata. No Go tests have been run concurrently with native windows.
The initial broad shell edit was rejected by automatic approval review because
marker truncation could remove unrelated code. It did not execute. Explicit,
bounded function patches were used instead; nothing remains blocked by that
review and no user approval is needed for the safe edits.

Validation passes. The initial 61.846s development race run found only the None
consumer mismatch and is retained diagnostically. The corrected integrated
Match race run passes 197.707s: all 295 selected new/prior projectile, chain/
juggle, legacy velocity, Random and HitPauseTime controls. A subsequent selected
non-native Match/clock run including the three new animation-end controls passes
27.278s. Full core races pass: Character 11.416s, System 9.726s, engine 1.451s,
evaluator 1.019s, config 1.040s, CNS/combat 1.013s each.

Native provenance pins 364 original hashes, 1,118 distinct variable slots /
1,128 capture readings (the full-field case overlaps ten slots), 91 programs
and exact prepared CNS/AIR/stage sources. Six separate native render controls
pin first-frame masks and finite hit/expiry/cancel tail completion. Six actual
Go apps match those masks exactly: three 4473-pixel rectangles and three empty
regions. A fresh priority-attrition native run reproduces all 12 values. Stock
four-PNG win/rematch/win, 82 balanced owned events and private PCM pass source/
config/binary/PNG/input/audio checks. All 755 frozen source/probe/fixture/module
hashes and root module guards pass. All sessions were awaited; no clients remain.

Artifacts are ignored temp/m4cf-{focused-match,core,core-all,final}.log,
m4cf-final-inputs.json, m4cf-final-validation.json, m4cf-native-audit.json,
m4cf-phase-app-audit.json and m4cf-app-evidence-audit.json. Last complete scoped
race baseline is M4cb; the final allocated integration will rerun it with a
longer package deadline for the expanded corpus.

The next coordinate slice will compare the eight captured projspace-* controls
against Go, fix foreign-state units as demonstrated and use the three pre-frozen
coordinate render controls. Native payloads distinguish source width 320/640:
foreign/own X velocities are 1.25/0.625 in the 320-unit receiver, with corresponding
yaccel 0.5/0.25. No conversion code has been added yet. Event query families and
legacy forms follow as M4cg, then reversals as M4ch. Broad effects/shadows,
mode/team combinations, numeric extremes and final renderer parity remain gates.
