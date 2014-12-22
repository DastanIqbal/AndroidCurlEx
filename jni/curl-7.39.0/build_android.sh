#!/bin/bash
NDK=/home/sinbad/Tools/android-ndk-r9d
SYSROOT=$NDK/platforms/android-19/arch-arm/
CFLAGS="-fno-exceptions -Wno-multichar -mthumb -mthumb-interwork -nostdlib -lc -ldl -lm "
LDFLAGS="-Wl,--fix-cortex-a8"
CPPFLAGS="-I/home/sinbad/workspace_new/EMS/TestNDK/jni/curl-7.39.0/include"
export AR="$NDK/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86/bin/arm-linux-androideabi-ar"
export LD="$NDK/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86/bin/arm-linux-androideabi-ld"
export CC="$NDK/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86/bin/arm-linux-androideabi-gcc --sysroot=$SYSROOT"
./configure \
--host=arm-linux-androideabi \
--prefix=/home/sinbad/workspace_new/EMS/TestNDK/jni/curllib/ \
--exec-prefix=/home/sinbad/workspace_new/EMS/TestNDK/jni/curllib/
make clean
make
make install
