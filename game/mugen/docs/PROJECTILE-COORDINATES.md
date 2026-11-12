# Projectile coordinate profiles — M4cf coordinate checkpoint

Projectiles retain the active state owner's coordinate width at creation. Motion,
offsets, HitDef payloads, bounds and sprite/AIR geometry use that script profile,
including a projectile issued from a foreign state. Resources and attack strength
still come from the issuer; root ownership is unchanged. Mixed-profile rendering
converts each projectile through its own profile, including the legacy Player
renderer.

Fresh native payload reproduces the original. All sessions were awaited; a
separate process check finds no clients. The before log preserves four
counterexamples; no observed original or golden was edited.

Differing resource archives/size constants, zoom/extreme profiles, teams and broad
renderer parity remain gates. Event/legacy queries follow as M4cg, reversals as
M4ch. Source route count remains112/127; no whole-engine percentage is claimed.
