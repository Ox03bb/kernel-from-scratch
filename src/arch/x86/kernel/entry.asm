[BITS 32]

section .text.start

global _start
extern kernel_main

%define MEMORY_MAP_BUFFER 0x8000
%define MEMORY_MAP_COUNT  0x8300

_start:
    cli

    mov dword [boot_info + 0], MEMORY_MAP_BUFFER
    movzx eax, word [MEMORY_MAP_COUNT]
    mov dword [boot_info + 4], eax

    push dword boot_info
    call kernel_main
    add esp, 4

    cli

.hang:
    hlt
    jmp .hang


section .data

boot_info:
    dd 0          ; memory_map_address
    dd 0          ; memory_map_count