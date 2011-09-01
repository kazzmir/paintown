#!/bin/sh

adb=/opt/android-sdk/platform-tools/adb
run='am start -a android.intent.action.MAIN -n org.libsdl.app/org.libsdl.app.SDLActivity'

(cd ../..; android=1 make) && ./install.sh && $adb shell $run

# ant debug && $adb uninstall org.libsdl.app && $adb install bin/Paintown-debug.apk
