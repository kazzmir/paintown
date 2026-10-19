# Projectile coordinate profiles — M4cf coordinate checkpoint

Projectiles retain the active state owner's coordinate width at creation. Motion,
offsets, HitDef payloads, bounds and sprite/AIR geometry use that script profile,
including a projectile issued from a foreign state. Resources and attack strength
still come from the issuer; root ownership is unchanged. Mixed-profile rendering
converts each projectile through its own profile, including the legacy Player
renderer.

Eleven immutable native programs pin 132 values and 44 original input hashes.
Own width640 to receiver320 gives X hit velocity0.625/yaccel0.25; foreign width320
gives1.25/0.5. The reverse foreign width640 case also gives1.25/0.5 in receiver640.
Motion/bounds and positive lifetime with both hitpause sides are covered. Three
separate frozen native render programs distinguish full and half sprite sizes:
7935 opaque pixels at [449,357,568,555], versus1793 at [545,479,604,576]. All
three actual Go application masks match exactly. Numeric replays pass8.886s;
full Character/System/engine/evaluator races pass11.593/9.789/1.461/1.021s.

Final selected projectile/chain/legacy-velocity Match races pass192.297s. Fresh
native payload reproduces the original. Four-PNG stock win/rematch/win with82
balanced owned events and private PCM passes binary/config/source/PNG/audio
audits. All355 frozen source/probe/fixture/module hashes and root module guards
pass. All sessions were awaited; a separate process check finds no clients.
Artifacts: ignored temp/m4cf-space-{before,after,core,final}.log,
m4cf-space-final-inputs.json, m4cf-space-final-validation.json,
m4cf-space-native-audit.json, m4cf-space-phase-app-audit.json and
m4cf-space-app-evidence-audit.json. The before log preserves four counterexamples;
no observed original or golden was edited.

Differing resource archives/size constants, zoom/extreme profiles, teams and broad
renderer parity remain gates. Event/legacy queries follow as M4cg, reversals as
M4ch. Source route count remains112/127; no whole-engine percentage is claimed.
