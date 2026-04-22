CROSS  = riscv64-unknown-elf-
CC     = $(CROSS)gcc
LD     = $(CROSS)ld

CFLAGS = -march=rv64gc -mabi=lp64d -ffreestanding -nostdlib -O0 -g
LFLAGS = -T kernel.ld

OBJS   = boot.o kernel.o
TARGET = kernel.elf

.PHONY: build run debug clean

build: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LFLAGS) -o $@ $^

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: build
	qemu-system-riscv64 -machine virt -bios none -kernel $(TARGET) -nographic

debug: build
	qemu-system-riscv64 -machine virt -bios none -kernel $(TARGET) -nographic -s -S &
	riscv64-unknown-elf-gdb $(TARGET) -ex "target remote :1234"

clean:
	rm -f $(OBJS) $(TARGET)
