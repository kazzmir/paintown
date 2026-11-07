# Next timed-HUD phase controls

M4dr implements the independently measured KO start gate and visible-message
clock. It closes five held Round/Fight overlays through component expiry. The
KO corpus deliberately holds PreOver for1,000 simulation passes; it does not
establish Round/Fight/Win-specific start rules or later phase overlap.

Continue these native distinctions before changing more phase logic:

- Hold authored Intro with AssertSpecial through GameTime399 and release at400.
  Reuse the immutable HUD recorder with a new frozen sequence: seek399, then
  capture the transition. Use invisible actors, positive stock sprite200,0,
  complete fight sidecars and live numeric counters. This makes the early round
  messages measurable after the native window has settled.
- Compare Round.time0/positive, default versus roundN components, finite display
  duration and animation lifetime. Observe the first visible frame and both
  roots' first RoundState2 pass. Match.RoundCall currently uses displaytime alone;
  do not infer its phase deadline from the KO component rule.
- Compare Fight.time0/positive and ctrl.time0/positive around first control.
  The current ctrl.time0 path bypasses FightCall and therefore does not register
  a fight message. This is an existing gap, not a measured correction yet.
  Verify drawing and expiry after control independently of granting control.
- Add nonempty inert winner/draw/dead-on-ground states and first establish a
  positive, visible Win component. Observe readiness, over.waittime and the
  actual first RoundState4 pass before varying win.time/displaytime. Measure
  KO/Win overlap and round reset. Current drawing selects KO only in PreOver
  and still uses the legacy Win/Draw text path; those are not closed by M4dr.
- Announcement sound delays, especially beyond FightCall or across slowdown,
  need separate positive native audio controls. Keep these distinct from BGM,
  which remains item4.

Use independently checked whole recipes before native execution. Retain any
short/unreached diagnostic window, extend under a fresh name, and review every
native-only value/image/geometry row before implementing. Never edit the already
native-used HUD recorder, prior programs, captures, configs, goldens or tolerances.

The95 visible KO sprite frames retain max-one RGB failures; eight text frames
retain placement differences. These causes are unassigned. Keep all strict
comparisons and older exact controls. Further font or sampling changes require
their own native evidence. Broader assets/fonts/modes remain item4; finish other
independent item3 work before a concrete allocation question.

After the HUD phase studies, continue finite/overlap color isolation, clipboard
formatting/debug display, residual variable aliases/StateTypeSet/HitAdd, wider
flags and prior strict obligations. Local source/text checkpoints remain
authorized; no checkpoint approval pause is required.
