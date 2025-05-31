#!/bin/bash

# Define the toolchain path
TOOLCHAIN_PATH="/home/fts427/data/oppo_a31t/build_src/los_12.1/prebuilts/gcc/linux-x86/arm/arm-eabi-4.8"

# Set environment variables
export ARCH=arm
export SUBARCH=arm
export PATH="$TOOLCHAIN_PATH/bin:$PATH"
export CROSS_COMPILE="$TOOLCHAIN_PATH/bin/arm-eabi-"
export CROSS_COMPILE_ARM32="$TOOLCHAIN_PATH/bin/arm-eabi-"

# Clean the build environment
echo "Cleaning the build environment..."
make clean
make mrproper

# Load the default configuration
make msm8916_a31t_defconfig

# Start building the kernel
echo "Building the kernel..."
make -j$(nproc)

# Check the build result
if [ -f arch/arm/boot/zImage ]; then
    echo "Build completed successfully!"
    echo "zImage image file: arch/arm/boot/zImage"
    
    OUTPUT_DIR="out"
    mkdir -p "$OUTPUT_DIR"
    cp "arch/arm/boot/zImage" "$OUTPUT_DIR/kernel"
    echo "Copied zImage to $OUTPUT_DIR directory"
else
    echo "Build failed!"
    exit 1
fi
