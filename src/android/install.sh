#!/bin/bash

echo "Making sure you are running the emulator"
state=$(/opt/android-sdk/platform-tools/adb get-state)
if [ $state = "unknown" ]; then
    echo "Emulator is not running please start."
else
    cp ../../paintown libs/armeabi/libpaintown.so
    if ant debug; then
        /opt/android-sdk/platform-tools/adb uninstall org.libsdl.app
        /opt/android-sdk/platform-tools/adb install bin/Paintown-debug.apk
    else
        echo "Build failed."
    fi
fi