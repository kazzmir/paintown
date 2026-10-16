# Coordinate spaces

The supplied [coordinate-space specification](../temp/mugen-1.1b1/docs/coordspace.html)
defines a common game space through Config.GameWidth/GameHeight. Characters and
motifs use Info.localcoord; stages use StageInfo.localcoord. Vector translation
uses the ratio of target width to source width for both axes, even when their
aspect ratios differ. The default local space is 320x240.

`coord.Space` retains dimensions and performs width-ratio translations. Loaders
preserve character, motif, stage and game profiles with contextual errors. Native
probes show fractional widths are accepted and truncated: 320.5 behaves as 320,
and 640.5 as 640. The parser applies integer coercion to both dimensions; height
rounding has not been isolated separately. Zero/nonfinite/out-of-range resulting
dimensions are rejected.

Character environments bind Const240p/Const480p/Const720p to local width. They
always return float32, propagate bottom and reject bad arity before compiled
controller effects. Base widths are 320, 640 and 1280. Five source/hash-pinned
native cases cover standard spaces and fractional widths, float division and
fractional/negative arguments. Helpers/projectiles inherit their creator's
profile; their complete ownership/lifecycle behavior remains separate work.

Production menus use the motif's declared logical dimensions. Background default
windows use that profile, and drawing uses the actual logical horizontal center.
The physical window honors Video.Width/Height without the former implicit x2.
The high-resolution title was inspected in temp/app-runs/hd-menu-centered/.

Separate-player native probes now pin 1280/320 and 320/1280 pairings. The
querying player sees its own local units: start -280/-70, P2Dist X 560/140,
and P2BodyDist X 480/120. Body distance compares body fronts rather than axes;
Y remains an axis distance. All 24 captured fvars match the Character fixture.

Cross-player position queries, guard range/facing, collision boxes and push
movement now translate the other player. Authored geometric tests verify equal
physical extents and displacement across spaces. Push/collision timing and
corner rules, dynamic Width overrides, full airborne cases and query redirection
remain open. The current Target list still conflates opponents with acquired
HitDef targets and must be replaced by the match ownership model.

This foundation does not yet translate hit vectors, stage cropping/overdraw,
camera zoom, custom-state/animation ownership, fight HUD spaces or all storyboard
drawing. Those conversions must use explicit source and target profiles when the
Match runtime is connected; these tests are not mixed-resolution gameplay parity.
