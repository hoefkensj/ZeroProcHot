#include <stdio.h>
#include <wchar.h>
//efi
#include <efi.h>
#include <efilib.h>
#include <efibind.h>
#include <efidef.h>
#include <efidevp.h>
#include <eficon.h>
#include <efiprot.h>
#include <efiapi.h>
#include <efierr.h>
//std
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>
#include <efi/efi.h>
#include <efi/efilib.h>

EFI_STATUS
efi_main(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE *systab
    )
{
    InitializeLib(image_handle, systab);

    Print(L"Hello, world!\n");

    return EFI_SUCCESS;
}
