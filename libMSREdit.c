#include "MSREdit.h"
#include "libMSREdit.h"


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


void
Charset(){                            // ONCE PER LOOKUP
	for (UINT16 i = 0 ; i < 128 ; i ++){
		CHARSETZERO[i*2] =                               // glob CHAR16 CHARSET[62];
			(           i <= 9 )*( 48 + i)           // 10      [0  - 9]
		+ (10 <= i && i <= 35)*( 65 + i-10)        // +26=36  [10 - 35]
		+ (36 <= i && i <= 61)*( 97 + i-36)        // +26=62  [36 - 61]
		+ (62 <= i && i <= 77)*( 32 + i-62)        // +16=78  [62 - 78]
		+ (78 <= i && i <= 83)*( 91 + i-78)        // +6=84   [80 - 85]
		+ (84 <= i && i <= 87)*( 123+ i-84)        // +4=88   [86 - 89]
		+ (88 <= i && i <= 94)*( 58 + i-88);       // +7=95   [90 - 96]
		CHARSET[i]=&CHARSETZERO[i*2];

	}//rof

}// END



void
tcOPrint(CHAR16 *string){
	tcO->OutputString(tcO,(CHAR16*) string);
}

void
setCursor(uint16_t col, uint64_t row){
	tcO->SetCursorPosition(tcO,col,row );
}
void
PrintXY(CHAR16 * string,uint64_t x, uint64_t y 	){
	setCursor(x,y);
	tcOPrint(string);
}

uint8_t
KeyTimeout(uint64_t timeout,	uint8_t Progress){
	CHAR16 keychar,keycode;
	uint8_t keypressed = 0;
  for(int i=0;i<timeout;i++) {
		if (i%2 == 1 && Progress == 1){
			tcOPrint( (CHAR16*)'.');
		}
		EFI_INPUT_KEY key;
		tcI->ReadKeyStroke(tcI, &key);
		keychar=key.UnicodeChar;
		keycode=key.ScanCode;
		if (keychar == CHAR_CARRIAGE_RETURN || keycode == SCAN_ESC) {
			keypressed=1;
			break;
		}
		SLEEP(10000);
  }
  return keypressed;
}

uint8_t
HexBoard(uint8_t current,uint64_t max){
	uint64_t index;
	CHAR16 keychar;
	uint8_t keyint=current;
KEYIN:
	sT->BootServices->WaitForEvent(1, &sT->ConIn->WaitForKey, &index);
	EFI_INPUT_KEY key;
	sT->ConIn->ReadKeyStroke(sT->ConIn, &key);
	keychar=key.UnicodeChar;
	if(keychar >= '0' && keychar <= '9'){
		keyint=keychar - '0';
	}else if(keychar >= 'A' && keychar <= 'F'){
		keyint=keychar - 'A'+10;
	}else if(keychar >= 'a' && keychar <= 'f'){
		keyint=keychar - 'a'+10;
	}else if(key.ScanCode == SCAN_UP){
		keyint=(keyint+max)%(max+1);
	}else if(key.ScanCode == SCAN_DOWN){
		keyint=(keyint+1)%(max+1);
	}else if (keychar == CHAR_CARRIAGE_RETURN ) {
		keyint= 0 - 1 ;
	}else if ( key.ScanCode == SCAN_ESC){
		keyint=254;
	}else{
		keyint=current;
	}
	if (254 > keyint && keyint > max ){
		goto KEYIN;
	}
	return keyint;
}





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

// void tcOPrint(CHAR16 *string);
// void PrintXY(CHAR16 * string,uint64_t x, uint64_t y 	);
// void setCursor(uint16_t col, uint64_t row);
// void Charset();

// uint8_t KeyTimeout(uint64_t timeout,	uint8_t Progress);

// void ConfirmAndWrite(uint64_t val,CHAR16 *msg) ;
// void choose(uint8_t choice);/ CHAR16 *test=HEAD0;
	// uint8_t selected = 0;
  // CLS;
	// strOut(tcO, STR_INTERACTIVE) ;
	// if (KeyTimeout(10,1)){
	// 	selected=menuloop();
	// }

// // void
// // // tcOPrint(CHAR16 *string){
// // // 	tcO->OutputString(tcO,(CHAR16*) string);
// // // }
//
// // void
// // setCursor(uint16_t col, uint64_t row){
// // 	tcO->SetCursorPosition(tcO,col,row );
// // }
// // void
// // PrintXY(CHAR16 * string,uint64_t x, uint64_t y 	){
// //
// // }
// void strRow(CHAR16 *head,uint64_t data);
// void strHL(uint8_t n);
// void change_MSR();
// void change_FLAG();
// void clear_FLAG();
// uint64_t read_MSR();
// void set_FLAG();
// uint8_t hexboard(uint8_t current,uint8_t max);

// uint64_t read_0x1fc();
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
// read_MSR(){L"<^v>"
//   uint64_t val = AsmReadMsr64(MSR_ADDR);
//   CHAR16 *tblADR  =(CHAR16*)L"0x1FC: ";
//   strRow(tblADR,val);
//   return val;
// }
// void
// strHL(uint8_t n){
//   for (int i = 0; i <= n; i++) {L"<^v>"
//     strOut(tcO,(CHAR16*)L"-");
//   }
// }

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
// https://www.youtube.com/watch?v=MrTtsX2Wg9Q
