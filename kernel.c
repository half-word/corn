#include <stdint.h>

#define UART_TRANSMITTER_REGISTER           0x10000000
#define MTIME_ADDRESS                       0x200bff8
#define MTIMECMP_0_ADDRESS                  0x2004000
#define MACHINE_TIMER_INTERUPT_CODE         7
#define IS_INTERRUPT                        1
#define TICK_RATE                           1000000 /* CLINT ticks at 10MHz (qemu virt) */

volatile uint64_t tick_counter = 0;

struct saved_registers {
    uint64_t ra;
    uint64_t sp;
    uint64_t gp;
    uint64_t tp;
    uint64_t t0;
    uint64_t t1;
    uint64_t t2;
    uint64_t s0;
    uint64_t s1;
    uint64_t a0;
    uint64_t a1;
    uint64_t a2;
    uint64_t a3;
    uint64_t a4;
    uint64_t a5;
    uint64_t a6;
    uint64_t a7;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
    uint64_t s9;
    uint64_t s10;
    uint64_t s11;
    uint64_t t3;
    uint64_t t4;
    uint64_t t5;
    uint64_t t6;
};

/* TODO: compare agains standard implementation */
int string_length(char *string)
{
    int length;
    length = 0;

    while ((*string++) != '\0') length++;

    return length;
}

void string_reverse(char *string)
{
    int c, i, j;

    for (i = 0, j = string_length(string) - 1; i < j; i++, j--) {
        c = string[i];
        string[i] = string[j];
        string[j] = c;
    }
}

void print_char(char c)
{
    volatile char *uart = (volatile char *)UART_TRANSMITTER_REGISTER;
    *uart = c;
}

void print(char *string)
{
    while (*string != '\0') {
        print_char(*string);
        string++;
    }
}

/* Recursion naturally produces digits high-to-low: we divide down to the most
 * significant nibble before printing anything, so digits come out in the right
 * order on the way back up. No buffer or reversal needed. */
void print_hex_recurse(uint64_t integer)
{
    if (integer == 0) {
        return;
    }

    uint64_t quotient, remainder;

    quotient = integer / 16;
    remainder = integer % 16;

    print_hex_recurse(quotient);

    if (remainder <= 9)
        print_char(remainder + '0');
    else
        print_char(remainder - 10 + 'a'); /* remainder is 10..15; 'a' is 97 */
}

void print_hex(uint64_t integer)
{
    if (integer == 0) {
        print("0x0");
        return;
    }
    print("0x");
    print_hex_recurse(integer);
}

void arm_timer()
{
    // volatile uint64_t *mtime = (volatile uint64_t *)MTIME_ADDRESS;
    volatile uint64_t *mtime_cmp = (volatile uint64_t *)MTIMECMP_0_ADDRESS;
    *mtime_cmp = *mtime_cmp + TICK_RATE;
}

void enable_interrupts()
{
    arm_timer();

    /* enable mstatus.MIE */
    // char mie_mask = 1 << 3;
    __asm__ volatile ("csrrsi zero, mstatus, %0" :: "i"(1 << 3));

    /* enable MIE.MTIE */
    uint64_t mtime_mask = 1 << 7;
    __asm__ volatile ("csrrs zero, mie, %0" :: "r"(mtime_mask));
}

void print_logo()
{
    char *logo =
    "   _________  _________ \n"
    "  / ___/ __ \\/ ___/ __ \\\n"
    " / /__/ /_/ / /  / / / /\n"
    " \\___/\\____/_/  /_/ /_/ \n";
    print(logo);
    print("-------------------------\n");
}

void print_misa()
{
    uint64_t misa;
    __asm__ volatile ("csrr %0, misa" : "=r"(misa));

    print("misa: ");
    print_hex(misa);
    print("\n");
}

void bar()
{
    int a = 909090;
    print("bar: ");
    print_hex((uint64_t)&a);
    print("\n");

    uint64_t sp;
    __asm__ volatile ("mv %0, sp" : "=r"(sp));
    print("sp (bar): ");
    print_hex(sp);
    print("\n");
}

void foo()
{
    int z = 404040;
    print("foo: ");
    print_hex((uint64_t)&z);
    print("\n");
    bar();
}

void kernel_main(void) {
    print_logo();
    enable_interrupts();

    foo();

    uint64_t sp;
    __asm__ volatile ("mv %0, sp" : "=r"(sp));
    print("sp: ");
    print_hex(sp);
    print("\n");

    for (;;) {}
}

void kernel_trap(struct saved_registers *frame)
{
    print("frame: ");
    print_hex((uint64_t)frame);
    print("\n");

    uint64_t mcause;
    __asm__ volatile ("csrr %0, mcause" : "=r"(mcause));

    uint64_t interrupt = mcause >> 63;
    uint64_t exception_code = mcause & ~(1ULL << 63);

    if (interrupt == IS_INTERRUPT && exception_code == MACHINE_TIMER_INTERUPT_CODE) {
        // print("TIMER!\n");

        tick_counter += 1;

        arm_timer();
        return;
    }

    print("Oh no!\n");

    print("mcause: ");
    print_hex(mcause);
    print("\n");

    print("interrupt: ");
    print_hex(interrupt);
    print("\n");

    print("exception_code: ");
    print_hex(exception_code);
    print("\n");

    uint64_t mepc;
    __asm__ volatile ("csrr %0, mepc" : "=r"(mepc));

    print("mepc: ");
    print_hex(mepc);
    print("\n");

    uint64_t mtval;
    __asm__ volatile ("csrr %0, mtval" : "=r"(mtval));

    print("mtval: ");
    print_hex(mtval);
    print("\n");

    mepc = mepc + 4;

    __asm__ volatile ("csrw mepc, %0" :: "r"(mepc));

    return;
}
