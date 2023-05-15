#include <efibind.h>
#include <efidef.h>
#include <efidevp.h>
#include <eficon.h>
#include <efiprot.h>
#include <efiapi.h>
#include <efierr.h>
#include <stdlib.h>
// #include <Library/BaseLib.h>
// #include <Library/UefiLib.h>
// #include <Library/AsciiStrToUnicodeStr.h>
#define MSR_ADDRESS 0x1FC
#define VERSION "0v402"
#define HEAD0  (CHAR16*)L"(c)2023 HoefkensJ            Zero BDProcHot 0v402        github.com/hoefkensj"
#define HEAD1  (CHAR16*)L"\r\n-------------------------------------------------------------------------------\r\n"
#define TBL_HL  (CHAR16*)L"     = ------------------------------------------------------------------"

#define DEF_STALL 100000
#define MSG_CONFIRM   (CHAR16*)L"\r\n\r\n\r\nWrite Result to System? (Y/n) "
#define MSG_INTERACTIVE  (CHAR16*)L"ZeroProcHot : Press Esc for interactive mode "
#define MSG_ABORT  (CHAR16*)L"\r\nAborted by user!"
#define MSG_ENABLED  (CHAR16*)L"\r\nBD_PROCHOT is now Enabled!  "
#define MSG_DISABLED  (CHAR16*)L"\r\nBD_PROCHOT is now Disabled!  "
#define CLEAR_ZERO 0xFFFFFFFFFFFFFFFE
#define SET_ZERO 1

EFI_SYSTEM_TABLE *sT;
EFI_BOOT_SERVICES *tbS;
SIMPLE_TEXT_OUTPUT_INTERFACE *tcO;
SIMPLE_INPUT_INTERFACE *tcI;
EFI_TEXT_OUTPUT_STRING strOut;
EFI_TEXT_RESET scrClear;
EFI_INPUT_READ_KEY keyRead;
EFI_STALL bsSlp;
EFI_WAIT_FOR_EVENT evtWait;
EFI_EVENT keyWait;


void print()
void ConfirmAndWrite(uint64_t val,CHAR16 *msg) ; 
void choose(uint8_t choice);
void menu (uint8_t choice);

void strRow(CHAR16 *head,uint64_t data);
void print_bits(uint64_t val);
void clear_ProcHot();
void read_ProcHot();
void set_ProcHot();
uint8_t menuloop ();
uint64_t read_MSR();
uint8_t toMsg(CHAR16*msg, uint64_t to,CHAR16 *prog);
uint64_t
AsmReadMsr64(uint32_t addr) {
  uint32_t low;
  uint32_t high;
  uint64_t val;
  __asm__ __volatile__("rdmsr"
                       : "=a"(low),
                         "=d"(high)
                       : "c"(addr));
  val = ((uint64_t)high << 32) | low;
  return val;
}

uint64_t
AsmWriteMsr64(uint32_t addr, uint64_t val) {
  uint32_t low;
  uint32_t high;
  low = (uint32_t)(val);
  high = (uint32_t)(val >> 32);

  __asm__ __volatile__("wrmsr"
                       :
                       : "c"(addr),
                         "a"(low),
                         "d"(high));
  return val;
}

EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systemTable) {
  sT = systemTable;
  tbS = sT->BootServices;
  tcO = sT->ConOut;
  tcI = sT->ConIn;
  strOut = tcO->OutputString;
  scrClear = tcO->Reset;
  keyRead = tcI->ReadKeyStroke;
  bsSlp = tbS->Stall;
  CHAR16 *msg = MSG_INTERACTIVE;
  uint8_t choice = 1 ;
  //clear the screen 
  scrClear(tcO,FALSE);  
  if (toMsg(msg,10,L".")){
    while  (choice != 0){ 
      choice=menuloop();
      choose(choice);
    }
  } 
  return EFI_SUCCESS;
}

uint8_t
toMsg(CHAR16*msg, uint64_t to,CHAR16 *prog){
  uint8_t keypressed = 0;
  EFI_INPUT_KEY key;

  strOut(tcO,msg);
  for(int i=0;i<to;i++) {
    if (i%5 == 0) strOut(tcO, prog);     
    keyRead(tcI, &key);
    if (key.UnicodeChar == CHAR_CARRIAGE_RETURN|| key.ScanCode == SCAN_ESC || key.UnicodeChar == '`') {
      keypressed=1;
      break;
    }
    bsSlp(DEF_STALL);
  }
  return keypressed;
}

void
ConfirmAndWrite(uint64_t val,CHAR16 *msg) {

  uint64_t index;
  strOut(tcO, MSG_CONFIRM); 
  sT->BootServices->WaitForEvent(1, &sT->ConIn->WaitForKey, &index);
  EFI_INPUT_KEY key;
  sT->ConIn->ReadKeyStroke(sT->ConIn, &key);
  scrClear(tcO,FALSE);  
  if ( key.ScanCode    == SCAN_ESC ||
       key.UnicodeChar == 'n'      ||
       key.UnicodeChar == 'N') {      
      toMsg(MSG_ABORT, 20, L""); 
  } else {
      AsmWriteMsr64(0x1FC, val); 
      toMsg(msg, 20, L""); 
  }
}

void
menu (uint8_t choice){
  CHAR16 *menu[4];
  int idx=((choice+3)%4);

  menu[0]= L"1. Read 0x1FC";
  menu[1]= L"2. Clear BD_PROCHOT";
  menu[2]= L"3. Set BD_PROCHOT";
  menu[3]= L"0. Exit";
  strOut(tcO, L"\r\n\r\nMenu : ");
  strOut(tcO, L"\r\n----------------------\r\n");
  for (int i = 0; i < 4; i++)  {
    if ( i == idx){
      strOut(tcO, L"> ");
    } else {
      strOut(tcO, L"  ");
    }
    strOut(tcO, menu[i]);
    strOut(tcO, L"\r\n");
  }
}

void
choose(uint8_t choice){
  uint64_t index;
  switch (choice) {
    case 1:
      read_MSR();
      sT->BootServices->WaitForEvent(1, &sT->ConIn->WaitForKey, &index);
      break;
    case 2:
      clear_ProcHot();
      break;
    case 3:
      set_ProcHot();
      break;
    default: break;
  }
}

uint8_t
menuloop () {
  uint64_t index;
  uint8_t choice = 1;
  CHAR16 keychar;
  EFI_INPUT_KEY key;
  while (1){
    mnu:
      scrClear(tcO,FALSE);  
      strOut(tcO,HEAD0);
      strOut(tcO,HEAD1);
      strOut(tcO, L"\r\n");
      strOut(tcO, L"BD_PROCHOT: ");
      strOut(tcO, (AsmReadMsr64(0x1FC) & 1) ? L"1" : L"0" ) ;

      menu(choice);

    sT->BootServices->WaitForEvent(1, &sT->ConIn->WaitForKey, &index);
    sT->ConIn->ReadKeyStroke(sT->ConIn, &key);
    keychar=key.UnicodeChar;
    if (keychar >= '0' && keychar <= '3') {
      choice = keychar - '0';
      break;
    }else if(key.ScanCode == SCAN_UP){
      choice=(choice+3)%4;
      goto mnu;
    }else if(key.ScanCode == SCAN_DOWN){
      choice=(choice+1)%4;
      goto mnu;
    }else if (keychar == CHAR_CARRIAGE_RETURN ) {
      break;
    }else goto mnu;  
  }
  return choice; 
}

void
table(uint64_t term,CHAR16 *op, uint64_t result){
  CHAR16 *tblHL   = TBL_HL ;
  strRow(op,term); 
  strOut(tcO,L"\r\n");
  strOut(tcO,tblHL);
  strRow(L"RESULT: ",result);
}

void
strRow(CHAR16 *head,uint64_t data){
  print(tcO, L"\r\n");
  print(tcO, head);
  for (int i = 63; i >= 0; i--) {
    strOut(tcO, (data & (1ULL << i)) ? L"1" : L"0");
  }
}

void
set_ProcHot() {
  uint64_t val = read_MSR();
  uint64_t setbitzero = SET_ZERO;  
  uint64_t nval = val | setbitzero;
  table(setbitzero,L"    OR: ",nval);
  ConfirmAndWrite(nval,MSG_ENABLED);
}

void
clear_ProcHot(){
  uint64_t val = read_MSR();
  uint64_t clearbitzero =  CLEAR_ZERO;
  uint64_t nval = val & clearbitzero;    
  table(clearbitzero,L"   AND: ",nval);
  ConfirmAndWrite( nval, MSG_DISABLED); 
}

uint64_t
read_MSR(){
  uint64_t val = AsmReadMsr64(MSR_ADDRESS);
  CHAR16 *tblADR  =L"\r\n 0x1FC: ";
  strRow(tblADR,val);
  return val;  
}

