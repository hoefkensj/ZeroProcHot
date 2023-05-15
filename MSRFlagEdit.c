// #include <BaseLib.h>
// #include <UefiLib.h>
#include <efi.h>
#include <efilib.h>
#include <efibind.h>
#include <efidef.h>
#include <efidevp.h>
#include <eficon.h>
#include <efiprot.h>
#include <efiapi.h>
#include <efierr.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>


#define VERSION          (CHAR16*)L"0v503"
#define STR_INTERACTIVE  (CHAR16*)L"MSR EDITOR : Press Esc for interactive mode "
#define HEAD0            (CHAR16*)L"(c)2023 HoefkensJ            Zero BDProcHot 0v502        github.com/hoefkensj"
#define MSG_CONFIRM      (CHAR16*)L"Write Result to System? (Y/n) "

#define MSG_ABORT        (CHAR16*)L"Aborted by user!"
#define MSG_ENABLED      (CHAR16*)L"BD_PROCHOT is now Enabled!  "
#define MSG_DISABLED     (CHAR16*)L"BD_PROCHOT is now Disabled!  "

#define CLEAR_ZERO       0xFFFFFFFFFFFFFFFE
#define SET_ZERO         1
#define MS100            10000
#define CRLF             strOut(tcO, (CHAR16*)L"\r\n")
#define CLS              CLEAR(tcO,FALSE)

#define PRT_MSRLBL       strOut(tcO, (CHAR16*)"MSR ADDRESS:")
// EFI_FILE_HANDLE GetVolume(EFI_HANDLE image)
// {
//   EFI_LOADED_IMAGE *loaded_image = NULL;                  /* image interface */
//   EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;      /* image interface GUID */
//   EFI_FILE_IO_INTERFACE *IOVolume;                        /* file system interface */
//   EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID; /* file system interface GUID */
//   EFI_FILE_HANDLE Volume;                                 /* the volume's interface */

//   /* get the loaded image protocol interface for our "image" */
//   uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (void **) &loaded_image);
//   /* get the volume handle */
//   uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fsGuid, (VOID*)&IOVolume);
//   uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &Volume);
//   return Volume;
// }


EFI_SYSTEM_TABLE *sT;
EFI_BOOT_SERVICES *tbS;
SIMPLE_TEXT_OUTPUT_INTERFACE *tcO;
SIMPLE_INPUT_INTERFACE *tcI;
EFI_TEXT_OUTPUT_STRING PRINT;
EFI_TEXT_RESET CLEAR;
EFI_INPUT_READ_KEY READ;
EFI_STALL SLEEP;
EFI_WAIT_FOR_EVENT evtWait;
EFI_EVENT keyWait;
CHAR16 STR_MSR_ADDR[6];
uint32_t MSR_ADDR = 0x1FC;


uint8_t hexboard(uint8_t current,uint8_t max);

// void ConfirmAndWrite(uint64_t val,CHAR16 *msg) ;
// void choose(uint8_t choice);
// void menu (uint8_t choice);
// void strRow(CHAR16 *head,uint64_t data);
// void strHL(uint8_t n);
// void change_MSR();
// void change_FLAG();
// void clear_FLAG();
// uint64_t read_MSR();
// void set_FLAG();
// uint8_t hexboard(uint8_t current,uint8_t max);
uint8_t menuloop ();
// uint64_t read_0x1fc();
uint8_t KeyTimeout(uint64_t timeout,	uint8_t Progress);


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


char* format_string(const char* format, ...) {
    static char output[1024]; // buffer to hold the output string
    va_list args; // variable argument list
    va_start(args, format); // initialize the argument list

    // use vsnprintf to format the string with the variable arguments
    int num_chars = vsnprintf(output, sizeof(output), format, args);

    va_end(args); // clean up the argument list
		    // check for buffer overflow
    if (num_chars < 0 || num_chars >= sizeof(output)) {
        fprintf(stderr, "Error: output string too long\n");
        return NULL;
    }

    return output;
}


EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systemTable) {
	sT    = systemTable;
	tbS   = sT->BootServices;
	tcO   = sT->ConOut;
	tcI   = sT->ConIn;

	CLEAR = tcO->Reset;
	READ  = tcI->ReadKeyStroke;
	SLEEP = tbS->Stall;

	// uint8_t selected = 0;
  CLS;
	// strOut(tcO, STR_INTERACTIVE) ;
	// if (KeyTimeout(10,1)){
	// 	selected=menuloop();
	// }
    tcO->OutputString(tcO, (CHAR16*)L"test\n");

    char* result = format_string("%d %s\n", 42, "green");
    tcO->OutputString(tcO, (CHAR16*)result);

    // Flush the console output buffer
    tcO->EnableCursor(tcO, FALSE);
    tcO->EnableCursor(tcO, TRUE);

    SLEEP(1000000000);

	return EFI_SUCCESS;
}

// uint8_t
// KeyTimeout(uint64_t timeout,	uint8_t Progress){
// 	CHAR16 keychar,keycode;
// 	uint8_t keypressed = 0;
//   for(int i=0;i<timeout;i++) {
// 		if (i%2 == 1 && Progress == 1){
// 			strOut(tcO, (CHAR16*)'.');
// 		}
// 		EFI_INPUT_KEY key;
// 		tcI->ReadKeyStroke(tcI, &key);
// 		keychar=key.UnicodeChar;
// 		keycode=key.ScanCode;
// 		if (keychar == CHAR_CARRIAGE_RETURN || keycode == SCAN_ESC) {
// 			keypressed=1;
// 			break;
// 		}
// 		SLEEP(MS100);
//   }
//   return keypressed;
// }
//
// uint8_t hexboard(uint8_t current,uint8_t max){
// 	uint64_t index;
// 	CHAR16 keychar;
// 	uint8_t keyint=current;
// KEYIN:
// 		sT->BootServices->WaitForEvent(1, &sT->ConIn->WaitForKey, &index);
// 		EFI_INPUT_KEY key;
// 		sT->ConIn->ReadKeyStroke(sT->ConIn, &key);
// 		keychar=key.UnicodeChar;
// 		if(keychar >= '0' && keychar <= '9'){
// 			keyint=keychar - '0';
// 			goto KEYIN;
// 		}else if(keychar >= 'A' && keychar <= 'F'){
// 			keyint=keychar - 'A'+10;
// 		}else if(keychar >= 'a' && keychar <= 'f'){
// 			keyint=keychar - 'a'+10;
// 		}else if(key.ScanCode == SCAN_UP){
// 			keyint=(keyint+max)%(max+1);
// 		}else if(key.ScanCode == SCAN_DOWN){
// 			keyint=(keyint+1)%(max+1);
// 		}else if (keychar == CHAR_CARRIAGE_RETURN ) {
// 			keyint=keyint & (1 << 7	);
// 		}else if ( key.ScanCode == SCAN_ESC){
// 			keyint=255;
// 		}else{
// 			keyint=current;
// 		}
// 	strOut(tcO, (CHAR16*)'0'+current);
// 	return current;
// }
//


//
// void
// ConfirmAndWrite(uint64_t val,CHAR16 *msg) {
//
//   uint64_t index;
//   strOut(tcO, MSG_CONFIRM);
//   sT->BootServices->WaitForEvent(1, &sT->ConIn->WaitForKey, &index);
//   EFI_INPUT_KEY key;
//   sT->ConIn->ReadKeyStroke(sT->ConIn, &key);
//   scrClear(tcO,FALSE);
//   if ( key.ScanCode	== SCAN_ESC ||
// 	   key.UnicodeChar == 'n'	  ||
// 	   key.UnicodeChar == 'N') {
// 	  toMsg(MSG_ABORT, 20, (CHAR16*)L"");
//   } else {
// 	  AsmWriteMsr64(0x1FC, val);
// 	  toMsg(msg, 20, (CHAR16*)L"");
//   }
// }
//
//
//
// void
// menu (uint8_t choice){
//   CHAR16 *menu[6];
// 	choice=(choice+5)%6;
//
//   menu[0]= (CHAR16*)L"1. Read MSR";
//   menu[1]= (CHAR16*)L"2. Change MSR";
//   menu[2]= (CHAR16*)L"3. Change FLAG";
//   menu[3]= (CHAR16*)L"4. Clear FLAG";
//   menu[4]= (CHAR16*)L"5. Set FLAG";
//   menu[5]= (CHAR16*)L"0. Exit";
//
//   strOut(tcO, (CHAR16*)L"Menu:");
//
//   for (int i = 0; i < 6; i++)  {
// 		if ( i == choice ){
// 			strOut(tcO,  (CHAR16*)L"> ");
// 		} else {
// 			strOut(tcO,  (CHAR16*)L"  ");
// 	}
// 	strOut(tcO,  (CHAR16*)&menu[i]);
// 	CRLF;
//   }
// }
// void
// choose(uint8_t choice){
//   uint64_t index;
//   switch (choice) {
// 	case 1:  read_MSR();
// 	  sT->BootServices->WaitForEvent(1, &sT->ConIn->WaitForKey, &index);
// 	  break;
// 	case 2:
// 	  change_MSR();
// 	  break;
// 	case 3:
// 	  change_FLAG();
// 	  break;
// 	case 4:
// 	  clear_FLAG();
// 	  break;
// 	case 5:
// 	  set_FLAG();
// 	  break;
// 	default: break;
//   }
// }
//
// uint8_t
// menuloop () {
//   uint8_t choice = 1;
//   uint8_t keychar;
//   while (1){
// 	mnu:
// 		CLS;
// 	  strOut(tcO,HEAD0);
//
// 		CRLF;CRLF;
// 	  // strOut(tcO, (AsmReadMsr64(0x1FC) & 1) ? L"1" : L"0" ) ;
// 	  menu(choice);
// 		keychar=hexboard(choice,6);
// 		if (keychar == 255){
// 			choice=0;
// 			break;
// 		}else if (keychar & (1<<7)){
// 			goto mnu;
// 		} else {
// 			choice=keychar;
// 			goto mnu;
// 		}
// 	}
//   return choice;
// }
//
// void
// table(uint64_t term,CHAR16 *op, uint64_t result){
//   strRow(op,term);
// 	CRLF;
//   strHL(65);
//   strRow((CHAR16*)L"RESULT: ",result);
// }
//
// void
// strRow(CHAR16 *head,uint64_t data){
// 	CRLF;
//   strOut(tcO, head);
//   for (int i = 63; i >= 0; i--) {
// 		strOut(tcO,(data & (1ULL << i)) ? (CHAR16*)L"1" : (CHAR16*)L"0");
// 	}
// }
//
// void
// change_MSR() {
//   CHAR16 keychar;
//   uint32_t val;
//   uint64_t index;
//   for (int i = 0; i <= 3; i++) {
// 		EFI_INPUT_KEY key;
// 		sT->BootServices->WaitForEvent(1, &sT->ConIn->WaitForKey, &index);
// 		tcI->ReadKeyStroke(tcI, &key);
// 		keychar=key.UnicodeChar;
// 		if (keychar >= '0' && keychar <= '9') {
// 			val= (uint32_t)(keychar - '0')<< (4*i);
// 			STR_MSR_ADDR[i+2]=keychar;
// 		} else if (keychar >= 'A' && keychar <= 'F') {
// 			val= (uint32_t)(keychar - 55)<< (4*i);
// 		   STR_MSR_ADDR[i+2]=keychar;
// 		} else if (keychar >= 'a' && keychar <= 'f') {
// 			val= (uint32_t)(keychar - 87)<< (4*i);
// 		   STR_MSR_ADDR[i+2]=keychar-32;
// 		} else {
// 		  i--;
// 		}
// 	   MSR_ADDR=MSR_ADDR+val;
// 	}
// }
//
// void
// set_FLAG() {
//   uint64_t val = read_MSR();
//   uint64_t setbitzero = SET_ZERO;
//   uint64_t nval = val | setbitzero;
//   table(setbitzero,(CHAR16*)L"	OR: ",nval);
//   ConfirmAndWrite(nval,MSG_ENABLED);
// }
//
// void
// clear_FLAG(){
//   uint64_t val = read_MSR();
//   uint64_t clearbitzero =  CLEAR_ZERO;
//   uint64_t nval = val & clearbitzero;
//   table(clearbitzero,(CHAR16*)L"   AND: ",nval);
//   ConfirmAndWrite( nval, MSG_DISABLED);
// }
//
// uint64_t
// read_MSR(){
//   uint64_t val = AsmReadMsr64(MSR_ADDR);
//   CHAR16 *tblADR  =(CHAR16*)L"0x1FC: ";
//   strRow(tblADR,val);
//   return val;
// }
// void
// strHL(uint8_t n){
//   for (int i = 0; i <= n; i++) {
//     strOut(tcO,(CHAR16*)L"-");
//   }
// }
