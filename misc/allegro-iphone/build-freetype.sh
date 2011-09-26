#!/bin/bash
function use(){
echo "Usage: $0 [sim|phone] painown-dir"
}
if [ -n "$2" ] && [ -d "$2" ]; then
    if [ "$1" = "sim" ]; then
        make clean
        CFLAGS='-arch i386' LDFLAGS='-arch i386' CC=/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/gcc-4.2 ./configure --prefix=$2/misc/allegro-iphone/install/sim/ && make && make install
    elif [ "$1" = "phone" ]; then
        make clean
        CFLAGS='-arch armv6 -isysroot /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS4.3.sdk -DDARWIN_NO_CARBON' LDFLAGS='-arch armv6 -isysroot /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS4.3.sdk' CC=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/gcc-4.2 ./configure --host=arm --build=i386 --prefix=$2/misc/allegro-iphone/install/phone/ --with-sysroot=/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS4.3.sdk && make && make install
    fi
fi
use
