package engine

import (
	"fmt"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
)

type clipboardPart struct {
	literal, format string
	floating        bool
}

func clipboardText(raw string) (string, error) {
	raw = strings.TrimSpace(raw)
	if len(raw) < 2 || raw[0] != '"' || raw[len(raw)-1] != '"' {
		return "", fmt.Errorf("text requires a double-quoted format string")
	}
	var text strings.Builder
	for i := 1; i < len(raw)-1; i++ {
		char := raw[i]
		if char == '"' {
			return "", fmt.Errorf("unescaped quote in clipboard text")
		}
		if char == '\\' {
			i++
			if i >= len(raw)-1 {
				return "", fmt.Errorf("incomplete clipboard escape")
			}
			switch raw[i] {
			case 'n':
				char = '\n'
			case 't':
				char = '\t'
			case '\\', '"':
				char = raw[i]
			default:
				return "", fmt.Errorf("unsupported clipboard escape \\%c", raw[i])
			}
		}
		text.WriteByte(char)
	}
	return text.String(), nil
}

func compileClipboard(ctrl *cns.StateController, appendText bool) (ControllerFunc, bool, error) {
	text, err := clipboardText(ctrl.Params["text"])
	if err != nil {
		return nil, true, err
	}
	var parts []clipboardPart
	arguments := 0
	for at := 0; at < len(text); {
		end := strings.IndexByte(text[at:], '%')
		if end < 0 {
			parts = append(parts, clipboardPart{literal: text[at:]})
			break
		}
		end += at
		if end > at {
			parts = append(parts, clipboardPart{literal: text[at:end]})
		}
		if end+1 < len(text) && text[end+1] == '%' {
			parts = append(parts, clipboardPart{literal: "%"})
			at = end + 2
			continue
		}
		cursor := end + 1
		for cursor < len(text) && strings.ContainsRune("-+ #0", rune(text[cursor])) {
			cursor++
		}
		for cursor < len(text) && text[cursor] >= '0' && text[cursor] <= '9' {
			cursor++
		}
		if cursor < len(text) && text[cursor] == '.' {
			cursor++
			for cursor < len(text) && text[cursor] >= '0' && text[cursor] <= '9' {
				cursor++
			}
		}
		if cursor >= len(text) || !strings.ContainsRune("difFeEgG", rune(text[cursor])) {
			return nil, true, fmt.Errorf("unsupported clipboard conversion at %q", text[end:])
		}
		verb := text[cursor]
		format := text[end : cursor+1]
		floating := verb != 'd' && verb != 'i'
		if verb == 'i' {
			format = format[:len(format)-1] + "d"
		}
		// printf's default %g precision is six; Go's default is shortest.
		if (verb == 'g' || verb == 'G') && !strings.Contains(format, ".") {
			format = format[:len(format)-1] + ".6" + string(verb)
		}
		parts = append(parts, clipboardPart{format: format, floating: floating})
		arguments++
		at = cursor + 1
	}
	if arguments > 6 {
		return nil, true, fmt.Errorf("clipboard supports at most six numeric parameters")
	}
	var params []evaluator.Expression
	raw := strings.TrimSpace(ctrl.Params["params"])
	if arguments > 0 {
		params, err = evaluator.ParseReferenceTupleForVersion(raw, arguments, ctrl.LanguageVersion)
		if err != nil {
			return nil, true, fmt.Errorf("params: %w", err)
		}
		for _, expression := range params {
			if err := evaluator.ValidateKnownTriggers(expression); err != nil {
				return nil, true, fmt.Errorf("params: %w", err)
			}
		}
	} else if raw != "" {
		return nil, true, fmt.Errorf("params supplied without a numeric format conversion")
	}
	return func(_ *cns.StateController, env Environment) error {
		values := make([]evaluator.Value, len(params))
		for i, expression := range params {
			value := evaluator.EvaluateValue(expression, env)
			if err := value.RuntimeError(); err != nil {
				return fmt.Errorf("params[%d]: %w", i, err)
			}
			if value.IsBottom() {
				value = evaluator.Int(0)
			}
			if !value.IsNumber() {
				return fmt.Errorf("params[%d] requires a number", i)
			}
			values[i] = value
		}
		var output strings.Builder
		index := 0
		for _, part := range parts {
			if part.format == "" {
				output.WriteString(part.literal)
				continue
			}
			value := values[index]
			index++
			integer, isInt := value.Int32()
			switch {
			case part.floating && isInt:
				output.WriteString(fmt.Sprintf("%d", integer))
			case !part.floating && !isInt:
				output.WriteString(fmt.Sprintf("%f", value.Number()))
			case isInt:
				output.WriteString(fmt.Sprintf(part.format, integer))
			default:
				output.WriteString(fmt.Sprintf(part.format, value.Number()))
			}
		}
		if appendText {
			env.AppendToClipboard(output.String())
		} else {
			env.DisplayToClipboard(output.String())
		}
		return nil
	}, true, nil
}
