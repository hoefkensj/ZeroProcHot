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

uint64_t val,nval;

SIMPLE_TEXT_OUTPUT_INTERFACE *conOut;
EFI_SYSTEM_TABLE *systemTable ;


void print_head(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut) {
    conOut->OutputString(conOut, L"\r\n###############################################################################");
    conOut->OutputString(conOut, L"\r\n##                              Zero BDProcHot                               ##");
    conOut->OutputString(conOut, L"\r\n##---------------------------------------------------------------------------##");
    conOut->OutputString(conOut, L"\r\n## (c)2023 HoefkensJ                        https://www.github.com/hoefkensj ##");
    conOut->OutputString(conOut, L"\r\n###############################################################################\r\n\r\n");
}
void print_menu(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut){
    conOut->OutputString(conOut, L"\r\nChoose Action:");
    conOut->OutputString(conOut, L"\r\n\t1. Clear BC_PROCHOT");
    conOut->OutputString(conOut, L"\r\n\t2. Set BC_PROCHOT");
    conOut->OutputString(conOut, L"\r\n\t0. Exit");
}

void print_bit( uint64_t val, int bit) {
    if (val & ((uint64_t)1 << bit))
        conOut->OutputString(conOut, L"1");
    else
        conOut->OutputString(conOut, L"0");
}

void print_bits(uint64_t val) {
    int i;
    for (i = 63; i >= 0; i--) {
        conOut->OutputString(conOut, (val & (1ull << i)) ? L"1" : L"0");
    }
}

void print_bintable(uint64_t val,uint64_t op,uint64_t nval, const char *opcode){
    conOut->OutputString(conOut, L"\r\n0x1FC:  ");
    print_bits( val);
    conOut->OutputString(conOut, L"\r\n");

    conOut->OutputString(conOut, opcode);
    print_bits( op);
    conOut->OutputString(conOut, L"\r\n");

    conOut->OutputString(conOut, L"------------------------------------------------------------------------------\r\n");
    
    conOut->OutputString(conOut, L"\r\n RESULT :  ");
    print_bits( nval);
    conOut->OutputString(conOut, L"\r\n");
}


static uint64_t AsmReadMsr64(uint32_t index) {
    uint32_t low;
    uint32_t high;
    uint64_t val;

    __asm__ __volatile__("rdmsr"
                         : "=a"(low),
                           "=d"(high)
                         : "c"(index));
    val = ((uint64_t)high << 32) | low;

    return val;
}

static uint64_t AsmWriteMsr64(uint32_t index, uint64_t val) {
    uint32_t low;
    uint32_t high;

    low = (uint32_t)(val);
    high = (uint32_t)(val >> 32);

    __asm__ __volatile__("wrmsr"
                         :
                         : "c"(index),
                           "a"(low),
                           "d"(high));

    return val;
}

int getConfirmation(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut) {
    int result = 1;
    uint64_t index;

    conOut->OutputString(conOut, L"\r\nApply these settings? (Y/n)\r\n");
    // Wait for user input, with 'Y' as default

    systemTable->BootServices->WaitForEvent(1, &systemTable->ConIn->WaitForKey, &index);
    EFI_INPUT_KEY key;
    systemTable->ConIn->ReadKeyStroke(systemTable->ConIn, &key);
    if (key.UnicodeChar == 'n' || key.UnicodeChar == 'N') {
        result = 0;
    }
    return result;
}

void set_ProcHot(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut) {
    uint64_t val = AsmReadMsr64(0x1FC);
    uint64_t setbitzero = 1;  
    uint64_t nval = val | setbitzero;
    const char opcode[6] = "   OR:";

    print_bintable(val,setbitzero,nval,opcode);

    if (getConfirmation( systemTable) == 1) {
        AsmWriteMsr64(0x1FC, nval);
        conOut->OutputString(conOut, L"\r\nBD PROCHOT is now enabled!\r\n");
    } else {
        conOut->OutputString(conOut, L"\r\nOperation aborted by user!\r\n");
    }
}

void clear_ProcHot(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut){
    uint64_t val = AsmReadMsr64(0x1FC);
    uint64_t zerobitzero =  18446744073709551614;
    uint64_t nval = val & zerobitzero;
    const char opcode[6] = "  AND:";

    print_bintable( val,zerobitzero,nval,opcode);
    if (getConfirmation(systemTable) == 1) {
        AsmWriteMsr64(0x1FC, nval);
        conOut->OutputString(conOut, L"\r\nBD PROCHOT disabled\r\n");
    } else {
        conOut->OutputString(conOut, L"\r\nOperation aborted by user!\r\n");
    }
 
}


void interactive(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut,EFI_SYSTEM_TABLE *systemTable){
    uint64_t val= AsmReadMsr64(0x1FC);
    if (val & 1){
        conOut->OutputString(conOut, L"\r\nBD_PROCHOT: 1");  
    }else{
        conOut->OutputString(conOut, L"\r\nBD_PROCHOT: 0");
    }
    print_menu(conOut);
    EFI_INPUT_KEY key;
ask: systemTable->ConIn->ReadKeyStroke(systemTable->ConIn, &key);
    if (key.UnicodeChar == '1')  {
        clear_ProcHot(systemTable);
    }else if (key.UnicodeChar == '2'){
        set_ProcHot(systemTable);
    }else if (key.UnicodeChar == '0' ){
        return;
    }else{
        goto ask;
    }
              
}

EFI_STATUS
efi_main(EFI_HANDLE image,EFI_SYSTEM_TABLE *systemTable )
{

    int i;
    print_head(conOut);
    conOut->OutputString(conOut, L"Press Esc for interactive mode ");
    for(i=0;i<500;i++) {
        // systemTable->BootServices->WaitForEvent(1, &systemTable->ConIn->WaitForKey, &index);
        conOut->OutputString(conOut, L".");
        EFI_INPUT_KEY key;
        systemTable->ConIn->ReadKeyStroke(systemTable->ConIn, &key);
        if (key.ScanCode == SCAN_ESC || key.UnicodeChar == '`') {

            interactive(conOut, systemTable);
            break;
        }
            
        systemTable->BootServices->Stall(1000);
    }    
    return EFI_SUCCESS;

}