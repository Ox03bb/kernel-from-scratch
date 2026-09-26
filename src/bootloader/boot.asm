[BITS 16]
[ORG 0x7C00]

%include "build/kernel_sectors.inc"


CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

KERNEL_LOAD_SEG equ 0x1000
KERNEL_START_ADDR equ 0x10000



; Set up stack
cli
mov ax, 0x0000
mov ss, ax
mov sp, 0xFFFF
; Set up data segment
mov ds, ax

; Load kernel
mov ax, KERNEL_LOAD_SEG
mov es, ax
xor bx, bx
mov dh, 0x00 ; Head number (Head 0)
mov dl, 0x80 ; Drive number (First hard disk)
mov cl, 0x02 ; Sector number (Sector 2) , becouse sector 1 is bootloader
mov ch, 0x00 ; Cylinder number (Cylinder 0)
mov ah, 0x02 ; Function: Read sectors from disk
mov al, KERNEL_SECTORS
int 0x13

jc disk_read_error

; jmp start

; msg db 13, 10,"Welcom to my bootLoader", 13, 10
;     db "press any key to continue...", 0

; s_msg db 13,10,10,"starting...",0    



; %include "src/bootloader/utils.asm"


; start:

; call clear_screen


; MOV si, msg
; call print

; call get_key

; MOV si, s_msg
; call print
; jmp PM_start


PM_start:
    CLI
    LGDT[gdt_descriptor]
    MOV eax, cr0
    OR eax, 1
    MOV cr0, eax 
    JMP CODE_SEG:PM_main

disk_read_error:
    hlt

%include "src/bootloader/gdt.asm"

; end:
;     JMP $


[BITS 32]

PM_main:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov gs, ax
    mov ebp, 0x9C00
    mov esp, ebp

    in al, 0x92
    or al, 2
    out 0x92, al

    JMP CODE_SEG:KERNEL_START_ADDR

times 510-($-$$) db 0 ;padding  
db 0x55, 0xAA