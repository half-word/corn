void kernel_main(void) {
    char *hello = (char *)0x10000000;
    *hello = 'a';
    for (;;) {}
}
