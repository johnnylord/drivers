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
