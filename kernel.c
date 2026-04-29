#include <stdint.h>

#define UART_RECEIVER_REGISTER 0x10000000

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
    volatile char *uart = (volatile char *)UART_RECEIVER_REGISTER;
    *uart = c;
}

void print(char *string)
{
    while (*string != '\0') {
        print_char(*string);
        string++;
    }
}

/*
 * x - decimal number
 * q - quotient
 * r - remainder
 * x = q * 16  + r
 */
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
        print_char(remainder + 'W'); /* W is the ASCII distance (87) to 'a' */
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

void kernel_main(void) {
    uint64_t misa;
    __asm__ volatile ("csrr %0, misa" : "=r"(misa));

    print("misa: ");
    print_hex(misa);
    print("\n");

    for (;;) {}
}
