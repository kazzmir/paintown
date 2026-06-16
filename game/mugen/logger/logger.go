package logger

import (
	"fmt"
	"os"
	"sync"
)

var (
	unresolvedLog  *os.File
	unresolvedOnce sync.Once
)

func LogUnresolved(msg string) {
	unresolvedOnce.Do(func() {
		var err error
		// Use a fixed path relative to project root or provide a way to set it
		unresolvedLog, err = os.OpenFile("game/mugen/unresolved.log", os.O_CREATE|os.O_TRUNC|os.O_WRONLY, 0644)
		if err != nil {
			fmt.Printf("Error opening unresolved.log: %v\n", err)
		}
	})

	if unresolvedLog != nil {
		fmt.Fprintf(unresolvedLog, "%s\n", msg)
	}
}
