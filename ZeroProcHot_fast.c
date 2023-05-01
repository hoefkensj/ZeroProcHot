//
// ############################################################################
// # REPO: github.com/hoefkensj/ZeroProcHot      AUTHOR: Hoefkens.j@gmail.com #
// # FILE: ZeroProcHot_fast.c                   LICENCE: MIT       2023-05-01 #
// ############################################################################
//
#include <efibind.h>
#include <efidef.h>
#include <efidevp.h>
#include <eficon.h>
#include <efiprot.h>
#include <efiapi.h>
#include <efierr.h>
uint32_t zerobitzero = 4294967294;
uint32_t low;
uint32_t high;

void print_head(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut) {
	conOut->OutputString(conOut, L"\r\n# Zero BDProcHot:Fast #  github.com/hoefkensj #  (c)2023 HoefkensJ #\r\n\r\n"); 
}

static uint64_t AsmReadMsr64() {

    __asm__ __volatile__("rdmsr"
                         : "=a"(low),
                           "=d"(high)
                         : "c"(0x1FC));
    
    return low ;
}
static uint64_t AsmWriteMsr64() {
    __asm__ __volatile__("wrmsr"
                         :
                         : "c"(0x1FC),
                           "a"(low&zerobitzero),
                           "d"(high));
    return AsmReadMsr64(0x1FC);
}


EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systemTable)
{
    SIMPLE_TEXT_OUTPUT_INTERFACE *conOut = systemTable->ConOut;
    print_head(conOut);
    if (AsmReadMsr64() & 1) AsmWriteMsr64();
    return EFI_SUCCESS;
}
