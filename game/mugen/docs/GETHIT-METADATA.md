# Received hit metadata (M4ca)

Thirteen frozen beta1 programs comprise ten accepted controls / 120 observations
and three load rejections, with 52 authored CNS/CMD/AIR/stage hashes. Native
executable SHA256 is d5508f77f2a86f12358e669f91c64ba5e4a789d4b71262e8a758814384303a4e.
Inputs and exact numeric goldens are tests/testdata/reference/metadata-*;
`TestNativeGetHitMetadata` replays them through production Match/Character.
Accepted programs replay 45 active simulation passes, covering their last Time=30
sample without extending an intentionally incomplete custom state set past its
liedown deadline. Rejected negative states report their authored CMD filename.

* Received GetHitVar(hitcount) increases by one per actual unguarded contact,
  independently of HitDef numhits=3 and the attacker's HitCount/HUD count.
  Contacts while H accumulate; the first unguarded contact after I starts at one.
  The stored received count remains readable after leaving H.
* GetHitVar(fallcount) becomes visible after the first CNS pass in state 5100,
  not on entry to 5110. Repeated impacts accumulate while H; leaving H clears
  the fall count after that pass, so same-pass reads still see the old value.
* None contacts do not establish a combo. A first guarded contact has received
  hit/fall counts zero, with the authored chainid retained. Leaving H clears
  chainid, as shown by the lifetime controls.
* xveladd/yveladd are typed floats and return zero in ordinary, ground KO,
  launched KO and air KO controls. Beta folds the measured KO offsets into
  xvel/yvel itself; the old KO vector consumer remains responsible for motion.
* The manual lists hitid and zoff, but beta1 rejects both at load time. hitid
  was already rejected by the Go selector validator; zoff now is too. These
  are measured target limitations, not missing runtime values to invent.

The inventory now distinguishes parser rejection from missing routing: 33 of
36 documented selectors have runtime routes, two are rejected and recovertime
remains for M4cb. The extra air.animtype runtime switch is not accepted by the
static reference parser and is not included in documented counts. Trigger
family routing remains 109/127. These are routing metrics, not behavioral parity.

Native preparation uses reference_probe.py, --bank fvar --pages 0,
--clipboard-in-cmd --mugenversion 1.1 --fixture-stage --stage probe-stage.
Ordinary/combo/impact/lifetime/None/guard observations use five-second captures;
KO acceptance uses ten seconds because five seconds captured before Time=8.
Those early all-zero KO captures remain diagnostics under temp/reference-runs/.
The first drafts rejected zoff, then hitid; originals/goldens stay unchanged.
Unused drafts are retained under ignored temp/m4ca-unobserved-drafts/.
All generated executables/images remain ignored; only source/text is committed.

Full Character/evaluator/engine races pass (12.195/1.021/1.494s). Focused Match
races, fresh native reproduction and source/module audits are recorded in the
handoff. Final full scoped integration follows M4cb. No native numeric golden
or tolerance was edited. Repeated guarded/third-party/helper/projectile contacts,
foreign states, special impact transitions and broad pause/reset combinations
remain separate interaction gates.
