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

Validation covers all 22 stock bindings, KeyA overrides/presence, disabled actions,
named keys, injected held/edge queries, player confirmation, and facing-relative
gameplay inputs. This is not complete hardware/input compatibility: layout-specific
SDL WORLD symbols and shifted-only symbols have no current translation; gamepad
binding interpretation, device calibration, full command timing/release behavior,
simultaneous-direction policy and reference hardware comparisons remain open.
Joystick numbers are preserved as raw device identifiers in the existing config
storage; they are not decoded through the keyboard symbol table. Joystick runtime
polling/binding translation and YAML joystick overrides remain incomplete.
