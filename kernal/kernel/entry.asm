[bits 32]

extern kmain
global _start

section .text.entry

_start:
    jmp kmain
