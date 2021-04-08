#!/bin/bash
set -e
if [ -f "avd.lock" ]; then
exit 1
fi
touch avd.lock
trap "rm -f avd.lock" HUP INT PIPE QUIT TERM
emulator -avd OSPrj-519021911045 -no-window -kernel ./kernel/goldfish/arch/arm/boot/zImage -no-boot-anim -show-kernel
rm -f avd.lock
