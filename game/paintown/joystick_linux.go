//go:build linux

package paintown

import (
	"encoding/binary"
	"log"
	"path/filepath"
	"strings"
	"time"
	"unsafe"

	"golang.org/x/sys/unix"
)

type linuxJoystick struct {
	fd       int
	name     string
	state    joystickState
	nextScan time.Time
}

var legacyJoystick = linuxJoystick{fd: -1}

func joystickIoctl(fd int, request uintptr, buffer []byte) error {
	_, _, errno := unix.Syscall(unix.SYS_IOCTL, uintptr(fd), request, uintptr(unsafe.Pointer(&buffer[0])))
	if errno != 0 {
		return errno
	}
	return nil
}

func openLinuxJoystick(path string) (*linuxJoystick, error) {
	fd, err := unix.Open(path, unix.O_RDONLY|unix.O_NONBLOCK|unix.O_CLOEXEC, 0)
	if err != nil {
		return nil, err
	}
	stick := &linuxJoystick{fd: fd}
	fail := func(err error) (*linuxJoystick, error) { unix.Close(fd); return nil, err }
	var name [128]byte
	if err := joystickIoctl(fd, 0x80806a13, name[:]); err != nil {
		return fail(err)
	} // JSIOCGNAME
	stick.name = strings.TrimRight(string(name[:]), "\x00")
	var count [1]byte
	if err := joystickIoctl(fd, 0x80016a11, count[:]); err != nil {
		return fail(err)
	} // JSIOCGAXES
	stick.state.axisCount = min(int(count[0]), len(stick.state.axes))
	if err := joystickIoctl(fd, 0x80016a12, count[:]); err != nil {
		return fail(err)
	} // JSIOCGBUTTONS
	stick.state.buttonCount = min(int(count[0]), len(stick.state.buttons))
	if err := joystickIoctl(fd, 0x80406a32, stick.state.axisCodes[:]); err != nil {
		return fail(err)
	} // JSIOCGAXMAP
	var buttonMap [1024]byte
	if err := joystickIoctl(fd, 0x84006a34, buttonMap[:]); err != nil {
		return fail(err)
	} // JSIOCGBTNMAP
	for index := range stick.state.buttonCodes {
		stick.state.buttonCodes[index] = binary.LittleEndian.Uint16(buttonMap[index*2:])
	}
	return stick, nil
}

func (stick *linuxJoystick) disconnect() {
	if stick.fd >= 0 {
		unix.Close(stick.fd)
	}
	stick.fd = -1
	stick.name = ""
	stick.state = joystickState{}
	stick.nextScan = time.Now().Add(time.Second)
}

// No goroutine or blocking read: js0-only setups (including containers without
// /dev/input/event*) work, while unplugging releases all logical buttons.
func pollLegacyController() (controllerState, string) {
	stick := &legacyJoystick
	if stick.fd < 0 {
		if time.Now().Before(stick.nextScan) {
			return controllerState{}, ""
		}
		stick.nextScan = time.Now().Add(time.Second)
		paths, _ := filepath.Glob("/dev/input/js*")
		for _, path := range paths {
			connected, err := openLinuxJoystick(path)
			if err != nil {
				continue
			}
			*stick = *connected
			log.Printf("Joystick: %s (%s)", stick.name, path)
			break
		}
		if stick.fd < 0 {
			return controllerState{}, ""
		}
	}
	var buffer [512]byte
	for range 32 {
		n, err := unix.Read(stick.fd, buffer[:])
		if err == unix.EAGAIN || err == unix.EWOULDBLOCK {
			break
		}
		if err == unix.EINTR {
			continue
		}
		if err != nil || n == 0 {
			stick.disconnect()
			return controllerState{}, ""
		}
		stick.state.feed(buffer[:n])
	}
	return stick.state.controller(), stick.name
}
