[BITS 32]

global ctx_save
global ctx_load

ctx_save:
    pushad
    mov eax, esp
    add esp, 32
    ret
    
ctx_load:
    mov ebx, [esp + 4]  ; Saved interrupt frame
    mov eax, [ebx + 12] ; Saved thread ESP
    mov esp, eax

    ; Construct an i386 IRET frame on the continuation stack.
    push dword [ebx + 44]
    push dword [ebx + 40]
    push dword [ebx + 36]

    mov edi, [ebx]
    mov esi, [ebx + 4]
    mov ebp, [ebx + 8]
    mov edx, [ebx + 20]
    mov ecx, [ebx + 24]
    mov eax, [ebx + 28]
    mov ebx, [ebx + 16]
    iretd