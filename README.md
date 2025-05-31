# OPPO A31t kernel source

Build: LNX.LA.3.7.3.c2-06500-8939.0-1

Project name: 15011 (Chinese) / 15061 (General)

## Building the kernel

To build the kernel, use the following commands:

```bash
# Example commands (adjust as needed)
export ARCH=arm
export CROSS_COMPILE=arm-eabi-
make a31t_defconfig
make
```

## Device Tree

*   For the Chinese variant (15011), device tree source files are located in:
    `kernel/arch/arm/boot/dts/15011`

*   Use the 15011 device tree to compile `dt.img`.
