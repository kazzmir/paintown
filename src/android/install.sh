#!/bin/bash

adb=/opt/android-sdk/platform-tools/adb
run='am start -a android.intent.action.MAIN -n org.paintown/org.paintown.PaintownActivity'

build_paintown(){
  #(cd ../../..; android=1 make)
  #echo skip
  echo 0
}

build_apk(){
  # ant debug
  echo 0
}

install_apk(){
  # echo "Uninstalling old version"
  # $adb uninstall org.libsdl.app
  echo "Installing latest version"
  $adb install -r bin/Paintown-debug.apk
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
