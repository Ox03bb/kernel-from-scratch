[BITS 16]

print:
    .next:
        lodsb               ; AL = [SI], SI++
        test al, al         ; Is AL == 0?
        jz .done

        mov ah, 0x0E
        int 0x10
        jmp .next

    .done:
        ret

get_key:
    MOV ah, 0x00
    INT 0x16
    ret

clear_screen:
    mov ax, 0x0003
    int 0x10
    ret