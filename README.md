# Dan-kernel

My own 64-bit x86 operating system kernel from scratch

## Prerequisites

- [Docker](https://www.docker.com/) for creating our build-environment.
- [Qemu](https://www.qemu.org/) for emulating our operating system.
- A text editor or IDE for writing code.

## Setup

Build an image for our build-environment:

- `docker build . -t dan-os`

## Build

Enter build environment:

- ` docker run --rm -it -v "$(pwd)":/root/env dan-os`

- `make build` : building the kernel
- `make clean` : cleaning the build
- `make help` : for more commands

If you are using Qemu, please close it before running this command to prevent errors.

To leave the build environment, enter `exit`.

## Emulate

You can emulate your operating system using [Qemu](https://www.qemu.org/):

- `qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso`
- You also should be able to load the operating system on a USB drive and boot into it when you turn on your computer, check your BIOS setup to unable secure boot options and enable legacy boot.

## Cleanup

Remove the build-evironment image:

- `docker rmi dan-os -f`

## Dev

Quick dev tip:

Inside of the docker container, run the following:

> apt-get update && apt-get install entr
> find src/ | entr make build

To auto rebuild everytime you change a file on the src directory
Note: you'll need to Ctrl+C and restart the find command if you _create_ a new file

## License

This project is licensed under the GNU GENERAL PUBLIC LICENSE - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [OSDev Wiki](https://wiki.osdev.org/Main_Page)
- [Pritam Zope on codeproject](https://www.codeproject.com/Articles/1225196/Create-Your-Own-Kernel-In-C)
- [Kishore on Medium](https://computers-art.medium.com/writing-a-basic-kernel-6479a495b713)
- [Theo on Medium](https://theogill.medium.com/creating-a-kernel-from-scratch-1a1aa569780f)
- [Petros Koutoupis on Linux Journal](https://www.linuxjournal.com/content/what-does-it-take-make-kernel-0)
- [David Callanan on YouTube](https://www.youtube.com/watch?v=FkrpUaGThTQ)
