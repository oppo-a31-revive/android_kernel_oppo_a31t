#!/bin/bash

TOOLCHAIN_PATH="/home/fts427/data/oppo_a31t/build_src/los_12.1/prebuilts/gcc/linux-x86/arm/arm-eabi-4.8"

# 设置编译环境变量
export ARCH=arm
export SUBARCH=arm
export PATH="$TOOLCHAIN_PATH/bin:$PATH"
export CROSS_COMPILE="$TOOLCHAIN_PATH/bin/arm-eabi-"
export CROSS_COMPILE_ARM32="$TOOLCHAIN_PATH/bin/arm-eabi-"

# 清理之前的构建
echo "清理之前的构建文件..."
make clean
make mrproper

# 设置配置文件
echo "应用 msm8916_defconfig..."
if ! make msm8916_defconfig; then
    echo "错误: 配置失败"
    exit 1
fi

# 开始编译
echo "开始编译内核..."
make -j$(nproc)

# 检查编译结果
if [ -f arch/arm/boot/zImage ]; then
    echo "内核编译成功！"
    echo "zImage 位置: arch/arm/boot/zImage"
    
    # 复制编译结果到输出目录
    OUTPUT_DIR="out"
    mkdir -p "$OUTPUT_DIR"
    cp arch/arm/boot/zImage "$OUTPUT_DIR/"
    echo "已复制 zImage 到 $OUTPUT_DIR 目录"
else
    echo "编译失败：找不到 zImage"
    exit 1
fi
