# Contact and fall shake frequency — M4cw

The shared HitDef/fall-shake parser now normalizes frequency magnitude and
replaces exact zero with0.001. It previously clamped every value below0.001,
which discarded negative frequencies and exaggerated tiny nonzero frequencies.
This follow-up uses separate native controls rather than assuming that the
standalone EnvShake policy also applies to contact/fall data.

## Measured beta behavior

All load. Six controls per source cover negative90 with omitted phase,
negative/positive30 with phase0, positive/negative0.0001 with phase0, and exact
zero with phase0. Every request lasts8 ticks. Tiny/zero controls magnify
displacement with amplitude100000; ordinary controls use4. Fall controls expose
both stored metadata and eight physical ScreenPosY readings. Contact controls
expose eight physical readings.

| Requested frequency | Stored native fall frequency | Default phase when omitted |
| --- | --- | --- |
| -90 | 90 | 90 |
| -30 | 30 | Not omitted in this control |
| 30 | 30 | Not omitted in this control |
| -0.0001 | 9.99999975e-05 (float32) | Not omitted |
| 0.0001 | 9.99999975e-05 (float32) | Not omitted |
| 0 | 0.00100000005 (float32) | Not omitted |

Negative/positive30 produce identical waves. Both tiny signs produce identical
waves and differ from exact zero. The fall tiny wave is
200,200,200,199,199,199,199,200; zero produces
200,198,197,195,193,191,190,200. The contact snapshots start a tick earlier relative
to the deferred request, consistent with the existing issuing-frame policy.
Amplitude, durations, evaluation order and request timing are unchanged.

The pre-fix replay fails against the newly captured negative90 fall control:
ScreenPos200 versus native196, stored frequency0.001 versus90 and phase0 versus90.
The diagnostic parser matches committedM4cv99a5f52c8. This establishes the actual
production failure, rather than only checking an isolated normalization helper.
No old HitDef/Fall/EnvShake source, capture or expected observation was changed.

`cns/hit_shake.go` now takes absolute frequency, converts tofloat32, replaces
exact zero, then selects an omitted phase from the normalized value. The typed
compiler, character coordinate conversion and global shake consumer retain their
existing paths. The controller inventory now includes this parser's source hash;
counts remain72typed/17fallback/5stubs/9partial/15bounded/60route-only.

The first authoring preflight caught an ambiguous FVar10/11 snapshot replacement
before any native execution. A separately corrected author preserves the24
already-written source paths byte-for-byte and completes the remaining controls.
All49 original hashes are frozen before observation. The native intent test
verifies the actual HitDef frequency/amplitude/time/phase fields and eight
FallEnvShake consumers where appropriate. No schema or renderer test can stand
in for a different intended controller.

## Verification and remaining scope

All145native programs pass152test/subtest results in93.361s. Allfour original
contact/fall rigid garment masks match exactly, without changing expectations.
Stockwin/rematch/win is visually inspected;82balanced owned inputevents and
1,915,680private PCM frames pass. The latest full scoped integration remains
M4cv4,033results/36packages and predates this parser fix.

No framebuffer sampler changed. Seven M4cv profile precision failures and12
M4cq fractional/nonquarter raster failures remain explicit and unwaived. This
bounded frequency fix does not establish complete shake/controller/engine
compatibility. More extreme values, legacy-version behavior and additional
mixed-owner/timing combinations remain interaction variants. Continue AfterImage
and all other allocated item3 consumers.
