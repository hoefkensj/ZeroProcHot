#include <efibind.h>
#include <efidef.h>
#include <efidevp.h>
#include <eficon.h>
#include <efiprot.h>
#include <efiapi.h>
#include <efierr.h>
#include <stdlib.h>

EFI_SYSTEM_TABLE *sT;
EFI_BOOT_SERVICES *tbS;
SIMPLE_TEXT_OUTPUT_INTERFACE *tcO;
SIMPLE_INPUT_INTERFACE *tcI;
EFI_TEXT_OUTPUT_STRING strOut;
EFI_TEXT_RESET strClear;
EFI_INPUT_READ_KEY keyRead;
EFI_STALL bsSlp;

EFI_WAIT_FOR_EVENT evtWait;
EFI_EVENT keyWait;   

uint8_t ConfirmAndWrite(uint64_t val,CHAR16 *msg); 
void strHeader();
uint8_t toMsg(CHAR16 *msg, uint64_t to);
void menu (uint8_t choice);
void menuloop ();
void strRow(CHAR16 *head,uint64_t data);
void print_bits(uint64_t val);
void print_bit(uint64_t val, int bit) ;
void clear_ProcHot();
uint64_t read_ProcHot();
void set_ProcHot();
void read_0x1fc();

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
  strClear = tcO->Reset;
  keyRead = tcI->ReadKeyStroke;
  bsSlp = tbS->Stall;
  CHAR16 *msg = L"ZeroProcHot : Press Esc for interactive mode ";
  //clear the screen 
  strClear(tcO,FALSE);  
  if (toMsg(msg,10)){
      strClear(tcO,FALSE);  
      strHeader();
      menuloop();
  } 
  return EFI_SUCCESS;
}

void
strHeader() {
  CHAR16 *head[5];
  head[0] = L"\r\n(c)2023 HoefkensJ";
  head[1] = L"               Zero BDProcHot            ";
  head[2] = L"github.com/hoefkensj";
  head[3] = L"\r\n-------------------------------------------------------------------------------";
  head[4] = L"\r\n";
  for (int i = 0; i < 5; i++) { 
    strOut(tcO, head[i]);
  }
}

uint8_t
toMsg(CHAR16*msg, uint64_t to){
  uint8_t keypressed = 0;
  EFI_INPUT_KEY key;
  strOut(tcO,msg);
  for(int i=0;i<to;i++) {
    // print a dot to the screen
    strOut(tcO, L".");
    keyRead(tcI, &key);
    if (key.ScanCode == SCAN_ESC || key.UnicodeChar == '`') {
      keypressed=1;
      goto end;
    } else {            
      bsSlp(100000);
    }
  }
  end:
  return keypressed;
}


uint8_t
ConfirmAndWrite(uint64_t val,CHAR16 *msg) {
  int result = 1;
  uint64_t index;
  strOut(tcO, L"(Y/n)"); 

  sT->BootServices->WaitForEvent(1, &sT->ConIn->WaitForKey, &index);
  EFI_INPUT_KEY key;
  sT->ConIn->ReadKeyStroke(sT->ConIn, &key);
  if (key.UnicodeChar == 'n' || key.UnicodeChar == 'N') {
      toMsg(L"\r\nAborted by user!\r\n", 10); 
      result = 0;
  } else {
      AsmWriteMsr64(0x1FC, val); 
      toMsg(msg, 10); 
  }
  return result;
}


void
menu (uint8_t choice){
  CHAR16 *menu[4];
  int idx=((choice+3)%4);

  menu[0]= L"1. Read 0x1FC";
  menu[1]= L"2. Clear BD_PROCHOT";
  menu[2]= L"3. Set BD_PROCHOT";
  menu[3]= L"0. Exit";

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

uint8_t 
choose(uint8_t choice){
  uint8_t exit = 0; 
  if (choice == 1){ 
    read_0x1fc();
  } else if (choice == 2){
    clear_ProcHot();
  } else if (choice == 3){
    set_ProcHot();
  } else if (choice == 0){
    exit=1;
  }
  return exit;
}

void
menuloop () {
  // EFI_INPUT_KEY key;
  uint64_t index;
  uint8_t choice = 1;
  uint8_t exit = 0;

  while(1) {
    mnu:
      strClear(tcO,FALSE);  
      strHeader(tcO);
      strOut(tcO, L"\r\n");
      read_ProcHot();
      strOut(tcO, L"\r\n\r\nMenu : ");
      strOut(tcO, L"\r\n-------------------\r\n");
      menu(choice);


    sT->BootServices->WaitForEvent(1, &sT->ConIn->WaitForKey, &index);
    EFI_INPUT_KEY key;
    sT->ConIn->ReadKeyStroke(sT->ConIn, &key);
    strOut(tcO,&key.UnicodeChar);

    if (key.UnicodeChar == '0')  {
      choice=0;
      goto end;
    }else if(key.UnicodeChar == '1'){ 
      choice=1;
      choose(choice);
      goto mnu;
    }else if(key.UnicodeChar == '2'){
      choice=2;
      choose(choice);
      goto mnu;
    }else if(key.UnicodeChar == '3'){
      choice=3;
      choose(choice);
      goto mnu;
    }else if(key.ScanCode == SCAN_UP){
      choice--;
      goto mnu;
    }else if(key.ScanCode == SCAN_DOWN){
      choice++;
      goto mnu;
    }else if(key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      exit=choose(choice);
      if (exit == 1) goto end;
    }else goto mnu;
  }

  end: return; 
}

void
print_bit(uint64_t val, int bit) {
  if (val & ((uint64_t)1 << bit))
      strOut(tcO, L"1");
  else
      strOut(tcO, L"0");
}

void
print_bits(uint64_t val) {
  for (int i = 63; i >= 0; i--) {
      strOut(tcO, (val & (1ull << i)) ? L"1" : L"0");
  }
}

void
strRow(CHAR16 *head,uint64_t data){
  strOut(tcO, L"\r\n");
  strOut(tcO, head);
  print_bits( data);
}

void
set_ProcHot() {
  uint64_t val = AsmReadMsr64(0x1FC);
  uint64_t setbitzero = 1;  
  uint64_t nval = val | setbitzero;
  CHAR16 *tblADR  =L"0x1FC:  ";
  CHAR16 *tblOR   =L"   OR:  ";
  CHAR16 *tblHL   =L"------------------------------------------------------------------------------";
  CHAR16 *tblRES  =L"RESULT:  ";
  strRow(tblADR,val); 
  strRow(tblOR,setbitzero);
  strOut(tcO,tblHL);  
  strRow(tblRES,nval);
  ConfirmAndWrite(nval,L"\r\nBD_PROCHOT is now Enabled!\r\n")
}

void
clear_ProcHot(){
  uint64_t val = AsmReadMsr64(0x1FC);
  uint64_t clearbitzero =  0xFFFFFFFFFFFFFFFE;
  uint64_t nval = val & clearbitzero;    
  CHAR16 *tblADR  =L"\r\n 0x1FC:  ";
  CHAR16 *tblAND  =L"\r\n   AND:  ";
  CHAR16 *tblHL   =L"\r\n------------------------------------------------------------------------------";
  CHAR16 *tblRES  =L"\r\nRESULT:  ";
  strRow(tblADR,val); 
  strRow(tblAND,clearbitzero);
  strOut(tcO,tblHL);  
  strRow(tblRES,nval);
  ConfirmAndWrite( nval, L"\r\nBD_PROCHOT is now Disabled!\r\n"); 
}

void read_0x1fc(){
  uint64_t val = AsmReadMsr64(0x1FC);
  uint64_t clearbitzero =  0xFFFFFFFFFFFFFFFE;
  uint64_t nval = val & clearbitzero;    
  CHAR16 *tblADR  =L"\r\n 0x1FC:  ";
  strRow(tblADR,val); 
}


uint64_t
read_ProcHot(){
  uint64_t val = AsmReadMsr64(0x1FC);
  uint64_t bit = val & 1;
  if (bit == 1){
      strOut(tcO, L"BD_PROCHOT: 1"); 
  }else { 
      strOut(tcO, L"BD_PROCHOT: 0");
  }
  return bit;
}

