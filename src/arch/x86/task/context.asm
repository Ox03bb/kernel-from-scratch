[BITS 32]

global ctx_save
global ctx_load

ctx_save:
    pushad
    mov eax, esp
    add esp, 32
    ret
    
ctx_load:
    mov esp, [esp + 4]  ; Get saved ESP argument
    popad               ; Restore general-purpose registers
    add esp, 4         ; Skip the saved IRQ vector
    iretd               ; Restore EIP, CS, and EFLAGS