# Actor and owned-effect ordering — M4ct

This checkpoint addresses the four M4cs owned-Explod overlap counterexamples
and shared actor/effect ordering before EnvColor. The focused checkpoint acceptance gates pass; remaining variants stay explicit.

## Native observations

Sixteen separately frozen programs provide192 numeric readings and16 static
captures.65 original/probe hashes preserve all inputs. They cover negative,
zero and positive actor/effect priorities, equal priorities, both creation
orders for two Explods, `ontop`, ModifyExplod priority updates and overlap with
a visible second root or helper. Actual controller-kind counts are pinned per
state and checked independently from numeric replay.

An ordinary owned effect draws in front of an actor only when its priority is
greater than that actor's. Equal priority puts the actor in front. Priority is
compared against actual actor values, rather than against zero. Earlier-created
equal-priority Explods draw in front of later ones. The two opposite creation
controls discriminate10,485 source pixels; every discriminating pixel matches
the corresponding native-only single-pose front control. `ontop` overrides
the root's higher sprite priority, and ModifyExplod changes the drawn order.

These are measured output rules, not a claim to recover native internals. The
new native crop is560,270..1000,615 (151,800 RGB pixels). The crop/hash is derived
from each immutable native image before running the changed Go application.
No observation, original capture, comparison tolerance or prior golden changes.

## Production change

The shared actor renderer now accepts renderer-owned sprite callbacks and
interleaves them against roots, helpers and projectiles by sprite priority.
Equal-priority effect callbacks draw before actors. The effect scene supplies
ordinary owned effects in reverse creation order, preserving stable sorting
for ties. Each callback reads the instance's current drawing state, so modified
priorities are consumed on subsequent draws. Existing actor/actor ties retain
their measured lower-world-ID rule.

Unowned scene cues retain their separate existing layer; `ontop` remains after
stage foreground. Texture usage is collected over the whole frame so drawing
one interleaved effect cannot evict another effect's cached image. Unused cache
entries are released once at frame end; palette revision replacement also
releases the old GPU image.

## Verification and limits

All16 actual-application crops are byte-exact (2,428,800 pixels). All four
unchanged M4cs overlap scenes now have exact actor and stage crops (434,700 actor
and462,400 stage pixels). The previous67 M4cs passing actor crops retain their
historical evidence; this checkpoint reruns the four failing scenes on the new
binary rather than claiming a fresh71-scene matrix.

Core races pass Character11.320s/System9.465s/Effect1.011s;99 native effect/global
programs pass67.354s and14 actor-order programs pass10.476s. Two local PalFX and
two indexed/RGB raster regression apps pass. Stock win/rematch/win is visually
inspected, with82 balanced owned events and1,965,839 private PCM frames. All
frozen source/native/resource/binary/config/PNG/audio/module/inventory guards
pass. Final audit:temp/m4ct-final-evidence-audit.json. No clients remain.

The last full scoped suite remains M4cs3,857 results/36 packages; focused tests
are appropriate for this dependent checkpoint and the full suite was not
repeated. New app binary SHA-256 is
3b89418915f793ca92b7a0e253dbdb03a17dcb2ed69e487a8513ef2bd46b1f05.

The public `tests/effect_order_render_probe.py` is read-only. It checks original
sources, native executable/capture, prepared assets, application binary/config/
PNG and every RGB pixel in the fixed native crop. It cannot record expectations.

Multiple `ontop` effects, foreground/layer interactions, unowned cues, capacity,
creation palette/remapping/random/shadow and broader helper/projectile cases
remain separate variants. The12 earlier M4cq fractional/nonquarter raster
counterexamples are unrelated and remain unwaived. No full-frame/font/platform
parity or whole-engine coverage percentage follows from this checkpoint.
