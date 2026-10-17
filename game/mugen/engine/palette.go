package engine

import (
	"fmt"

	"github.com/kazzmir/paintown/game/mugen/cns"
	"github.com/kazzmir/paintown/game/mugen/evaluator"
	"github.com/kazzmir/paintown/game/mugen/parsers"
)

func compileRemapPal(ctrl *cns.StateController) (ControllerFunc, bool, error) {
	var args []argument
	for _, key := range []string{"source", "dest"} {
		text, present := ctrl.Params[key]
		if !present {
			continue
		}
		parts := parsers.SplitList(text)
		if len(parts) != 2 {
			return nil, true, fmt.Errorf("%s requires group,item expressions", key)
		}
		for index, part := range parts {
			expr, err := evaluator.ParseReferenceForVersion(part, ctrl.LanguageVersion)
			if err != nil {
				return nil, true, fmt.Errorf("%s component %d: %w", key, index+1, err)
			}
			args = append(args, argument{argumentSpec{fmt.Sprintf("%s%d", key, index), true, true}, expr})
		}
	}
	return func(_ *cns.StateController, env Environment) error {
		values, err := evaluateArguments(args, env)
		if err != nil {
			return err
		}
		source, dest := [2]int{-1, 0}, [2]int{-1, 0}
		for index := 0; index < 2; index++ {
			if value, present := values[fmt.Sprintf("source%d", index)]; present {
				source[index] = int(value.Number())
			}
			if value, present := values[fmt.Sprintf("dest%d", index)]; present {
				dest[index] = int(value.Number())
			}
		}
		context, ok := env.(interface{ SetPaletteMapping([2]int, [2]int) error })
		if !ok {
			return fmt.Errorf("RemapPal palette context is unavailable")
		}
		return context.SetPaletteMapping(source, dest)
	}, true, nil
}
