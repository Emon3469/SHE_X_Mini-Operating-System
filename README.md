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

Recent Changes
- Build: linker script path fixed to `kernal/linker.ld`; added `tools-check` target.
- Interrupts: added `interrupts_init()` wrapper to match header/usage.
- Timer: corrected PIT programming to write MSB to data port `0x40`.
- VGA: unified symbol to `__VGA_text_memory` and updated mapping.
- Physical memory: fixed E820 `size_low` usage, 4 KiB alignment, and bitmap range toggling.
- Stage2 boot: corrected GDT descriptor limit/base; added PDE entries before enabling paging.
- Kernel entry: removed extra `esp` subtraction to preserve boot parameters for `kmain`.
- Cursor: now writes both low/high bytes to VGA cursor registers.
- Printing: hex buffer now null-terminated before `monitor_puts`.
- UI: spinner offset uses 80-column calculation for correct screen addressing.
- VM: replaced XOR flag toggles with explicit set/clear helpers.

Verification Notes
- After `make debug`, inspect CR3 and PDEs (identity and `0x300`) before paging jump.
- In shell, confirm spinner rotates and cursor positions correctly after prints.
- Trigger exceptions to view hex diagnostics; verify clean formatting.
- Use `remove_identity_map()` and ensure no page fault occurs entering shell.

