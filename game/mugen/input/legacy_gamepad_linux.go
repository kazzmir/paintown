//go:build linux

package input

import (
	"fmt"
	"os"
	"path/filepath"
	"unsafe"

	"golang.org/x/sys/unix"
)

// Linux Ebitengine enumerates evdev keys from BTN_MISC and ABS axes in kernel
// order, removing hat pairs from the axis array and appending their directions
// after real buttons. Read only capability metadata, never input events. Match
// its SDL GUID/name/counts so an unrelated event node cannot supply the layout.
func discoverLegacyJoystick(guid, name string, axes, combinedButtons int) (LegacyJoystickLayout, bool) {
	paths, _ := filepath.Glob("/dev/input/event*")
	for _, path := range paths {
		layout, deviceGUID, deviceName, ok := evdevJoystickLayout(path)
		if ok && deviceGUID == guid && deviceName == name && layout.Axes == axes && layout.Buttons+layout.Hats*4 == combinedButtons {
			return layout, true
		}
	}
	return LegacyJoystickLayout{}, false
}

func evdevJoystickLayout(path string) (LegacyJoystickLayout, string, string, bool) {
	file, err := os.Open(path)
	if err != nil {
		return LegacyJoystickLayout{}, "", "", false
	}
	defer file.Close()
	read := func(number, size uintptr, address unsafe.Pointer) bool {
		// Linux input.h _IOR('E', number, size), including variable-size bitsets.
		_, _, errno := unix.Syscall(unix.SYS_IOCTL, file.Fd(), 2<<30|size<<16|'E'<<8|number, uintptr(address))
		return errno == 0
	}
	var id [4]uint16 // bus, vendor, product, version
	var label [256]byte
	var keys [96]byte // KEY_CNT = 0x300
	var abs [8]byte   // ABS_CNT = 0x40
	if !read(2, 8, unsafe.Pointer(&id[0])) || !read(6, 256, unsafe.Pointer(&label[0])) ||
		!read(0x20+unix.EV_KEY, 96, unsafe.Pointer(&keys[0])) || !read(0x20+unix.EV_ABS, 8, unsafe.Pointer(&abs[0])) {
		return LegacyJoystickLayout{}, "", "", false
	}
	name := unix.ByteSliceToString(label[:])
	var guid string
	if id[1] != 0 && id[2] != 0 && id[3] != 0 {
		guid = fmt.Sprintf("%02x%02x0000%02x%02x0000%02x%02x0000%02x%02x0000",
			byte(id[0]), byte(id[0]>>8), byte(id[1]), byte(id[1]>>8), byte(id[2]), byte(id[2]>>8), byte(id[3]), byte(id[3]>>8))
	} else {
		guid = fmt.Sprintf("%02x%02x0000%x", byte(id[0]), byte(id[0]>>8), label[:12])
	}
	bit := func(bits []byte, n int) bool { return bits[n/8]&(1<<uint(n%8)) != 0 }
	var layout LegacyJoystickLayout
	for key := 0x100; key < 0x300; key++ {
		if bit(keys[:], key) {
			layout.Buttons++
		}
	}
	for axis := 0; axis < 0x40; axis++ {
		if !bit(abs[:], axis) {
			continue
		}
		if axis >= 0x10 && axis <= 0x17 {
			layout.Hats++
			axis++
		} else {
			layout.Axes++
		}
	}
	return layout, guid, name, true
}
