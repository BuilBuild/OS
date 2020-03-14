CC = gcc
LD = ld
D = -Wall -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -DEFI_FUNCTION_WRAPPER -I /usr/include/efi/x86_64 -I ../../include -c -o

.PHONY : all
all:
	make bootx64.efi
	make kernel.bin

.PHONY : bootx64.efi
bootx64.efi:
	cd ./src/uefi && make bootx64.efi

	cp ./src/uefi/bootx64.efi ./
	
	rm ./src/uefi/bootx64.efi

.PHONY : kernel.bin
kernel.bin:
	cd ./src/kernel && make kernel.bin

	cp ./src/kernel/kernel.bin ./
	cp ./src/kernel/kernel.elf ./

	rm ./src/kernel/kernel.bin
	rm ./src/kernel/kernel.elf

.PHONY : clean
ALL_OBJ = bootx64.efi kernel.bin
clear:
	-rm $(ALL_OBJ)
