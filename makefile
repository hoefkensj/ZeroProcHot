
C_FLAGS		= \
			-c \
			-DGNU_EFI_USE_MS_ABI \
			-D${ARCH} \
			-ffreestanding \
			-fno-stack-protector \
			-fPIC \
			-fshort-wchar \
			-m64 \
			-maccumulate-outgoing-args \
			-mno-red-zone \
			-Wall \
			-DGNU_EFI_USE_MS_ABI

C_INCLUDES	= \
			-I${PWD}/headers/efi \
			-I/usr/include/efi \
			-I/usr/include/efi/${ARCH} \
			-I/usr/include/efi/protocol \







ARCH		= $(shell uname -m | sed s,i[3456789]86,ia32,)

OBJS		= MSRFlagEdit.c.o
TARGET		= MSRFlagEdit.c.efi


LIB		= /usr/lib64
EFILIB		= /usr/lib64
EFI_CRT_OBJS	= $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS		= $(EFILIB)/elf_$(ARCH)_efi.lds

CFLAGS		= $(C_INCLUDES)  $(C_FLAGS)

ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

LDFLAGS         = -nostdlib -znocombreloc $(EFICRT) -T $(EFI_LDS) -T $(ELFX64LDS) -shared \
		  -Bsymbolic -L $(EFILIB) -L $(LIB) $(EFI_CRT_OBJS)





all: $(TARGET)

MSRFlagEdit.c.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@

