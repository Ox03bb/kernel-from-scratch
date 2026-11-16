#include "utils/utils.h"

void outb(uint16_t port, uint8_t value) {
    // a mean the the value is in the AL register
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t value;

    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));

    return value;
}

void cli(void) { __asm__ volatile("cli"); }

void sti(void) { __asm__ volatile("sti"); }

void hlt(void) { __asm__ volatile("hlt"); }

void pushad(void) { __asm__ volatile("pushad"); }
void pushfd(void) { __asm__ volatile("pushfd"); }

registers_t pushad_r(void) {
    registers_t registers;

    __asm__ volatile(
        "pushal\n\t"
        "movl 0(%%esp), %[edi]\n\t"
        "movl 4(%%esp), %[esi]\n\t"
        "movl 8(%%esp), %[ebp]\n\t"
        "movl 12(%%esp), %[esp_value]\n\t"
        "movl 16(%%esp), %[ebx]\n\t"
        "movl 20(%%esp), %[edx]\n\t"
        "movl 24(%%esp), %[ecx]\n\t"
        "movl 28(%%esp), %[eax]\n\t"
        "addl $32, %%esp"
        : [edi] "=r"(registers.edi), [esi] "=r"(registers.esi), [ebp] "=r"(registers.ebp),
          [esp_value] "=r"(registers.esp), [ebx] "=r"(registers.ebx), [edx] "=r"(registers.edx),
          [ecx] "=r"(registers.ecx), [eax] "=r"(registers.eax)
        :
        : "memory");

    return registers;
}

uint32_t pushfd_r(void) {
    uint32_t flags;

    __asm__ volatile("pushfl\n\tpopl %0" : "=r"(flags));

    return flags;
}

void popad(void) { __asm__ volatile("popad"); }
void popfd(void) { __asm__ volatile("popfd"); }

void io_wait(void) { // delay for I/O operation

    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
}

uint32_t read_eflags(void) {
    uint32_t flags;
    asm volatile("pushfl\n"
                 "popl %0"
                 : "=r"(flags));
    return flags;
}

uint32_t reg_v(register_name_t reg) {
    uint32_t value;
    uint16_t segment;

    switch (reg) {
    case REG_EAX:
        __asm__ volatile("movl %%eax, %0" : "=r"(value));
        break;
    case REG_EBX:
        __asm__ volatile("movl %%ebx, %0" : "=r"(value));
        break;
    case REG_ECX:
        __asm__ volatile("movl %%ecx, %0" : "=r"(value));
        break;
    case REG_EDX:
        __asm__ volatile("movl %%edx, %0" : "=r"(value));
        break;
    case REG_ESI:
        __asm__ volatile("movl %%esi, %0" : "=r"(value));
        break;
    case REG_EDI:
        __asm__ volatile("movl %%edi, %0" : "=r"(value));
        break;
    case REG_EBP:
        __asm__ volatile("movl %%ebp, %0" : "=r"(value));
        break;
    case REG_ESP:
        __asm__ volatile("movl %%esp, %0" : "=r"(value));
        break;
    case REG_EFLAGS:
        value = read_eflags();
        break;
    case REG_EIP:
        __asm__ volatile("call 1f\n"
                         "1: popl %0"
                         : "=r"(value));
        break;
    case REG_CS:
        __asm__ volatile("movw %%cs, %0" : "=r"(segment));
        value = segment;
        break;
    case REG_DS:
        __asm__ volatile("movw %%ds, %0" : "=r"(segment));
        value = segment;
        break;
    case REG_ES:
        __asm__ volatile("movw %%es, %0" : "=r"(segment));
        value = segment;
        break;
    case REG_FS:
        __asm__ volatile("movw %%fs, %0" : "=r"(segment));
        value = segment;
        break;
    case REG_GS:
        __asm__ volatile("movw %%gs, %0" : "=r"(segment));
        value = segment;
        break;
    case REG_SS:
        __asm__ volatile("movw %%ss, %0" : "=r"(segment));
        value = segment;
        break;
    case REG_CR0:
        __asm__ volatile("movl %%cr0, %0" : "=r"(value));
        break;
    case REG_CR2:
        __asm__ volatile("movl %%cr2, %0" : "=r"(value));
        break;
    case REG_CR3:
        __asm__ volatile("movl %%cr3, %0" : "=r"(value));
        break;
    case REG_CR4:
        __asm__ volatile("movl %%cr4, %0" : "=r"(value));
        break;
    default:
        value = 0;
        break;
    }

    return value;
}