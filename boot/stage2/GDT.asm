null_segment:
    dd 0
    dd 0

code_segment:
    dw 0xffff
    dw 0
    dw 0
    db 0
    db 10011010b
    db 11001111b
    db 0

GDT_end:
   gdt_descriptor:
     dw GDT_end - null_segment - 1
     dd null_segment
