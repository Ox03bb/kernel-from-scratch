[BITS 16]

%define E820_MAX_ENTRIES 32
%define E820_ENTRY_SIZE  24
%define SMAP_SIGNATURE   0x534D4150

; ============================================================
; E820 Memory Map Storage
; ============================================================

; Keep the BIOS-owned data outside the 512-byte boot sector.
memory_map_buffer equ 0x8000
memory_map_count  equ memory_map_buffer + E820_MAX_ENTRIES * E820_ENTRY_SIZE



; ============================================================
; memory_dump
;
; Collect the BIOS E820 memory map.
;
; Output:
;   memory_map_buffer -> E820 entries
;   memory_map_count  -> number of entries
;
; Destroys:
;   EAX, EBX, ECX, EDX, DI
; ============================================================

memory_dump:

    ; --------------------------------------------------------
    ; ES:DI points to the buffer where BIOS writes the entry.
    ;
    ; This example assumes memory_map_buffer is in the
    ; current 64 KiB segment.
    ; --------------------------------------------------------

    push ds
    pop es

    mov di, memory_map_buffer

    ; Number of entries collected
    mov word [memory_map_count], 0

    ; --------------------------------------------------------
    ; EBX = 0 means:
    ; "Start with the first E820 entry."
    ; --------------------------------------------------------

    xor ebx, ebx


.next_entry:

    ; --------------------------------------------------------
    ; EAX = E820h
    ; Select the E820 BIOS service.
    ; --------------------------------------------------------

    mov eax, 0xE820

    ; --------------------------------------------------------
    ; EDX = "SMAP"
    ; Required E820 signature.
    ; --------------------------------------------------------

    mov edx, SMAP_SIGNATURE

    ; --------------------------------------------------------
    ; ECX = size of the buffer.
    ;
    ; Standard E820 entry:
    ;   8 bytes  base
    ;   8 bytes  length
    ;   4 bytes  type
    ;   4 bytes  attributes
    ;
    ; Total = 24 bytes
    ; --------------------------------------------------------

    mov ecx, E820_ENTRY_SIZE

    ; --------------------------------------------------------
    ; ES:DI already points to our output buffer.
    ; --------------------------------------------------------

    int 0x15

    ; --------------------------------------------------------
    ; CF = 1 means BIOS reported an error.
    ; --------------------------------------------------------

    jc .error


    ; --------------------------------------------------------
    ; BIOS should return "SMAP" in EAX.
    ; --------------------------------------------------------

    cmp eax, SMAP_SIGNATURE
    jne .error


    ; --------------------------------------------------------
    ; One entry was successfully written to:
    ;
    ;     ES:DI
    ;
    ; Increment our entry counter.
    ; --------------------------------------------------------

    inc word [memory_map_count]


    ; --------------------------------------------------------
    ; Check whether BIOS has another entry.
    ;
    ; BIOS returns:
    ;
    ;     EBX != 0 -> more entries
    ;     EBX == 0 -> last entry
    ; --------------------------------------------------------

    test ebx, ebx
    jz .done


    ; --------------------------------------------------------
    ; Move DI to the next 24-byte entry.
    ; --------------------------------------------------------

    add di, E820_ENTRY_SIZE


    ; --------------------------------------------------------
    ; Don't allow BIOS to write beyond our 32-entry buffer.
    ; --------------------------------------------------------

    cmp word [memory_map_count], E820_MAX_ENTRIES
    jae .done

    jmp .next_entry


.done:

    clc                    ; success
    jmp PM_start_continue


.error:

    stc                    ; error
    jmp disk_read_error