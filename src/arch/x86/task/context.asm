global ctx_save
global ctx_load

ctx_save:
    pushad

    mov eax, [esp + 36] ; ctx argument
    mov [eax], esp      ; Save stack pointer
    ret

ctx_load:
    mov eax, [esp + 4]  ; ctx argument
    mov esp, [eax]      ; Load saved stack pointer
    popad
    iretd