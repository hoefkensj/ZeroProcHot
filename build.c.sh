#!/usr/bin/env bash
NAME=$1
VER="0v401"
REFINDDRIVERS="$2/EFI/reFind/drivers_x64"


LIBGCC="/usr/lib/gcc/x86_64-pc-linux-gnu/11.3.0/libgcc.a"
LIBGNUEFI="/usr/lib64/libgnuefi.a"
EFIHEADERS="$PWD/headers"
EFIINCLUDE="/usr/include/efi"
EFIINCLUDEX64="/usr/include/efi/x86_64"
ELFX64LDS="/usr/lib64/elf_x86_64_efi.lds"
EFICRT="/usr/lib64/crt0-efi-x86_64.o"
TARGET="efi-app-x86_64"




mkdir -p "$PWD/build/bin"


GCCPARAMS=(
    -I${EFIHEADERS}
    -I${EFIINCLUDE}
    -I${EFIINCLUDEX64}
    -DGNU_EFI_USE_MS_ABI
    -Dx86_64
	-fPIC
	-fshort-wchar 
	-ffreestanding 
	-fno-stack-protector
	-maccumulate-outgoing-args 
	-Wall
	-m64 
	-mno-red-zone 
	-c
)
LDPARAMS=(
	-T ${ELFX64LDS}
	-Bsymbolic 
	-shared 
	-nostdlib 
	-znocombreloc ${EFICRT}
)

OBJCPYPARAMS=(
	-j .text 
	-j .sdata 
	-j .data 
	-j .dynamic 
	-j .dynsym 
	-j .rel 
	-j .rela 
	-j .reloc 
	-S 
	--target=$TARGET	
)

gcc  "${GCCPARAMS[@]}" "-obuild/${NAME}.c.$VER.o" "${NAME}.c" 
 
ld "${LDPARAMS[@]}" "-obuild/${NAME}.c.$VER.so" "build/${NAME}.c.$VER.o" "${LIBGCC}" "${LIBGNUEFI}"

objcopy  "${OBJCPYPARAMS[@]}" "build/${NAME}.c.$VER.so" "build/bin/${NAME}_${VER}_x64.efi"

ls -l "build/bin/${NAME}_${VER}_x64.efi"
md5sum "build/bin/${NAME}_${VER}_x64.efi"
sudo cp -v "$PWD/build/bin/${NAME}_${VER}_x64.efi" "${REFINDDRIVERS}/${NAME}_x64.efi"


