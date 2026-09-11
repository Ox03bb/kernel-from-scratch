[BITS 32]

extern isr_handler


; ============================================================
; ISR WITHOUT ERROR CODE
; ============================================================

%macro ISR_NOERRCODE 1

global isr%1

isr%1:
    pusha

    push dword %1
    call isr_handler
    add esp, 4

    popa
    iret

%endmacro


; ============================================================
; ISR WITH ERROR CODE
; ============================================================

%macro ISR_ERRCODE 1

global isr%1

isr%1:
    pusha

    push dword %1
    call isr_handler
    add esp, 4

    popa

    ; Remove CPU-pushed error code
    add esp, 4

    iret

%endmacro


; ============================================================
; CPU EXCEPTIONS 0-31
; ============================================================

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7

ISR_ERRCODE   8

ISR_NOERRCODE 9

ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14

ISR_NOERRCODE 15
ISR_NOERRCODE 16

ISR_ERRCODE   17

ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20

ISR_ERRCODE   21

ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29

ISR_ERRCODE   30

ISR_NOERRCODE 31


; ============================================================
; VECTORS 32-255
; ============================================================

%assign i 32

%rep 224

    ISR_NOERRCODE i

%assign i i + 1

%endrep


; ============================================================
; ISR TABLE
; ============================================================

section .rodata

global isr_table

isr_table:

%assign i 0

%rep 256

    dd isr%+i

%assign i i + 1

%endrep