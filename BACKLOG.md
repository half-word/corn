/# Backlog

Tasks are roughly ordered by priority within each section. Each task includes a *why* — the motivation behind it, not just what it is.

---

## Cleanup

### Remove boot.S / kernel.c test scaffolding
The BSS zeroing verification left a poison write in `boot.S` and a `beef` variable + dead pointer cast in `kernel.c`. This scaffolding served its purpose and should be removed before it causes confusion later. Hardcoded addresses and test artifacts make the code harder to read and reason about.

---

## Print / Output

### Extract `print_char`
The UART write is currently inlined inside `print`. Isolating it into a single-character function gives everything else a clean hardware boundary to build on — all the formatting logic above it becomes pure C with no UART details leaking through.

### Implement `print_hex`
Print an unsigned integer as a hex string (e.g. `0xdeadbeef`). The most immediately useful output primitive for kernel debugging — addresses, register values, and memory contents are all naturally hex. Exercises bitwise operations (nibble extraction via shift and mask) and building a string from an integer.

### Implement `print_uint` / `print_int`
Print a decimal integer. Harder than hex because decimal requires division/modulo and produces digits in reverse order. The interesting problem is how to reverse them — a fixed-size buffer, a recursive approach, or something else. Less critical than hex for kernel debugging, but a good pure-C exercise.

### Implement a variadic `printf`
A format string function: `printf("pc=%x val=%d\n", pc, val)`. Makes all future debugging significantly easier — currently every value requires a separate `print_hex` call with a manual label. Requires variadic arguments (`va_list`, `va_arg`, `va_start`, `va_end`). Worth deciding upfront whether to use `<stdarg.h>` or figure out what the calling convention guarantees about argument layout.

---

## UART

### Read UART status registers before writing
The current `print` writes directly to the UART data register without checking whether it's ready. This works on QEMU because QEMU never actually blocks, but it's not correct. Read the 16550A datasheet, find the Line Status Register, and understand what bit to poll before each byte. Good practice for reading hardware datasheets and the right habit before targeting real hardware.

---

## Trap Handling

### Set up `mtvec` and a minimal trap handler
Currently any exception or interrupt crashes silently — there's no handler, so the CPU jumps to whatever is at the trap vector address (likely garbage). Setting up `mtvec` and a basic handler that at minimum prints something before halting would make debugging dramatically easier. This is hardware-dictated (the privileged spec defines the exact behavior) and is a prerequisite for syscalls and interrupts later. Highest-leverage missing piece in the kernel right now.

## Misc

- Check how to enforce compilation standard to C89
