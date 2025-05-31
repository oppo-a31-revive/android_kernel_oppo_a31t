#!/bin/bash

TOOLCHAIN_PATH="/home/fts427/data/oppo_a31t/build_src/los_12.1/prebuilts/gcc/linux-x86/arm/arm-eabi-4.8"

export ARCH=arm
export SUBARCH=arm
export PATH="$TOOLCHAIN_PATH/bin:$PATH"
export CROSS_COMPILE="$TOOLCHAIN_PATH/bin/arm-eabi-"
export CROSS_COMPILE_ARM32="$TOOLCHAIN_PATH/bin/arm-eabi-"

echo "Cleaning..."
make clean
make mrproper

make msm8916_defconfig

echo "Building..."
make -j$(nproc)

if [ -f arch/arm/boot/zImage ]; then
    echo "Done!"
    echo "zImage: arch/arm/boot/zImage"
    
    OUTPUT_DIR="out"
    mkdir -p "$OUTPUT_DIR"
    cp arch/arm/boot/zImage "$OUTPUT_DIR/"
    echo "Copied zImage to $OUTPUT_DIR "
else
    echo "Error!"
    exit 1
fi
