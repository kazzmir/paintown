#!/bin/bash

adb=/opt/android-sdk/platform-tools/adb

echo "Making sure you are running the emulator"
state=$($adb get-state)
if [ $state = "unknown" ]; then
    echo "Emulator is not running please start."
else
    cp ../../paintown libs/armeabi/libpaintown.so
    if ant debug; then
        $adb uninstall org.libsdl.app
        $adb install bin/Paintown-debug.apk
    else
        echo "Build failed."
    fi
fi
