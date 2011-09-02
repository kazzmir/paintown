#!/bin/bash

adb=/opt/android-sdk/platform-tools/adb
run='am start -a android.intent.action.MAIN -n org.libsdl.app/org.libsdl.app.SDLActivity'

build_paintown(){
  (cd ../..; android=1 make) && cp ../../paintown libs/armeabi/libpaintown.so
}

build_apk(){
  ant debug
}

install_apk(){
  echo "Uninstalling old version"
  $adb uninstall org.libsdl.app
  echo "Installing latest version"
  $adb install bin/Paintown-debug.apk
}

run_apk(){
  $adb shell $run
}

echo "Making sure you are running the emulator"
state=$($adb get-state)
if [ $state = "unknown" ]; then
    echo "Emulator is not running please start."
else
    build_paintown && build_apk && install_apk && run_apk
fi
