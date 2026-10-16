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

This foundation does not yet translate mixed-player collision/position contexts,
stage cropping/overdraw, camera zoom, custom-state/animation ownership, fight HUD
spaces or all storyboard drawing. Those conversions must use explicit source
and target profiles when the Match runtime is connected; loaded profiles alone
are not mixed-resolution gameplay parity.
