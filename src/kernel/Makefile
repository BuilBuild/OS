CC = gcc
LD = ld
D = -g -mgeneral-regs-only -ffreestanding -fno-asynchronous-unwind-tables -fno-strict-aliasing -fno-pic -nostdlib -Wall -I /usr/include/efi/x86_64 -I ../../include

SRC = src

KERNEL_OBJ =  kernel.o bios.o font.o lib.o descriptor.o
kernel.bin: $(KERNEL_OBJ)
	$(LD) -T kernel.lds $(KERNEL_OBJ) -o kernel.elf
	objcopy -O binary kernel.elf $@
	make clear
	
kernel.o: ../kernel/kernel.c
	$(CC) $(D) -c $< -o $@

bios.o: ../bios/bios.c
	$(CC) $(D) -c $< -o $@

lib.o: ../lib/lib.c
	$(CC) $(D) -c $< -o $@

font.o: ../font/font.c
	$(CC) $(D) -c $< -o $@

descriptor.o: ../descriptor/descriptor.c
	$(CC) $(D) -c $< -o $@

.PHONY : clean
ALL_OBJ = $(KERNEL_OBJ)
clear:
	-rm $(ALL_OBJ)
