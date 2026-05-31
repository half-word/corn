CROSS  = riscv64-unknown-elf-
CC     = $(CROSS)gcc
LD     = $(CROSS)ld
GDB    = $(CROSS)gdb

CFLAGS = -march=rv64g -mabi=lp64d -ffreestanding -nostdlib -O0 -g -mcmodel=medany
LFLAGS = -T kernel.ld

OBJS   = boot.o kernel.o
TARGET = kernel.elf

.PHONY: build run startqemu connectgdb clean

build: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LFLAGS) -o $@ $^

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: build
	qemu-system-riscv64 -machine virt -bios none -kernel $(TARGET) -nographic

startqemu: build
	qemu-system-riscv64 -machine virt -bios none -kernel $(TARGET) -nographic -S -gdb tcp::1234

connectgdb:
	$(GDB) $(TARGET) -q -iex "set auto-load safe-path $(CURDIR)" -ex "target remote localhost:1234" -ex "break _start" -ex "continue"

clean:
	rm -f $(OBJS) $(TARGET)
