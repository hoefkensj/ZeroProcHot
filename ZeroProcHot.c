#include <efibind.h>
#include <efidef.h>
#include <efidevp.h>
#include <eficon.h>
#include <efiprot.h>
#include <efiapi.h>
#include <efierr.h>

void print_head(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut) {
    conOut->OutputString(conOut, L"\r\n###############################################################################");
    conOut->OutputString(conOut, L"\r\n##                              Zero BDProcHot                               ##");
    conOut->OutputString(conOut, L"\r\n##---------------------------------------------------------------------------##");
    conOut->OutputString(conOut, L"\r\n## (c)2023 HoefkensJ                        https://www.github.com/hoefkensj ##");
    conOut->OutputString(conOut, L"\r\n###############################################################################\r\n\r\n");
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
void print_bits(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, uint64_t val) {
    int i;
    for (i = 63; i >= 0; i--) {
        conOut->OutputString(conOut, (val & (1ull << i)) ? L"1" : L"0");
    }
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
    UINTN index;

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
    uint64_t nval;
    int yesno;
    uint64_t setbitzero = 1;


    conOut->OutputString(conOut, L"\r\n0x1FC:  ");
    print_bits(conOut, val);
    conOut->OutputString(conOut, L"\r\n   OR:  ");
    print_bits(conOut, setbitzero);
    conOut->OutputString(conOut, L"\r\n");
    conOut->OutputString(conOut, L"\r\nEnabling BD PROCHOT\r\n");
    nval = val | setbitzero;
    print_bits(conOut, nval);
    yesno = getConfirmation(conOut, systemTable);
    if (yesno == 1) {
        AsmWriteMsr64(0x1FC, nval);
        conOut->OutputString(conOut, L"\r\nBD PROCHOT is now enabled!\r\n");
    } else {
        conOut->OutputString(conOut, L"\r\nOperation aborted by user!\r\n");
    }
}
void clear_ProcHot(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut,EFI_SYSTEM_TABLE *systemTable){
    uint64_t val,nval;
    UINTN index;
    int yesno;
    uint64_t zerobitzero = 18446744073709551614;    
    conOut->OutputString(conOut, L"\r\n0x1FC:  ");   
    print_bits(conOut, val);
    conOut->OutputString(conOut, L"\r\n  AND:  ");
    print_bits(conOut, zerobitzero);
    conOut->OutputString(conOut, L"\r\n");
    conOut->OutputString(conOut, L"\r\nDisabling BD PROCHOT\r\n");    
    nval= val & zerobitzero;
    print_bits(conOut,nval);
    yesno=getConfirmation(conOut,systemTable);
    if (yesno == 1) {
        AsmWriteMsr64(0x1FC, nval);
        conOut->OutputString(conOut, L"\r\nBD PROCHOT disabled\r\n");
        val = AsmReadMsr64(0x1FC);
        print_bits(conOut, val);
        conOut->OutputString(conOut, L"\r\n");
        systemTable->BootServices->WaitForEvent(1, &systemTable->ConIn->WaitForKey, &index); 
    //else if 'n' or 'N' skip this section
    }
}




EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systemTable)
{
    uint64_t val;
    UINTN index, i;
    SIMPLE_TEXT_OUTPUT_INTERFACE *conOut = systemTable->ConOut;
    print_head(conOut);
    val = AsmReadMsr64(0x1FC);
    conOut->OutputString(conOut, L"BD PROCHOT:  ");  
    print_bit(conOut,val & 1,0) ; 
    conOut->OutputString(conOut, L"\r\n\r\n");  
    if (val & 1) {      
        clear_ProcHot(conOut,systemTable);
    }else{
        for(i=0;i<500;i++) {
            systemTable->BootServices->WaitForEvent(1, &systemTable->ConIn->WaitForKey, &index);
             EFI_INPUT_KEY key;
            systemTable->ConIn->ReadKeyStroke(systemTable->ConIn, &key);
            // read the key (would be blocking, but since there's a key it returns immediately)
            // here you can do whatever you want with the "key"
            if (key.UnicodeChar == '~' || key.UnicodeChar == 'N') {
                set_ProcHot(conOut,systemTable);
            }

            break;
            }
            // delay 1ms
            systemTable->BootServices->Stall(1000);
        }  

    return EFI_SUCCESS;
}