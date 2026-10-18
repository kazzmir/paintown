# Preserved native counterexamples

These text inputs and native observations track open compatibility gates. They
are separate from the promoted passing round/result corpus. Do not regenerate
or edit the original inputs, recipes or goldens. PNGs and executable resources
remain in ignored temp/reference-runs.

roundlate-projectile-{8,9,10}-detail records a lethal projectile with a zero
authored velocity vector, a 320-wide receiver and a 1280-wide game. Native
HitVel Y is -24 (Var(22)=-2400); Go currently returns 0 (Var(22)=0)
because the legacy trigger is unbound. The stored GetHitVar vector and actual
physics are not compared by this legacy-trigger observation. All
other 59 integers agree in each program, including damage before/after the KO
grace, fall data, Alive, control, results and phase clocks. The focused diagnostic
log/source are temp/m4bv-projectile-coordinate-diagnostic{.log,-test.txt}.
The primary reference EXE hash and every source/recipe hash are in each JSON.
This is an open legacy HitVel trigger gate, not a waived numeric tolerance or
a claim of full projectile compatibility. See docs/ROUND-RESULTS.md.
