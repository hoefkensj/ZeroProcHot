#!/usr/bin/env bash
gcc -I$PWD/headers -I/usr/include/efi -I/usr/include/efi/x86_64 -I/home/hoefkens/Development/Git/Github/ZeroProcHot_3/efi -DGNU_EFI_USE_MS_ABI -Dx86_64 -fPIC -fshort-wchar -ffreestanding -fno-stack-protector -maccumulate-outgoing-args -Wall -m64 -mno-red-zone -c -oZeroProcHot.c.o ZeroProcHot.c 

 
ld -T /usr/lib64/elf_x86_64_efi.lds -Bsymbolic -shared -nostdlib -znocombreloc /usr/lib64/crt0-efi-x86_64.o -o ZeroProcHot.c.so ZeroProcHot.c.o /usr/lib/gcc/x86_64-pc-linux-gnu/11.3.0/libgcc.a /usr/lib64/libgnuefi.a

objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc -S --target=efi-app-x86_64 ZeroProcHot.c.so ZeroProcHot_x64.efi

sudo cp ZeroProcHot_x64.efi /mnt/qemu/EFI/reFind/drivers_x64/
ls -l ZeroProcHot_x64.efi
md5sum ZeroProcHot_x64.efi
e
