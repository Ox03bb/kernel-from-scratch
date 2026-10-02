# GDB & Pwndbg — Kernel Debugging Reference

A practical reference for debugging a 32-bit x86 kernel with **GDB + QEMU + pwndbg**.

---

## 1. Basic GDB Concepts

GDB mainly gives you several ways to inspect and control a program:

| Command          | Meaning                                             |
| ---------------- | --------------------------------------------------- |
| `run` / `r`      | Start a local program                               |
| `continue` / `c` | Continue execution                                  |
| `next` / `n`     | Execute next source line, stepping over functions   |
| `step` / `s`     | Execute next source line, stepping into functions   |
| `finish`         | Execute until the current function returns          |
| `break` / `b`    | Set a breakpoint                                    |
| `delete`         | Delete breakpoints                                  |
| `info`           | Display debugger information                        |
| `print` / `p`    | Evaluate and print an expression                    |
| `x`              | Examine raw memory                                  |
| `display`        | Automatically display an expression after each stop |
| `watch`          | Stop when a value changes                           |
| `list` / `l`     | Show source code                                    |
| `bt`             | Show the call stack                                 |
| `quit` / `q`     | Exit GDB                                            |

---

# 2. Connecting GDB to QEMU

For kernel development, QEMU is normally started with:

```bash
qemu-system-i386 ... -S -s
```

### `-S`

Start QEMU **paused**.

The CPU does not execute until GDB tells it to continue.

### `-s`

Start a GDB server on TCP port `1234`.

Then connect from GDB:

```gdb
target remote :1234
```

You should see something similar to:

```text
Remote debugging using :1234
```

Then:

```gdb
continue
```

or:

```gdb
c
```

to start execution.

---

# 3. Restarting a QEMU Kernel

When using:

```gdb
target remote :1234
```

GDB is controlling a remote target.

Therefore:

```gdb
run
```

does **not** restart the kernel.

You will get:

```text
The "remote" target does not support "run".
```

Instead:

1. Stop QEMU.
2. Start QEMU again with `-S -s`.
3. Reconnect:

```gdb
target remote :1234
```

4. Continue:

```gdb
continue
```

Breakpoints can normally remain in GDB.

---

# 4. `p` — Print / Evaluate Expressions

The most important command for inspecting C variables is:

```gdb
p expression
```

For example:

```gdb
p start_addr
```

GDB evaluates the C expression and prints the result.

---

## 4.1 Print in hexadecimal

```gdb
p/x start_addr
```

Example:

```text
$1 = 0x10000
```

`/x` means **hexadecimal**.

---

## 4.2 Print decimal

```gdb
p/d start_addr
```

---

## 4.3 Print unsigned decimal

```gdb
p/u start_addr
```

---

## 4.4 Print binary

```gdb
p/t start_addr
```

`t` means binary representation.

---

## 4.5 Print character

```gdb
p/c variable
```

---

## 4.6 Print address

```gdb
p/x &variable
```

`&` is the C **address-of operator**.

For example:

```gdb
p/x &kernel_start_addr
```

might produce:

```text
$1 = 0x10000
```

---

# 5. `p/x` vs `x/...`

This distinction is extremely important.

## `p`

```gdb
p/x expression
```

means:

> Evaluate a C expression and print its value.

Example:

```gdb
p/x start_addr
```

---

## `x`

```gdb
x/... address
```

means:

> Examine memory starting at this address.

Example:

```gdb
x/8xb pmm_bitmap
```

means:

> Examine 8 bytes starting at `pmm_bitmap`.

### Simple rule

```text
p = What is the value of this expression?

x = What is stored in memory at this address?
```

---

# 6. The `x` Command

General syntax:

```gdb
x/NFU ADDRESS
```

Where:

```text
N = number of units
F = display format
U = unit size
```

Example:

```gdb
x/16xb pmm_bitmap
```

means:

```text
16 = display 16 units
x  = hexadecimal
b  = byte
```

---

# 7. Memory Display Formats

Common formats:

| Format | Meaning             |
| ------ | ------------------- |
| `x`    | Hexadecimal         |
| `d`    | Signed decimal      |
| `u`    | Unsigned decimal    |
| `o`    | Octal               |
| `t`    | Binary              |
| `c`    | Character           |
| `s`    | String              |
| `i`    | Machine instruction |

Examples:

```gdb
x/8xb address
x/8db address
x/8tb address
x/4xw address
x/10i address
```

---

# 8. Memory Unit Sizes

| Unit | Meaning    |    Size |
| ---- | ---------- | ------: |
| `b`  | byte       |  1 byte |
| `h`  | halfword   | 2 bytes |
| `w`  | word       | 4 bytes |
| `g`  | giant word | 8 bytes |

Examples:

```gdb
x/16xb address
```

16 bytes.

```gdb
x/4xw address
```

4 × 4-byte words = 16 bytes.

```gdb
x/2xg address
```

2 × 8-byte values = 16 bytes.

---

# 9. Inspecting Your PMM Bitmap

Suppose:

```c
static uint8_t pmm_bitmap[BITMAP_SIZE];
```

You can inspect it with:

```gdb
x/16xb pmm_bitmap
```

Binary:

```gdb
x/16tb pmm_bitmap
```

Decimal:

```gdb
x/16ub pmm_bitmap
```

---

## Inspect one bitmap byte

```gdb
p/x pmm_bitmap[2]
```

or:

```gdb
x/1xb pmm_bitmap+2
```

These inspect the same byte, but in different ways.

---

# 10. Bitmap Frame Calculation

Your PMM uses:

```text
FRAME_SIZE = 4096
```

and:

```text
1 bit = 1 physical frame
```

Therefore:

```text
frame = address / FRAME_SIZE
```

Bitmap location:

```text
byte = frame / 8
bit  = frame % 8
```

For example:

```text
physical address = 0x10000
```

Then:

```text
frame = 0x10000 / 0x1000
      = 16
```

Therefore:

```text
bitmap byte = 16 / 8
            = 2

bit = 16 % 8
    = 0
```

So frame `0x10000` is represented by:

```text
pmm_bitmap[2], bit 0
```

You can inspect it:

```gdb
p/x pmm_bitmap[2]
```

---

# 11. Checking PMM Functions

You can directly call a C function from GDB.

For example:

```gdb
p pmm_check_mm(pmm_bitmap, 0x10000)
```

If the function returns `true`:

```text
$1 = 0x1
```

If it returns `false`:

```text
$1 = 0x0
```

You can also check a bitmap index:

```gdb
p pmm_check_mm_index(pmm_bitmap, 16)
```

---

# 12. Breakpoints

Set a breakpoint:

```gdb
break pmm_reserve
```

or:

```gdb
b pmm_reserve
```

Continue:

```gdb
continue
```

When execution reaches the function, GDB stops there.

---

## Break at a source line

```gdb
break pmm.c:25
```

or:

```gdb
b pmm.c:25
```

---

## Break at an address

Useful for assembly/kernel debugging:

```gdb
break *0x10000
```

The `*` means:

> Break at this exact machine address.

---

# 13. Inspect Function Arguments

Suppose GDB stops inside:

```c
void pmm_reserve(
    uint8_t *bitmap,
    uintptr_t start_addr,
    uintptr_t end_addr
)
```

You can inspect:

```gdb
p/x bitmap
p/x start_addr
p/x end_addr
```

For your kernel you expect:

```text
start_addr = 0x10000
end_addr   = 0x3c762
```

You can also inspect:

```gdb
p/x first_frame
p/x end_frame
```

---

# 14. Stepping Through Code

### `next`

```gdb
next
```

Execute the next source line without entering a function.

Shortcut:

```gdb
n
```

---

### `step`

```gdb
step
```

Enter a function if the next line calls one.

Shortcut:

```gdb
s
```

---

### `finish`

```gdb
finish
```

Continue until the current function returns.

This is useful when you accidentally stepped into a function and want to return to the caller.

---

# 15. Continue Execution

```gdb
continue
```

Shortcut:

```gdb
c
```

Execution continues until:

* a breakpoint is hit
* a watchpoint triggers
* an exception occurs
* the CPU halts
* you interrupt execution

---

# 16. Call Stack

Show the current call stack:

```gdb
backtrace
```

or:

```gdb
bt
```

Example:

```text
#0  pmm_reserve()
#1  kernel_main()
#2  _start()
```

This tells you:

```text
Who called this function?
```

---

## More detailed stack

```gdb
bt full
```

This also shows local variables.

---

# 17. Select a Stack Frame

Suppose:

```gdb
bt
```

shows:

```text
#0 pmm_reserve()
#1 kernel_main()
#2 _start()
```

Select frame 1:

```gdb
frame 1
```

or:

```gdb
f 1
```

Then inspect variables belonging to that function:

```gdb
info locals
```

---

# 18. Local Variables

Show local variables:

```gdb
info locals
```

Example:

```text
bitmap = 0x1b2a0
start_addr = 0x10000
end_addr = 0x3c762
first_frame = 0x10
end_frame = 0x3d
```

Show function arguments:

```gdb
info args
```

---

# 19. Registers

Show all CPU registers:

```gdb
info registers
```

or:

```gdb
i r
```

Specific register:

```gdb
p/x $eax
p/x $ebx
p/x $ecx
p/x $edx
```

Important x86 registers:

```text
EAX
EBX
ECX
EDX
ESI
EDI
EBP
ESP
EIP
EFLAGS
```

Segment registers:

```text
CS
DS
ES
FS
GS
SS
```

---

# 20. Important Registers for Kernel Debugging

Instruction pointer:

```gdb
p/x $eip
```

Stack pointer:

```gdb
p/x $esp
```

Base pointer:

```gdb
p/x $ebp
```

General-purpose registers:

```gdb
p/x $eax
p/x $ebx
p/x $ecx
p/x $edx
```

---

# 21. Examine Instructions

To see assembly at the current instruction:

```gdb
x/i $eip
```

Several instructions:

```gdb
x/10i $eip
```

Example:

```text
0x10000 <_start>: cli
0x10001 <_start+1>: call 0x...
0x10006 <_start+6>: cli
```

---

# 22. Disassemble a Function

```gdb
disassemble pmm_reserve
```

or:

```gdb
disas pmm_reserve
```

For Intel syntax:

```gdb
set disassembly-flavor intel
```

Then:

```gdb
disas pmm_reserve
```

---

# 23. Inspect Source Code

```gdb
list
```

or:

```gdb
l
```

Show a specific function:

```gdb
list pmm_reserve
```

Show around a specific line:

```gdb
list pmm.c:25
```

---

# 24. Watchpoints

A watchpoint stops execution when a value changes.

For example:

```gdb
watch pmm_bitmap[2]
```

Now GDB stops whenever that byte changes.

This is extremely useful for your PMM.

For example:

```gdb
watch pmm_bitmap[2]
continue
```

You can discover exactly which operation modifies the bitmap.

---

# 25. Hardware Watchpoints

For memory that is frequently changed, use:

```gdb
awatch expression
```

or:

```gdb
rwatch expression
```

Basic distinction:

```text
watch  → stop when value is written/changed
rwatch → stop when value is read
awatch → stop when value is read or written
```

Hardware support and exact behavior depend on the target.

---

# 26. Display an Expression Automatically

Instead of repeatedly typing:

```gdb
p/x pmm_bitmap[2]
```

use:

```gdb
display/x pmm_bitmap[2]
```

GDB will display it every time execution stops.

List displays:

```gdb
info display
```

Remove a display:

```gdb
undisplay 1
```

---

# 27. Inspecting Strings

If memory contains a C string:

```gdb
x/s address
```

For example:

```gdb
x/s 0x10000
```

---

# 28. Inspecting Arrays

Suppose:

```c
uint8_t bitmap[16];
```

You can use:

```gdb
p/x bitmap
```

or:

```gdb
x/16xb bitmap
```

For a larger array:

```gdb
x/128xb bitmap
```

---

# 29. `p` with Array Elements

You can inspect individual elements:

```gdb
p/x bitmap[0]
p/x bitmap[1]
p/x bitmap[2]
```

You can also calculate:

```gdb
p/x bitmap[2] & 1
```

This is useful for checking individual bits.

---

# 30. Testing Individual Bitmap Bits

For frame 16:

```gdb
p/x pmm_bitmap[2]
```

Check bit 0:

```gdb
p/x pmm_bitmap[2] & (1 << 0)
```

Check bit 1:

```gdb
p/x pmm_bitmap[2] & (1 << 1)
```

Check bit 7:

```gdb
p/x pmm_bitmap[2] & (1 << 7)
```

Nonzero means the bit is set.

---

# 31. Kernel Linker Symbols

For linker symbols such as:

```c
extern uint8_t kernel_start_addr;
extern uint8_t kernel_end_addr;
```

use:

```gdb
p/x &kernel_start_addr
p/x &kernel_end_addr
```

For your kernel:

```text
kernel_start = 0x10000
kernel_end   = 0x3c762
```

The `&` is important because the linker symbols represent addresses.

---

# 32. Physical vs Virtual Memory

Before paging is enabled, your kernel may have:

```text
virtual address ≈ physical address
```

After paging is enabled, this is no longer necessarily true.

GDB:

```gdb
x/16xb 0x10000
```

examines the target's address space.

When debugging paging, you must distinguish:

```text
virtual address
physical address
```

QEMU's monitor has commands that can inspect physical memory directly.

---

# 33. QEMU Monitor

QEMU provides a monitor interface.

If configured appropriately, you can enter the QEMU monitor and use commands such as:

```text
info registers
```

and:

```text
xp /16bx 0x10000
```

`xp` is particularly useful because it examines **physical memory**.

This distinction becomes important after paging is enabled.

---

# 34. Useful QEMU Monitor Commands

```text
info registers
```

Display CPU registers.

```text
info mem
```

Display memory mappings when supported.

```text
xp /16bx 0x10000
```

Examine physical memory.

```text
quit
```

Exit QEMU.

---

# 35. Checking PMM Allocation

After:

```c
uintptr_t address = pmm_alloc_frame(pmm_bitmap);
```

you can inspect:

```gdb
p/x address
```

Then:

```gdb
p pmm_check_mm(pmm_bitmap, address)
```

It should return:

```text
0x1
```

because the allocated frame should now be marked used.

---

# 36. Testing Kernel Reservation

Your kernel:

```text
0x10000 → 0x3c762
```

should reserve frames:

```text
0x10000
0x11000
0x12000
...
0x3c000
```

Test:

```gdb
p pmm_check_mm(pmm_bitmap, 0x10000)
p pmm_check_mm(pmm_bitmap, 0x11000)
p pmm_check_mm(pmm_bitmap, 0x13000)
p pmm_check_mm(pmm_bitmap, 0x3c000)
```

Expected:

```text
1
1
1
1
```

A frame after the kernel:

```gdb
p pmm_check_mm(pmm_bitmap, 0x3d000)
```

may be free, depending on your E820 memory map and other reservations.

---

# 37. Testing `pmm_reserve()`

Set a breakpoint:

```gdb
break pmm_reserve
continue
```

When stopped:

```gdb
p/x start_addr
p/x end_addr
p/x first_frame
p/x end_frame
```

Expected:

```text
start_addr  = 0x10000
end_addr    = 0x3c762
first_frame = 0x10
end_frame   = 0x3d
```

Then step through:

```gdb
next
```

and inspect:

```gdb
p/x pmm_bitmap[2]
```

---

# 38. Debugging `pmm_alloc_frame()`

Breakpoint:

```gdb
break pmm_alloc_frame
```

Continue:

```gdb
continue
```

Inspect:

```gdb
p/x bitmap
p/x i
p/x bitmap[i]
```

After finding a free bit:

```gdb
p/x n
```

Then inspect the returned address:

```gdb
finish
p/x $eax
```

For a 32-bit kernel, a function returning a pointer/integer commonly returns it in `EAX`.

---

# 39. Useful Expression Calculations

GDB can calculate directly:

```gdb
p/x 0x3c762 - 0x10000
```

Kernel size:

```gdb
p/d 0x3c762 - 0x10000
```

Number of frames:

```gdb
p/d (0x3c762 - 0x10000) / 0x1000
```

Round up:

```gdb
p/x (0x3c762 + 0x1000 - 1) / 0x1000
```

---

# 40. Breakpoint Management

List breakpoints:

```gdb
info breakpoints
```

Delete breakpoint number 2:

```gdb
delete 2
```

Delete all breakpoints:

```gdb
delete
```

Disable:

```gdb
disable 2
```

Enable:

```gdb
enable 2
```

Temporary breakpoint:

```gdb
tbreak pmm_reserve
```

It automatically disappears after being hit.

---

# 41. Conditional Breakpoints

Break only when a condition is true:

```gdb
break pmm_reserve if start_addr == 0x10000
```

This is very useful when a function is called many times.

Another example:

```gdb
break pmm_alloc_frame if i == 2
```

---

# 42. Catching Kernel Crashes

When the kernel crashes, immediately inspect:

```gdb
bt
```

Then:

```gdb
info registers
```

Then:

```gdb
x/10i $eip
```

And:

```gdb
x/32xb $esp
```

For a page fault or other exception, inspect the relevant CPU state and stack.

---

# 43. Inspecting the Stack

```gdb
x/16xw $esp
```

Display 16 words starting from the current stack pointer.

Binary:

```gdb
x/16tw $esp
```

---

# 44. Inspecting CPU State Quickly

A useful sequence:

```gdb
info registers
x/10i $eip
x/16xw $esp
bt
```

This gives you:

1. Registers
2. Current instructions
3. Stack contents
4. Call stack

---

# 45. Common GDB Mistakes

## Mistake 1 — Using `run` with remote QEMU

Wrong:

```gdb
run
```

when connected using:

```gdb
target remote :1234
```

Use QEMU restart +:

```gdb
target remote :1234
continue
```

---

## Mistake 2 — Confusing a value with an address

For example:

```gdb
p/x kernel_start_addr
```

is not necessarily the same as:

```gdb
p/x &kernel_start_addr
```

The first evaluates the object.

The second obtains its address.

For linker symbols declared as objects:

```c
extern uint8_t kernel_start_addr;
```

you generally want:

```gdb
p/x &kernel_start_addr
```

---

## Mistake 3 — Using an array element as an address

This:

```gdb
x/16xb pmm_bitmap[2]
```

is wrong for examining bitmap byte 2 because `pmm_bitmap[2]` is a byte value.

Use:

```gdb
x/16xb pmm_bitmap+2
```

or:

```gdb
x/1xb &pmm_bitmap[2]
```

---

## Mistake 4 — Forgetting `*` for an address breakpoint

Function:

```gdb
break pmm_reserve
```

Address:

```gdb
break *0x10000
```

---

# 46. Useful Aliases

GDB accepts short versions of many commands:

```text
p       → print
x       → examine
b       → break
c       → continue
n       → next
s       → step
r       → run
f       → frame
bt      → backtrace
l       → list
i r     → info registers
```

For kernel debugging, the most frequently used commands will probably be:

```gdb
b
c
n
s
p
x
bt
info registers
disas
```

---

# 47. A Practical PMM Debugging Session

A typical debugging session for your current PMM code:

```gdb
target remote :1234
```

Set a breakpoint:

```gdb
break pmm_reserve
```

Start execution:

```gdb
continue
```

Check the kernel boundaries:

```gdb
p/x start_addr
p/x end_addr
```

Check frame calculations:

```gdb
p/x first_frame
p/x end_frame
```

Expected:

```text
start_addr  = 0x10000
end_addr    = 0x3c762
first_frame = 0x10
end_frame   = 0x3d
```

Inspect the relevant bitmap:

```gdb
x/8tb pmm_bitmap+2
```

Check individual frames:

```gdb
p pmm_check_mm(pmm_bitmap, 0x10000)
p pmm_check_mm(pmm_bitmap, 0x11000)
p pmm_check_mm(pmm_bitmap, 0x13000)
p pmm_check_mm(pmm_bitmap, 0x3c000)
```

Then test allocation:

```gdb
break pmm_alloc_frame
continue
```

Inspect:

```gdb
p/x i
p/x bitmap[i]
p/x n
```

After returning:

```gdb
finish
p/x $eax
```

Then verify:

```gdb
p pmm_check_mm(pmm_bitmap, $eax)
```

---

# 48. Most Useful Commands to Memorize

If you do not want to memorize the entire reference, memorize these:

```gdb
target remote :1234
continue
break function
next
step
finish
backtrace
info locals
info args
info registers
p/x expression
p/d expression
p/t expression
p/x &variable
x/16xb address
x/16tb address
x/10i $eip
disassemble function
watch variable
display/x expression
```

For your PMM specifically:

```gdb
p/x start_addr
p/x end_addr
p/x first_frame
p/x end_frame

p/x pmm_bitmap[2]
x/16tb pmm_bitmap

p pmm_check_mm(pmm_bitmap, 0x10000)

break pmm_reserve
break pmm_alloc_frame
```

---

# 49. Mental Model

The easiest way to remember the most important distinction is:

```text
                 GDB
                  │
        ┌─────────┴─────────┐
        │                   │
        ▼                   ▼
       p                   x
     "value"             "memory"
        │                   │
        ▼                   ▼
 p/x variable        x/16xb address
 p/x &variable       x/16tb address
 p/x a + b            x/10i $eip
 p function()         x/16xw $esp
```

And for your PMM:

```text
Physical address
      │
      ▼
 address / FRAME_SIZE
      │
      ▼
   frame number
      │
      ├───────────────┐
      ▼               ▼
 frame / 8         frame % 8
      │               │
      ▼               ▼
 bitmap byte        bitmap bit
```

That is the core calculation behind:

```c
bitmap[frame / 8] & (1U << (frame % 8))
```

---

# 50. Quick Cheat Sheet

```text
# Connect
target remote :1234

# Execution
c                   continue
n                   next
s                   step
finish              finish current function

# Breakpoints
b function
b file.c:line
b *0xADDRESS
info breakpoints
delete N
disable N
enable N

# Variables
p variable
p/x variable
p/d variable
p/t variable
p/x &variable
info locals
info args

# Memory
x/16xb address      16 bytes, hex
x/16tb address      16 bytes, binary
x/4xw address       4 words, hex
x/10i $eip          10 instructions
x/s address         string

# CPU
info registers
p/x $eax
p/x $ebx
p/x $esp
p/x $ebp
p/x $eip

# Stack
bt
bt full
x/16xw $esp

# Code
list
disas function

# Memory changes
watch variable
rwatch variable
awatch variable

# Automatic display
display/x expression
info display
undisplay N

# PMM
p/x start_addr
p/x end_addr
p/x first_frame
p/x end_frame
p/x pmm_bitmap[2]
x/16tb pmm_bitmap
p pmm_check_mm(pmm_bitmap, 0x10000)
```
