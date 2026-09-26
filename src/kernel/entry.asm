[BITS 32]

section .text.start


global _start
extern kernel_main



_start:
    cli
    ; call print_kernel_loaded
    call kernel_main

    cli
.hang:
    hlt
    jmp .hang

; ----------------------------------------
; Print "Kernel is loaded"
; ----------------------------------------
; print_kernel_loaded:
;     pushad

;     mov edi, 0xB8000          ; VGA text buffer
;     mov esi, kernel_msg

; .print:
;     lodsb                     ; AL = *ESI++
;     test al, al
;     jz .done

;     mov ah, 0x0F              ; White text on black background
;     stosw                     ; Write AX (character + attribute)
;     jmp .print

; .done:
;     popad
;     ret

; kernel_msg db "Hello from kernel!", 0