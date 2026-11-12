# Preserved compatibility counterexamples

These tracked text inputs and observations preserve the legacy HitVel coordinate
case covered by the match regression. The current implementation is required to
pass it; it is not an approved divergence or a numeric-tolerance exception.

A zero authored projectile velocity, mixed receiver/game coordinate widths and
lethal contact distinguish the legacy HitVel query from stored GetHitVar vectors
and later physics. Keep those observations separate when changing coordinate or
contact behavior. See the project documentation for legacy hit velocity and
round-result semantics. Broader projectile compatibility remains a separate
scope in the progress ledger.
