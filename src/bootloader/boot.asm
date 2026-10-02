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
mov sp, 0x7000
; Set up data segment
mov ds, ax

; Load kernel
mov ax, KERNEL_LOAD_SEG
mov es, ax
mov [boot_drive], dl
mov si, disk_address_packet
mov ah, 0x42 ; Extended read sectors from disk
int 0x13

jc disk_read_error

%include "src/mm/physical/pmm.asm"

PM_start:
    jmp memory_dump

PM_start_continue:
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

disk_address_packet:
    db 0x10, 0x00              ; packet size, reserved
    dw KERNEL_SECTORS          ; sectors to read
    dw 0x0000                   ; transfer offset
    dw KERNEL_LOAD_SEG          ; transfer segment
    dq 0x0000000000000001      ; start at LBA 1 (sector 2)

boot_drive:
    db 0x00

times 510-($-$$) db 0 ;padding  
db 0x55, 0xAA