.section .text.boot
.global _start

_start:
    la      sp, _stack_top      # Setting the stack pointer to the top of the stack

    la      t0, __bss_start     # Loading __bss_start into a temporary register
    la      t1, __bss_end       # Loading __bss_end into a temporary register
1:
    bgeu    t0, t1, 2f          # jump to 2 if t0 >= t1
    sd      zero, 0(t0)         # zero what t0 points to
    addi    t0, t0, 8           # increment t0 by 8
    j       1b                  # jump back to 1

2:
    la      t0, trap            # load the address of trap in t0
    csrw    mtvec, t0           # write what's in t0 into mtvec
    call    kernel_main         # call the kernel_main symbol (go into C)

halt:
    j       halt                # infinite loop

.align 4
trap:
    # sp must be saved before we move it, so we pre-save it at the slot it will
    # occupy once the frame is allocated. sp's slot is offset 8 in the frame, so
    # its pre-allocation address is: old_sp - 248 + 8 = old_sp - 240.
    sd      sp, -240(sp)
    addi    sp, sp, -248        # allocate frame: 31 registers × 8 bytes
    sd      ra, 0(sp)
    sd      gp, 16(sp)
    sd      tp, 24(sp)
    sd      t0, 32(sp)
    sd      t1, 40(sp)
    sd      t2, 48(sp)
    sd      s0, 56(sp)
    sd      s1, 64(sp)
    sd      a0, 72(sp)
    sd      a1, 80(sp)
    sd      a2, 88(sp)
    sd      a3, 96(sp)
    sd      a4, 104(sp)
    sd      a5, 112(sp)
    sd      a6, 120(sp)
    sd      a7, 128(sp)
    sd      s2, 136(sp)
    sd      s3, 144(sp)
    sd      s4, 152(sp)
    sd      s5, 160(sp)
    sd      s6, 168(sp)
    sd      s7, 176(sp)
    sd      s8, 184(sp)
    sd      s9, 192(sp)
    sd      s10, 200(sp)
    sd      s11, 208(sp)
    sd      t3, 216(sp)
    sd      t4, 224(sp)
    sd      t5, 232(sp)
    sd      t6, 240(sp)
    mv      a0, sp
    call    kernel_trap
    ld      ra, 0(sp)
    ld      gp, 16(sp)
    ld      tp, 24(sp)
    ld      t0, 32(sp)
    ld      t1, 40(sp)
    ld      t2, 48(sp)
    ld      s0, 56(sp)
    ld      s1, 64(sp)
    ld      a0, 72(sp)
    ld      a1, 80(sp)
    ld      a2, 88(sp)
    ld      a3, 96(sp)
    ld      a4, 104(sp)
    ld      a5, 112(sp)
    ld      a6, 120(sp)
    ld      a7, 128(sp)
    ld      s2, 136(sp)
    ld      s3, 144(sp)
    ld      s4, 152(sp)
    ld      s5, 160(sp)
    ld      s6, 168(sp)
    ld      s7, 176(sp)
    ld      s8, 184(sp)
    ld      s9, 192(sp)
    ld      s10, 200(sp)
    ld      s11, 208(sp)
    ld      t3, 216(sp)
    ld      t4, 224(sp)
    ld      t5, 232(sp)
    ld      t6, 240(sp)
    addi    sp, sp, 248
    mret
