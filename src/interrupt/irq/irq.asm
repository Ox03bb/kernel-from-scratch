[BITS 32]

global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

global irq_table

extern irq_handler


; ============================================================
; IRQ macro
;
; Hardware IRQs do NOT automatically push an error code.
;
; We push only the IRQ number ourselves.
;
; Stack when entering irq_common_stub:
;
;   [ESP]     = IRQ number
;   [ESP + 4] = EIP
;   [ESP + 8] = CS
;   [ESP + 12] = EFLAGS
; ============================================================

%macro IRQ 1

irq%1:
    ; Interrupt gates already clear IF automatically.
    ; This CLI is therefore optional.
    cli

    ; Push IRQ number
    push dword %1

    jmp irq_common_stub

%endmacro


; ============================================================
; IRQ handlers
; ============================================================

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15


; ============================================================
; IRQ table
;
; irq_table[0]  -> irq0
; irq_table[1]  -> irq1
; ...
; irq_table[15] -> irq15
; ============================================================

section .data

irq_table:
    dd irq0
    dd irq1
    dd irq2
    dd irq3
    dd irq4
    dd irq5
    dd irq6
    dd irq7
    dd irq8
    dd irq9
    dd irq10
    dd irq11
    dd irq12
    dd irq13
    dd irq14
    dd irq15


; ============================================================
; Common IRQ handler
; ============================================================

section .text

irq_common_stub:

    ; Save all general-purpose registers
    pusha

    ; After PUSHA:
    ;
    ; [ESP + 0]  = EDI
    ; [ESP + 4]  = ESI
    ; [ESP + 8]  = EBP
    ; [ESP + 12] = original ESP
    ; [ESP + 16] = EBX
    ; [ESP + 20] = EDX
    ; [ESP + 24] = ECX
    ; [ESP + 28] = EAX
    ; [ESP + 32] = IRQ number
    ; [ESP + 36] = EIP
    ; [ESP + 40] = CS
    ; [ESP + 44] = EFLAGS

    ; Pass IRQ number to C
    push dword [esp + 32]

    call irq_handler

    ; Remove argument passed to C
    add esp, 4

    ; Restore registers
    popa

    ; Remove IRQ number
    add esp, 4

    ; Return from interrupt
    iretd