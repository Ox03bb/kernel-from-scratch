[![Assembly](https://img.shields.io/badge/x86%20Assembly-6e4c13?style=flat&logo=assembly&logoColor=white)](https://www.nasm.us/)
[![C](https://img.shields.io/badge/-C/C+-142282?style=flat&logo=C&logoColor=white)](https://en.cppreference.com/w/c)
[![QEMU](https://img.shields.io/badge/-QEMU-FF6600?style=flat&logo=qemu&logoColor=white)](https://www.qemu.org/)
[![GDB](https://img.shields.io/badge/-GDB-4EAA25?style=flat&logo=gnu&logoColor=white)](https://www.gnu.org/software/gdb/)
# kernel-from-scratch
An educational kernel built completely from scratch to explore low-level systems programming, memory management, interrupts, scheduling, and hardware interaction.

## Required Tools:
- [QEMU](https://www.qemu.org/) - An open-source emulator that allows you to run the kernel in a virtual environment.
- [GCC](https://gcc.gnu.org/) - The GNU Compiler Collection, used to compile the kernel code.
- [i686-elf-gcc](https://gcc.gnu.org/) - A cross-compiler for the i686 architecture, used to compile the kernel code for the target architecture.
- [NASM](https://www.nasm.us/) - The Netwide Assembler, used for assembling low-level assembly code.
- [GCC](https://gcc.gnu.org/) - The GNU Compiler Collection, used to compile the kernel code.
- [Make](https://www.gnu.org/software/make/) - A build automation tool that simplifies the compilation process.
- [GDB](https://www.gnu.org/software/gdb/) - The GNU Debugger, used for debugging the kernel during development.
- [PWNdbg](https://github.com/pwndbg/pwndbg) - A TUI debugger for GDB, providing an enhanced debugging experience.

## Project Structure
```
.
├── conf.gdb
├── Dockerfile
├── linker.ld
├── Makefile
├── README.md
├── bin/
├── build/
├── docs/
├── includes/
│   ├── kernel.h
│   ├── panic.h
│   ├── timer.h
│   ├── driver/
│   ├── interrupt/
│   ├── lib/
│   └── utils/
├── scripts/
│   └── requirements.sh
├── src/
│   ├── bootloader/
│   ├── driver/
│   ├── interrupt/
│   ├── kernel/
│   ├── libs/
│   └── utils/
└── test/
```


To install all the required tools, you can use the following commands:

* **if u'r using Linux:**

    ```bash
        sh ./scripts/install.sh
    ```
* **if u'r using Windows:** (shame on you)

    1. Uninstall windows 
    2. Install linux
    3. Run the above command
   
## Getting Started
- clone the repository and navigate to the project directory:

```bash
    git clone https://github.com/Ox03bb/kernel-from-scratch.git
    cd kernel-from-scratch
```

- install the required tools using the provided script:

```bash
    sh ./scripts/install.sh
```

- build the kernel using the Makefile:

```bash
    make setup # to setup the build environment(build docker image...etc) && build the kernel
    # or 
    make build # if u'r already setup the build environment
```

- run the kernel in QEMU:

```bash
    make run
```