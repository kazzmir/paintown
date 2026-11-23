# Input compatibility

The supplied beta configuration uses SDL 1.2 key symbols. The earlier DOS
scancode conversion mapped its arrows, punctuation and P2 letters incorrectly.
The corrected conversion follows the
[SDL 1.2 key definitions](https://github.com/libsdl-org/SDL-1.2/blob/main/include/SDL_keysym.h).

Stock bindings, with ambient YAML overrides disabled:

| Action | P1 | P2 |
| --- | --- | --- |
| Up / down | Up / Down arrows | W / S |
| Left / right | Left / Right arrows | A / D |
| a / b / c | Comma / Period / Slash | F / G / H |
| x / y / z | L / Semicolon / Quote | R / T / Y |
| Start / confirm | Enter | U |

The optional scoped `mugen.yaml` may override these. Named values use Ebitengine
key names, with optional `Key` prefix and direction aliases. `none` or `disabled`
explicitly unbinds an action. Unknown SDL symbols are diagnosed and left unbound.
Unsupported named YAML values warn and preserve the previous binding.

Ebitengine's `KeyA` equals zero. Binding presence is recorded separately, including
across value copies, so a real A key is never mistaken for an omitted setting.
Programmatic callers should use `Keys.SetBinding` to record explicit zero-valued
or disabled bindings. Absent fields retain the port's existing fallback controls.

`InputManager.Keyboard` accepts an instance-local `KeyboardSource` with held and
edge queries. The default source reads Ebitengine; tests/replays inject frames
without sending keyboard events to the operating system. Gameplay's facing-relative
input uses the same manager. Menu confirmation honors each player's Start binding.

Legacy `[Input] P1.Joystick.type` / `P2.Joystick.type` and numeric joystick bindings
now reach the shared menu/gameplay consumer. CFG loads first; YAML lists replace
an action, explicit enable settings win, and the input editor remains session-only.
Native codes have four 32-code device blocks. Each device reserves two codes per
axis, then four per reported hat, then real buttons. The first axis pair is
vertical, the second horizontal; hat directions use Up/Down/Left/Right and alias
the first quartet. Zero and device-block boundaries are unbound. Numeric button
offsets depend on the device, so native codes are never treated as SDK indices.

The target executable's joystick initialization/event handler establishes this
namespace. The pinned Linux SDK's public button count includes appended hats;
the adapter discovers their boundary from matching evdev name/GUID/capabilities,
without reading another event stream. Unknown layouts warn instead of guessing.
Use named/raw YAML or session bindings when metadata is unavailable. Other
platforms retain named/raw controls; their legacy capability adapters are deferred.

The connected configuration/menu/combat/reconnect flow covers native translation,
explicit overrides, keyboard alternatives, held edges and player ownership.
Physical Linux acceptance remains H1 in the project ledger. Injected layouts
cannot prove hardware mapping/calibration or complete input compatibility;
layout-specific SDL WORLD and shifted-only keyboard symbols remain untranslated.
