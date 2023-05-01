// Copyright (c) 2023 Hoefkens Jeroen
#include <efibind.h>
#include <efidef.h>
#include <efidevp.h>
#include <eficon.h>
#include <efiprot.h>
#include <efiapi.h>
#include <efierr.h>

void print_head(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut){
    conOut->OutputString(conOut,L"\r\n###############################################################################");
    conOut->OutputString(conOut,L"\r\n##                              Zero BDProcHot                               ##");
    conOut->OutputString(conOut,L"\r\n##---------------------------------------------------------------------------##");
    conOut->OutputString(conOut,L"\r\n## (c)2023 HoefkensJ                        https://www.github.com/hoefkensj ##");
    conOut->OutputString(conOut,L"\r\n###############################################################################\r\n\r\n");
}

static uint64_t AsmReadMsr64(uint32_t index){
    uint32_t low;
    uint32_t high;
    uint64_t val;

    __asm__ __volatile__ ( "rdmsr" 
    	: "=a" (low),
    	  "=d" (high) 
    	: "c" (index) 
    	);
    val=((uint64_t)high << 32) | low;
    
    return val;
}

static uint64_t AsmWriteMsr64(uint32_t index, uint64_t val){
    uint32_t low;
    uint32_t high;

    low  = (uint32_t)(val);
    high = (uint32_t)(val >> 32);

    __asm__ __volatile__ (
        "wrmsr"
        :
        : "c" (index),
          "a" (low),
          "d" (high)
    );

    return val;
}




// Helper function to print a single bit
void print_bit(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, uint64_t val, int bit)
{
    if (val & ((uint64_t)1 << bit))
        conOut->OutputString(conOut, L"1");
    else
        conOut->OutputString(conOut, L"0");
}

// Print a 64-bit value as binary
void print_bits(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, uint64_t val)
{
    for (int i = 63; i >= 0; i--)
        print_bit(conOut, val, i);
}


EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systemTable)
{
    uint64_t val,nval;
    UINTN index;
   	uint64_t zerobitzero = 18446744073709551614;

    // uint64_t zerobitzero = 1;
    SIMPLE_TEXT_OUTPUT_INTERFACE *conOut = systemTable->ConOut;
    print_head(conOut);
    val = AsmReadMsr64(0x1FC);
    conOut->OutputString(conOut, L"BD PROCHOT:  ");  
    print_bit(conOut,val & 1,0) ; 
    conOut->OutputString(conOut, L"\r\n\r\n");  
    if (val & 1) {   	
    	conOut->OutputString(conOut, L"\r\n0x1FC:  ");   
    	print_bits(conOut, val);
    	conOut->OutputString(conOut, L"\r\n  AND:  ");
    	print_bits(conOut, zerobitzero);
    	conOut->OutputString(conOut, L"\r\n");
    	conOut->OutputString(conOut, L"Disabling BD PROCHOT\r\n");    
    	nval= val & zerobitzero;
    	print_bits(conOut,nval);
    	systemTable->BootServices->WaitForEvent(1, &systemTable->ConIn->WaitForKey, &index);
    	AsmWriteMsr64(0x1FC, 0);
    	conOut->OutputString(conOut, L"BD PROCHOT disabled\r\n");
    	val = AsmReadMsr64(0x1FC);
    	print_bits(conOut, val);
    	conOut->OutputString(conOut, L"\r\n");
    	systemTable->BootServices->WaitForEvent(1, &systemTable->ConIn->WaitForKey, &index);	
	}
    return EFI_SUCCESS;
}