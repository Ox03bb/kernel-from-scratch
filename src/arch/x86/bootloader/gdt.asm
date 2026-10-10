[BITS 16]

gdt_start:
    gdt_null:
        dq 0x0
    gdt_code:
        dw 0xFFFF       ; Limte
        dw 0x0000       ; Base
        db 0x00         ; Base
        db 10011010b    ; Access byte
        db 11001111b    ; Flags
        db 0x00         ; Base
    gdt_data:
        dw 0xFFFF       ; Limte
        dw 0x0000       ; Base
        db 0x00         ; Base
        db 10010010b    ; Access byte p=1 , DPL= 00 
        db 11001111b    ; Flags
        db 0x00         ; Base
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; Size of GDT -1
    dd gdt_start 