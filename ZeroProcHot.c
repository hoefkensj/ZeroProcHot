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

// Helper function to print a single bit
void print_bit(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, uint64_t val, int bit) {
    if (val & ((uint64_t)1 << bit))
        conOut->OutputString(conOut, L"1");
    else
        conOut->OutputString(conOut, L"0");
}
// Print a 64-bit value as binary
void print_bits(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, uint64_t val) {
    int i;
    for (i = 63; i >= 0; i--) {
        conOut->OutputString(conOut, (val & (1ull << i)) ? L"1" : L"0");
    }
}

void print_bintable(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, uint64_t val,uint64_t op,uint64_t nval,char* opcode){
    conOut->OutputString(conOut, L"\r\n0x1FC:  ");
    print_bits(conOut, val);
    conOut->OutputString(conOut, L"\r\n");

    conOut->OutputString(conOut, opcode);
    print_bits(conOut, op);
    conOut->OutputString(conOut, L"\r\n");

    conOut->OutputString(conOut, L"------------------------------------------------------------------------------\r\n");
    
    conOut->OutputString(conOut, L"\r\n RESULT :  ");
    print_bits(conOut, nval);
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

int getConfirmation(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, EFI_SYSTEM_TABLE *systemTable) {
    int result = 1;
    uint64_t index;

    conOut->OutputString(conOut, L"\r\n(Y/n) ");
    // Wait for user input, with 'Y' as default

    systemTable->BootServices->WaitForEvent(1, &systemTable->ConIn->WaitForKey, &index);
    EFI_INPUT_KEY key;
    systemTable->ConIn->ReadKeyStroke(systemTable->ConIn, &key);
    if (key.UnicodeChar == 'n' || key.UnicodeChar == 'N') {
        result = 0;
    }
    return result;
}

void set_ProcHot(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, EFI_SYSTEM_TABLE *systemTable) {
    uint64_t val = AsmReadMsr64(0x1FC);
    uint64_t setbitzero = 1;  
    uint64_t nval = val | setbitzero;
    int yesno;
    char opcode = L"   OR:";
    print_bintable(conOut,val,setbitzero,nval,opcode);
    yesno = getConfirmation(conOut, systemTable);
    if (yesno == 1) {
        AsmWriteMsr64(0x1FC, nval);
        conOut->OutputString(conOut, L"\r\nBD PROCHOT is now enabled!\r\n");
    } else {
        conOut->OutputString(conOut, L"\r\nOperation aborted by user!\r\n");
    }
}

void clear_ProcHot(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut,EFI_SYSTEM_TABLE *systemTable){
    uint64_t val = AsmReadMsr64(0x1FC);
    uint64_t zerobitzero =  18446744073709551614;
    uint64_t nval = val & zerobitzero;
    int yesno;
    char opcode = L"  AND:";
    uint64_t index;

    print_bintable(conOut, val,zerobitzero,nval,opcode);

    yesno=getConfirmation(conOut,systemTable);
    if (yesno == 1) {
        AsmWriteMsr64(0x1FC, nval);
        conOut->OutputString(conOut, L"\r\nBD PROCHOT disabled\r\n");
        val = AsmReadMsr64(0x1FC);
        print_bits(conOut, val);
        conOut->OutputString(conOut, L"\r\n");
        EFI_INPUT_KEY key;
        systemTable->BootServices->WaitForEvent(1, &systemTable->ConIn->WaitForKey, &index); 
    //else if 'n' or 'N' skip this section
    }
}


void interactive(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut,EFI_SYSTEM_TABLE *systemTable){
    if (val & 1){
        conOut->OutputString(conOut, L"\r\nBD_PROCHOT: 1");  
    }else{
        conOut->OutputString(conOut, L"\r\nBD_PROCHOT: 0");
    }
    print_menu(conOut);
    EFI_INPUT_KEY key;
ask: systemTable->ConIn->ReadKeyStroke(systemTable->ConIn, &key);
    if (key.UnicodeChar == '1')  {
        clear_ProcHot(conOut,systemTable);
    }else if (key.UnicodeChar == '2'){
        set_ProcHot(conOut,systemTable);
    }else if (key.UnicodeChar == '0' ){
        return;
    }else{
        goto ask;
    }
              
}

EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systemTable)
{
    uint64_t val;
    UINTN i;
    SIMPLE_TEXT_OUTPUT_INTERFACE *conOut = systemTable->ConOut;
    print_head(conOut);
    val = AsmReadMsr64(0x1FC);
    conOut->OutputString(conOut, L"Press Esc for interactive mode ");
    for(i=0;i<5;i++) {
        // systemTable->BootServices->WaitForEvent(1, &systemTable->ConIn->WaitForKey, &index);
        conOut->OutputString(conOut, L".");
        EFI_INPUT_KEY key;
        systemTable->ConIn->ReadKeyStroke(systemTable->ConIn, &key);
        if (key.ScanCode == SCAN_ESC) {
            print_head(conOut);
            interactive(conOut,systemTable);
            break;
        }
            
        systemTable->BootServices->Stall(100000);
    }    
    return EFI_SUCCESS;

}