#!/bin/bash

# Exporting NDK and emulator folder
export NDKL="/home/victrid/AUR/osprj/ndk"
export EMUL="/home/victrid/AUR/osprj/sdk/tools"

PATH="${NDKL}${PATH:+:${PATH}}"; export PATH;
PATH="${EMUL}${PATH:+:${PATH}}"; export PATH;

# Exporting cross-compiler location for kernel cross-compiling
export XCOMPILERL="${NDKL}/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin"
PATH="${XCOMPILERL}${PATH:+:${PATH}}"; export PATH;
