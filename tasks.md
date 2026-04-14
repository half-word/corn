# Build/Run Loop Setup

## What we're building

A pipeline that takes source files, produces an ELF binary, and boots it in QEMU — giving you a tight edit-compile-run loop before there's any "real" kernel to speak of.

## Build pipeline

```
 .S file(s) ──┐
              ├──► riscv64-unknown-elf-gcc ──► .o files ──► riscv64-unknown-elf-ld ──► ELF ──► qemu-system-riscv64
 .c file(s) ──┘                                             (+ linker script)
```

## Boot sequence (what QEMU does when it starts)

```
QEMU virt machine starts
        │
        ▼
Loads your ELF into RAM at 0x80000000
(this address is dictated by the virt machine — it's where RAM starts)
        │
        ▼
Jumps to your ELF entry point
(whatever symbol you declare as the entry in the linker script)
        │
        ▼
Boot stub runs  ← you are here, in assembly, no stack yet
        │
        ▼
Stack pointer set up, jump to C entry point
        │
        ▼
Your C kernel entry runs
```

---

## Tasks

### 1. Linker script

**Why:** The linker doesn't know where your code will live in memory. On a normal OS, the kernel handles that. Here, *you* are the kernel — so you have to tell the linker. The QEMU `virt` machine puts RAM at `0x80000000`, and after the (optional) BIOS/bootloader stage, execution lands there. Your linker script needs to reflect this, and also define the layout of sections (.text, .rodata, .data, .bss) and provide symbols the boot stub can use (e.g. stack top, BSS bounds).

**Reference:** GNU ld manual — "Linker Scripts" chapter. Also the QEMU virt memory map (look for it in the QEMU source under `hw/riscv/virt.c`, or find a documented summary).

- [ ] Write `kernel.ld`

---

### 2. Boot assembly stub

**Why:** When execution reaches your entry point, the hardware state is essentially undefined from C's perspective — there is no valid stack pointer. The RISC-V calling convention (ABI-dictated, not invented) requires `sp` to be valid before you call any C function. The stub's only jobs are: set `sp` to the top of a stack region you defined in the linker script, then jump to your C entry. You may also want to zero the BSS here.

**Reference:** RISC-V unprivileged spec, Chapter 1 (calling convention overview). For BSS zeroing: understand why uninitialized globals are expected to be zero (C standard requirement, not hardware).

- [ ] Write `boot.S`

---

### 3. Minimal C kernel entry

**Why:** You need *something* to jump to, and something observable to confirm it ran. An infinite loop is fine for now — the goal is just to prove the boot sequence completes without crashing. Outputting a character to the UART is the natural next milestone after this loop exists, but don't build that yet.

- [ ] Write `kernel.c` with a bare entry point (infinite loop)

---

### 4. Makefile

**Why:** You'll be recompiling constantly. The key flags to get right: `-march` and `-mabi` (tell the compiler what RISC-V ISA and ABI to target), `-nostdlib` and `-ffreestanding` (no standard library — there's no OS to provide one), and `-T kernel.ld` (use your linker script). You'll also want a `qemu` target so running the kernel is one command.

**Flags to look up:** what `-march=rv64gc` means, what `lp64d` ABI means, why `-nostdlib` is necessary in a bare-metal context.

- [ ] Write `Makefile` with `build` and `run` targets

---

### 5. Boot in QEMU and verify

**Why:** Proving the loop works. Key QEMU flags: `-machine virt` (the target board), `-bios none` (skip the default OpenSBI firmware — you want bare-metal, not SBI-mediated), `-kernel` (load your ELF), `-nographic` (redirect serial to terminal). At this stage "verify" just means QEMU starts, doesn't immediately crash/exit, and you can kill it with `Ctrl-A X`.

- [ ] Run QEMU and confirm it reaches your infinite loop without faulting

---

### 6. GDB hookup (optional but recommended)

**Why:** You'll need this almost immediately once you start doing anything non-trivial. QEMU has a built-in GDB stub (`-s -S` flags). Connecting GDB lets you inspect register state, set breakpoints, and single-step — essential for debugging trap handlers, wrong addresses, etc. Worth wiring up now while the setup is simple.

- [ ] Add a `debug` Makefile target that launches QEMU with `-s -S` and a GDB invocation
