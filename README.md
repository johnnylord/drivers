# Linux Kernel Driver Programming

This repo is for my own self-study about linux kernel driver programming. In the following sections, I record what I learned and what I did, and save some useful material for future references.

## Useful Commands for kernel development
Find files with the specified pattern recursively in current directory
```bash
$ grep -rnw '.' -e '[PATTERN]'
```
Find directories containing specific file recursively in current directory
```bash
$ find . -type f -name '[PATTERN]' | sed -r 's|/[^/]+$||' | sort | uniq
```

## Customize Linux & Hardware Emulator
System-level programming involves developing and testing kernel modules. You need to familiar yourself with compiling linux kernel and kernel modules from scrach, and run them on the hardware emulator (qemu) for rapid development cycle. In the following sections, we prepare the lightweight root filesystem and default kernel image for the embedded device with `buildroot`. Then, we switch to build a customized kernel image target for an `aarch64` embedding device with the latest vanilla linux kernel source tree combined with the generated rootfs from `buildroot`. Finally, we run the hardware emulator to emulate an `aarch64` device and boot up the system with the customized kernel image and root filesystem.

### Build RootFS with Buildroot
`buildroot` is a handy tool to create lightweight root filesystem with `busybox`. We will use the `qemu_aarch64_virt_defconfig` configuration file to an emulated `aarch64` device.
```bash
$ git submodule update --init --recursive # Clone the buildroot repo
$ cd emulator/buildroot/
$ make qemu_aarch64_virt_defconfig
# Go to "Filesystem Images" and select "cpio the root filesystem"
$ make menuconfig
$ make -j8
# This will generate predefined kernel image and lightweight rootfs under `output/images` directory
```
You can test if the building process is success by running the emulator with the script `output/images/start-qemu.sh`
```bash
$ cd output/images/
$ ./start-qemu.sh   # Make sure you have qemu-aarch64 or qemu-arm64 of version 4.0.0+ installed on your host computer.
```
If you successfully get a login prompt, then you have successfully built the project. (Run `Ctrl+A x` to exit the emulator)

### Build Vanlla Linux Kernel From Scratch
Run the provided script `scripts/vanilla.sh` to download the kernel source tree of version `5.13.9` and the build toolchain for `aarch64` hardware architecture.
```bash
# This will download the kernel source tree in the `emulator` directory and `gcc-aarch64` tool chain under `toolchain` directory.
$ ./scripts/vanilla.sh
```
Then, create environment variables for cross-compiling a kernel image for `aarch64` device.
```bash
$ source ./scripts/envsetup.zsh # if you run in zsh shell

# It exports the following variables
# export PATH="$(pwd)/toolchain/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin:$PATH"
# export CROSS_COMPILE=aarch64-linux-gnu-
# export ARCH=arm64
```
Go to kernel source directory and compile a kernel image with default arm64 configuration
```bash
$ cd emulator/linux-5.13.9/
$ make ARCH=arm64 defconfig # you can ignore ARCH option as we have already set it in the environment variables

# Modift the `.config` file to specify the initramfs with the generated rootfs by buildroot.
# CONFIG_INITRAMFS_SOURCE="[absolute path to the rootfs.cpio in the builtroot project]"
# Type `make menuconfig` again to create autoconf.h for kernel build tools

# Build the kernel image in verbose with cross-compiling toolchain
$ make \
    ARCH=amd64 \
    CROSS_COMPILE=aarch64-linux-gnu- \
    V=1 -j8
```
After the building process, the built image is generated under `arch/arm64/boot` directory.

### Run Hardware QEMU Emulator with RootFS & Kernel Image
`qemu` is a popular hardware emulator for emulating different devices of different hardware architecture. As you are trying to boot a `aarch64` device, we need the `qemu-system-aarch64` or `qemu-aarch64` emulator. Make sure your install the emulator with at least 4.0 version. Then you are good to go to run the emulator.
```bash
$ cd emulator/linux-5.13.9/
$ qemu-system-aarch64 \
    -M virt \
    -cpu cortex-a53 \
    -m 2048M \
    -smp 2 \
    -kernel arch/arm64/boot/Image \
    -append "console=ttyAMA0" \
    -nographic
```

## Documents to read before get your hands dirty
- For host environment setup: [Compile toolchain from scratch](https://gts3.org/2017/cross-kernel.html)
- For host environment setup: [Download precompiled toolchain](scripts/toolchain.sh)
- For driver programming: [LKMPG 5.4](https://sysprog21.github.io/lkmpg/)
- For makefile: [Makefile in linux](https://www.kernel.org/doc/Documentation/kbuild/makefiles.rst)
- For sysfs: [KObject demessify](https://www.kernel.org/doc/Documentation/kobject.txt)

## Environment Setup
Whenever you trying to develop kernel module, make sure you are in a virtual machine. As you are programming in kernel space, wrong code might cause kernel crashing and you need to reboot your computer frequently.

First, download the `vagrant`, `libvirt`, and `qemue` utilities from offical repository in your host computer
```bash
$ sudo apt install -y \
    qemu-kvm \
    libvirt-dev \
    bridge-utils \
    libvirt-daemon-system \
    libvirt-daemon \
    virtinst \
    bridge-utils \
    libosinfo-bin \
    libguestfs-tools \
    virt-top
```
Download libvirt box and boot up the virtual machine with the downloaded box. (You can first modify the provided `Vagrantfile` to sync what directory you want to pass to the virtual machine)
```bash
$ vagrant up --provider=libvirt
```
Then you are good to login into the virtual machine and download essential tools for kernel development.
```bash
$ vagrant ssh
# Inside the virtual environment
$ sudo apt-get update
$ sudo apt-get install -y build-essential kmod
$ sudo apt-get install linux-headers-`uname -r`
```
After that you can test your environment by compiling the passwd example in `/vagrant` and run them.
