# Define variables
CC = i686-elf-gcc
CFLAGS = -ffreestanding -O2 -nostdlib
LDFLAGS = -T linker.ld
OBJS = boot.o kernel.o
TARGET = myos.bin

# Default target
all: $(TARGET)

# Rule to create the final binary
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(CFLAGS) $(OBJS) -lgcc

# Rule to compile kernel.c into kernel.o
kernel.o: kernel.c
	$(CC) -c kernel.c -o kernel.o $(CFLAGS)

# Rule to compile boot.o (assuming you have a boot.s or boot.c file)
boot.o: boot.s
	$(CC) -c boot.s -o boot.o $(CFLAGS)

# Clean target to remove generated files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean

