CC = gcc
LD = ld
D = -Wall -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -DEFI_FUNCTION_WRAPPER -I /usr/include/efi/x86_64 -I ../../include -c -o

bootx64.o:boot.c
	$(CC) $< -g $(D) $@
bootx64.efi:bootx64.o
	$(LD) $< -g -m elf_x86_64 -nostdlib -znocombreloc -shared -Bsymbolic /usr/lib/crt0-efi-x86_64.o -T /usr/lib/elf_x86_64_efi.lds -L /usr/lib/ -l:libgnuefi.a -l:libefi.a -o bootx64.so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc  --target=efi-app-x86_64 bootx64.so $@
	make clear

.PHONY : clean
ALL_OBJ = bootx64.o bootx64.so
clear:
	-rm $(ALL_OBJ)
