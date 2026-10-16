package logger

import (
	"io"
	"log"
	"os"
)

var unresolved = log.New(os.Stderr, "mugen: ", 0)

// SetOutput configures diagnostics without opening a working-directory-relative
// file. The caller owns the writer. Passing nil disables this diagnostic stream.
func SetOutput(w io.Writer) {
	if w == nil {
		w = io.Discard
	}
	unresolved.SetOutput(w)
}

func LogUnresolved(msg string) {
	unresolved.Print(msg)
}
