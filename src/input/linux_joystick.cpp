#ifdef LINUX

/* based on
 * jstest.c  Version 1.2
 *
 * Copyright (c) 1996-1999 Vojtech Pavlik
 *
 * Sponsored by SuSE
 */

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <sstream>

#include <linux/input.h>
#include <linux/joystick.h>

#include "linux_joystick.h"
#include "globals.h"

using namespace std;

static const char *axis_names[ABS_MAX + 1] = {
"X", "Y", "Z", "Rx", "Ry", "Rz", "Throttle", "Rudder", 
"Wheel", "Gas", "Brake", "?", "?", "?", "?", "?",
"Hat0X", "Hat0Y", "Hat1X", "Hat1Y", "Hat2X", "Hat2Y", "Hat3X", "Hat3Y",
"?", "?", "?", "?", "?", "?", "?", 
};

static const char *button_names[KEY_MAX - BTN_MISC + 1] = {
"Btn0", "Btn1", "Btn2", "Btn3", "Btn4", "Btn5", "Btn6", "Btn7", "Btn8", "Btn9", "?", "?", "?", "?", "?", "?",
"LeftBtn", "RightBtn", "MiddleBtn", "SideBtn", "ExtraBtn", "ForwardBtn", "BackBtn", "TaskBtn", "?", "?", "?", "?", "?", "?", "?", "?",
"Trigger", "ThumbBtn", "ThumbBtn2", "TopBtn", "TopBtn2", "PinkieBtn", "BaseBtn", "BaseBtn2", "BaseBtn3", "BaseBtn4", "BaseBtn5", "BaseBtn6", "BtnDead",
"BtnA", "BtnB", "BtnC", "BtnX", "BtnY", "BtnZ", "BtnTL", "BtnTR", "BtnTL2", "BtnTR2", "BtnSelect", "BtnStart", "BtnMode", "BtnThumbL", "BtnThumbR", "?",
"?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", "?", 
"WheelBtn", "Gear up",
};

/* tries to open one /dev/input/jsX and return an open file descriptor */
static int read_joystick(){
    for (int num = 0; num < 100; num++){
        ostringstream name;
        name << "/dev/input/js" << num;
        const char * cname = name.str().c_str();
        int fd = open(cname, O_RDONLY);
        if (fd >= 0){
            return fd;
        }
    }
    return -1;
}

LinuxJoystick::LinuxJoystick():
button(0),
axis(0){
    file = read_joystick();
    if (file == -1){
        Global::debug(0) << "Could not open joystick" << endl;
    } else {
        Global::debug(1) << "Opened joystick " << endl;

        /* grab useful info */
        ioctl(file, JSIOCGVERSION, &version);
        ioctl(file, JSIOCGAXES, &axes);
        ioctl(file, JSIOCGBUTTONS, &buttons);
        ioctl(file, JSIOCGNAME(NAME_LENGTH), name);
        ioctl(file, JSIOCGAXMAP, axmap);
        ioctl(file, JSIOCGBTNMAP, btnmap);

        /* put joystick in nonblocking mode */
        fcntl(file, F_SETFL, O_NONBLOCK);
		
        axis = new int[axes];
        memset(axis, 0, sizeof(int) * axes);
        button = new char[buttons];
        memset(button, 0, sizeof(char) * buttons);

        Global::debug(1) << "Joystick axis: " << (int)axes << " buttons: " << (int)buttons << endl;
    }
}

LinuxJoystick::~LinuxJoystick(){
    delete[] axis;
    delete[] button;
    if (file != -1){
        close(file);
    }
}

void LinuxJoystick::poll(){

    struct js_event js;

    if (file == -1){
        return;
    }

    /* from an email by Vojtech Pavlik:
     * "if you #include <linux/joystick.h>, the struct is defined in such a way
     * that gcc doesn't add padding."
     */
    int bytes = read(file, &js, sizeof(struct js_event));
    if (bytes == sizeof(struct js_event)){
        Global::debug(4) << "Event: type " << (int)js.type << " time " << (int)js.time << " number " << (int)js.number << " value " << (int)js.value << endl;

        if (errno != EAGAIN) {
            perror("joystick: error reading");
            return;
        }

        switch (js.type & ~JS_EVENT_INIT) {
            case JS_EVENT_BUTTON:
                button[js.number] = js.value;
                break;
            case JS_EVENT_AXIS:
                axis[js.number] = js.value;
                break;
        }
    }
}

bool LinuxJoystick::pressed(){
    if (file == -1){
        return false;
    }

    for (int i = 0; i < axes; i++){
        if (axis[i]){
            return true;
        }
    }
    for (int i = 0; i < buttons; i++){
        if (button[i]){
            return true;
        }
    }
    return false;
}
    
JoystickInput LinuxJoystick::readAll(){
    JoystickInput input;
    if (file == -1){
        return input;
    }
    if (axes > 0){
        if (axis[0] < 0){
            input.left = true;
        }
        if (axis[0] > 0){
            input.right = true;
        }
        if (axes > 1){
            if (axis[1] < 0){
                input.up = true;
            }
            if (axis[1] > 0){
                input.down = true;
            }
        }
    }

    if (buttons > 0 && button[0]){
        input.button1 = true;
    }
    if (buttons > 1 && button[1]){
        input.button2 = true;
    }
    if (buttons > 2 && button[2]){
        input.button3 = true;
    }
    if (buttons > 3 && button[3]){
        input.button4 = true;
    }

    Global::debug(1) << "joystick input up " << input.up
                                 << " down " << input.down
                                 << " left " << input.left
                                << " right " << input.right
                              << " button1 " << input.button1
                              << " button2 " << input.button2
                              << " button3 " << input.button3
                              << " button4 " << input.button4
                              << endl;
    return input;
}

#if 0
int main (int argc, char **argv)
{
	int fd, i;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[NAME_LENGTH] = "Unknown";
	uint16_t btnmap[KEY_MAX - BTN_MISC + 1];
	uint8_t axmap[ABS_MAX + 1];

	if (argc < 2 || argc > 3 || !strcmp("--help", argv[1])) {
		puts("");
		puts("Usage: jstest [<mode>] <device>");
		puts("");
		puts("Modes:");
		puts("  --normal           One-line mode showing immediate status");
		puts("  --old              Same as --normal, using 0.x interface");
		puts("  --event            Prints events as they come in");
		puts("  --nonblock         Same as --event, in nonblocking mode");
		puts("  --select           Same as --event, using select() call");
		puts("");
		return 1;
	}
	if ((fd = open(argv[argc - 1], O_RDONLY)) < 0) {
		perror("jstest");
		return 1;
	}

	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);
	ioctl(fd, JSIOCGAXMAP, axmap);
	ioctl(fd, JSIOCGBTNMAP, btnmap);


	printf("Driver version is %d.%d.%d.\n",
		version >> 16, (version >> 8) & 0xff, version & 0xff);

	printf("Joystick (%s) has %d axes (", name, axes);
	for (i = 0; i < axes; i++)
		printf("%s%s", i > 0 ? ", " : "", axis_names[axmap[i]]);
	puts(")");

	printf("and %d buttons (", buttons);
	for (i = 0; i < buttons; i++)
		printf("%s%s", i > 0 ? ", " : "", button_names[btnmap[i] - BTN_MISC]);
	puts(").");

	printf("Testing ... (interrupt to exit)\n");

/*
 * Old (0.x) interface.
 */

	if ((argc == 2 && version < 0x010000) || !strcmp("--old", argv[1])) {

		struct JS_DATA_TYPE js;

		while (1) {

			if (read(fd, &js, JS_RETURN) != JS_RETURN) {
				perror("\njstest: error reading");
				return 1;
			}

			printf("Axes: X:%3d Y:%3d Buttons: A:%s B:%s\r",
				js.x, js.y, (js.buttons & 1) ? "on " : "off", (js.buttons & 2) ? "on " : "off");

			fflush(stdout);

			usleep(10000);
		}
	}

/*
 * Event interface, single line readout.
 */

	if (argc == 2 || !strcmp("--normal", argv[1])) {

		int *axis;
		char *button;
		int i;
		struct js_event js;

		axis = calloc(axes, sizeof(int));
		button = calloc(buttons, sizeof(char));

		while (1) {
			if (read(fd, &js, sizeof(struct js_event)) != sizeof(struct js_event)) {
				perror("\njstest: error reading");
				return 1;
			}

			switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				button[js.number] = js.value;
				break;
			case JS_EVENT_AXIS:
				axis[js.number] = js.value;
				break;
			}

			printf("\r");

			if (axes) {
				printf("Axes: ");
				for (i = 0; i < axes; i++)
					printf("%2d:%6d ", i, axis[i]);
			}

			if (buttons) {
				printf("Buttons: ");
				for (i = 0; i < buttons; i++)
					printf("%2d:%s ", i, button[i] ? "on " : "off");
			}

			fflush(stdout);
		}
	}


/*
 * Event interface, events being printed.
 */

	if (!strcmp("--event", argv[1])) {

		struct js_event js;

		while (1) {
			if (read(fd, &js, sizeof(struct js_event)) != sizeof(struct js_event)) {
				perror("\njstest: error reading");
				return 1;
			}

			printf("Event: type %d, time %d, number %d, value %d\n",
				js.type, js.time, js.number, js.value);

			fflush(stdout);
		}
	}

/*
 * Reading in nonblocking mode.
 */

	if (!strcmp("--nonblock", argv[1])) {

		struct js_event js;

		fcntl(fd, F_SETFL, O_NONBLOCK);

		while (1) {

			while (read(fd, &js, sizeof(struct js_event)) == sizeof(struct js_event))  {
				printf("Event: type %d, time %d, number %d, value %d\n",
					js.type, js.time, js.number, js.value);
			}

			if (errno != EAGAIN) {
				perror("\njstest: error reading");
				return 1;
			}

			usleep(10000);
		}
	}

/*
 * Using select() on joystick fd.
 */

	if (!strcmp("--select", argv[1])) {

		struct js_event js;
		struct timeval tv;
		fd_set set;

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		while (1) {

			FD_ZERO(&set);
			FD_SET(fd, &set);

			if (select(fd+1, &set, NULL, NULL, &tv)) {

				if (read(fd, &js, sizeof(struct js_event)) != sizeof(struct js_event)) {
					perror("\njstest: error reading");
					return 1;
				}

				printf("Event: type %d, time %d, number %d, value %d\n",
					js.type, js.time, js.number, js.value);

			}

		}
	}

	printf("jstest: unknown mode: %s\n", argv[1]);
	return -1;
}
#endif

#endif
