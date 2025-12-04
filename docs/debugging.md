Debugging the Kernel

Attach GDB
- Build and launch: `make debug`
- In GDB: `target remote localhost:1234`
- Load symbols: `symbol-file kernel/kernel.elf`
- Break: `b kmain`, `continue`

Useful Checks
- Read CR3: `info registers cr3`
- Inspect first PDE flags: evaluate `_page_directory[0]` in `vmmngr_init`.
- Verify VGA mapping: check `__VGA_text_memory` symbol and physical `0xB8000`.

Interrupts
- Confirm IDT installed (`lidt`) and PIC remapped to 0x20/0x28.
- Expect timer ISR ticks and keyboard ISR on keypress; spinner should rotate.

