// This helper runs in the isolated reference Wine prefix. It reads only the
// known MUGEN error dialog, without keyboard, clipboard, focus or pointer input.
package main

import (
	"fmt"
	"os"
	"strings"
	"syscall"
	"unsafe"
)

func main() {
	user := syscall.NewLazyDLL("user32.dll")
	title, _ := syscall.UTF16PtrFromString("M.U.G.E.N Error")
	window, _, _ := user.NewProc("FindWindowW").Call(0, uintptr(unsafe.Pointer(title)))
	if window == 0 {
		fmt.Fprintln(os.Stderr, "Reference error dialog is absent")
		os.Exit(1)
	}
	text := ""
	callback := syscall.NewCallback(func(child, _ uintptr) uintptr {
		class := make([]uint16, 256)
		user.NewProc("GetClassNameW").Call(child, uintptr(unsafe.Pointer(&class[0])), uintptr(len(class)))
		if !strings.HasPrefix(strings.ToLower(syscall.UTF16ToString(class)), "richedit") {
			return 1
		}
		buffer := make([]uint16, 65536)
		length, _, _ := user.NewProc("SendMessageW").Call(child, 13, uintptr(len(buffer)), uintptr(unsafe.Pointer(&buffer[0])))
		if length >= uintptr(len(buffer)) {
			length = uintptr(len(buffer) - 1)
		}
		text = syscall.UTF16ToString(buffer[:length])
		return 0
	})
	user.NewProc("EnumChildWindows").Call(window, callback, 0)
	if text == "" {
		fmt.Fprintln(os.Stderr, "Reference error text is absent")
		os.Exit(1)
	}
	fmt.Println(text)
}
