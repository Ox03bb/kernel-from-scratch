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
mov bx, 0
mov di, 0
mov si, disk_address_packet
read_kernel_dap:
cmp di, KERNEL_SECTORS
jae kernel_loaded

mov word [si + 2], 1
mov word [si + 4], bx
mov word [si + 6], es
mov ax, di
inc ax
mov word [si + 8], ax
mov word [si + 10], 0

mov dl, [boot_drive]
mov ah, 0x42
int 0x13
jc chs_load_kernel

add bx, 512
cmp bx, 0
jne dap_next_sector
mov ax, es
add ax, 0x1000
mov es, ax

dap_next_sector:
inc di
jmp read_kernel_dap

chs_load_kernel:
mov di, 0
read_kernel_sector:
cmp di, KERNEL_SECTORS
jae kernel_loaded

mov ax, di
inc ax
xor dx, dx
mov cx, 63
div cx

push dx

xor dx, dx
mov cx, 16
div cx

mov dh, dl
mov ch, al
pop dx
mov cl, dl
inc cl

mov dl, [boot_drive]
mov ah, 2
mov al, 1
int 0x13
jc disk_read_error

add bx, 512
cmp bx, 0
jne next_kernel_sector
mov ax, es
add ax, 0x1000
mov es, ax

next_kernel_sector:
inc di
jmp read_kernel_sector

kernel_loaded:

%include "src/arch/x86/mm/physical/pmm.asm"

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

%include "src/arch/x86/bootloader/gdt.asm"

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
    db 0x10, 0x00
    dw KERNEL_SECTORS
    dw 0x0000
    dw KERNEL_LOAD_SEG
    dq 0x0000000000000001

boot_drive:
    db 0x00

times 510-($-$$) db 0 ;padding  
db 0x55, 0xAA