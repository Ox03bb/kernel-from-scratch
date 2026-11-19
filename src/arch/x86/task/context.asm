[BITS 32]

global ctx_save
global ctx_load

ctx_save:
    pushad
    
    mov eax, esp
    ret       

ctx_load:
    mov esp, [esp + 4]  ; Get saved ESP argument
    popad               ; Restore general-purpose registers
    ret                 ; Return to the saved execution address