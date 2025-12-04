SHE_X Mini Operating System

Overview
- BIOS bootloader loads stage2 and a higher-half kernel, switches to protected mode, enables paging, and jumps to `0xC0000000`.
- Kernel initializes GDT, physical and virtual memory, interrupts, keyboard, timer, and provides a simple shell.

Prerequisites
- `i686-elf-gcc`, `i686-elf-ld`, `i686-elf-g++`
- `nasm`, `qemu-system-i386`
- `mtools` (`mcopy`), `mkfs.fat` or `mkfs.vfat`, `dd`, `truncate`
- On Windows, use WSL/MSYS for FAT image and Unix tools.

Build
- Check tools: `make tools-check`
- Assemble and run: `make run`
- Debug: `make debug` (QEMU `-s`, GDB attaches and breaks at `kmain`)

Repository Layout
- `boot/stage1` and `boot/stage2`: bootloader, E820 map, GDT, paging setup.
- `kernal/kernel`: entry, core init, shell.
- `kernal/hal`: IDT/PIC setup, x86 port I/O and helpers.
- `kernal/driver`: keyboard, timer, ATA, VGA cursor.
- `kernal/mem`: physical memory bitmap and paging utilities.

Troubleshooting
- Use `make tools-check` to identify missing tools.
- On Windows, run builds inside WSL with required packages.
- If QEMU boots but hangs, use `make debug` and verify CR3 and PDEs in `vmmngr_init`.

