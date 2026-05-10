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

trap:
    j       halt
