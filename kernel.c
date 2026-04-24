void print(char *string)
{
    volatile char *uart = (volatile char *)0x10000000;

    while ((*uart = *string++) != '\0') {
        ;
    }
}

void kernel_main(void) {
    // volatile char *hello = (volatile char *)0x10000000;
    // *hello = 'a';
    print("hello");
    for (;;) {}
}
