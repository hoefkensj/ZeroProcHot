#include <efi.h>
#include <efilib.h>
#include <efibind.h>
#include <efidef.h>
#include <efidevp.h>

#include <efiprot.h>
#include <efiapi.h>
#include <efierr.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>



#define VERSION          (CHAR16*)L"0v503"
#define STR_INTERACTIVE  (CHAR16*)L"MSR EDITOR : Press Esc for interactive mode "
#define ARROWS           (CHAR16*)L"<^v>"
#define HEAD0            (CHAR16*)L"(c)2023 HoefkensJ            Zero BDProcHot 0v605        github.com/hoefkensj"
#define ACTIVEMSR        (CHAR16*)L"MSR ADRESS:"
#define ACTIVEFLAG       (CHAR16*)L"FLAG BIT:"
#define MSG_CONFIRM      (CHAR16*)L"Write Result to System? (Y/n) "

#define MSG_ABORT        (CHAR16*)L"Aborted by user!"
#define MSG_ENABLED      (CHAR16*)L"BD_PROCHOT is now Enabled!  "
#define MSG_DISABLED     (CHAR16*)L"BD_PROCHOT is now Disabled!  "
#define CRLF             (CHAR16*)L"\r\n"
#define CLEAR_ZERO       0xFFFFFFFFFFFFFFFE
#define SET_ZERO         1
#define MS100            10000

#define CLS              CLEAR(tcO,FALSE)

#define PRT_MSRLBL       strOut(tcO, (CHAR16*)"MSR ADDRESS:")


EFI_SYSTEM_TABLE               *sT;
EFI_BOOT_SERVICES              *tbS;
SIMPLE_TEXT_OUTPUT_INTERFACE   *tcO;
SIMPLE_INPUT_INTERFACE         *tcI;
EFI_TEXT_OUTPUT_STRING          STROUT;
EFI_TEXT_RESET                  CLEAR;
EFI_INPUT_READ_KEY              READ;
EFI_STALL                       SLEEP;
EFI_WAIT_FOR_EVENT              WAITevt;
EFI_EVENT                       WAITKEY;
EFI_TEXT_SET_CURSOR_POSITION    SETCURSOR;

EFI_TEXT_ENABLE_CURSOR          CURSOR;


CHAR16                         *CHARSET[128];
CHAR16                         CHARSETZERO[256];

// CHAR16 STR_MSR_ADDR[6];
// uint32_t MSR_ADDR = 0x1FC;
void printMenu(CHAR16 *menu[]);
void shortcuts(EFI_SYSTEM_TABLE *systemTable);
void tcOPrint(CHAR16 *string);
void PrintXY(CHAR16 * string,uint64_t x, uint64_t y 	);
void setCursor(uint16_t col, uint64_t row);
void Charset();
uint8_t HexBoard(uint8_t current,uint64_t max);
uint8_t KeyTimeout(uint64_t timeout,	uint8_t Progress);
uint8_t menu (uint8_t choice);
uint8_t menu (uint8_t choice);





EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systemTable) {
	shortcuts(systemTable);
	uint64_t selected = 0;
	Charset();
	tcOPrint(HEAD0);
	tcOPrint(CRLF);
	selected = menu(1);
	tcOPrint(CHARSET[selected]);

	KeyTimeout(1000 , 0);
	return EFI_SUCCESS;
}


void shortcuts( EFI_SYSTEM_TABLE *systemTable){
		sT    = systemTable;
	tbS   = sT->BootServices;
	tcO   = sT->ConOut;
	tcI   = sT->ConIn;

	CLEAR     = tcO->Reset;
	CURSOR    = tcO->EnableCursor;
	SETCURSOR = tcO->SetCursorPosition;

	READ      = tcI->ReadKeyStroke;
	SLEEP     = tbS->Stall;

}

UINT8
menu (UINT8 selected){
  CHAR16 *menu[6];
	CHAR16 *slct[2];

	slct[0]= (CHAR16 *) L"  ";
	slct[1]= (CHAR16 *) L"> ";
  menu[1]= (CHAR16 *) L"Change MSR";
  menu[2]= (CHAR16 *) L"Change FLAG";
  menu[3]= (CHAR16 *) L"Clear FLAG";
  menu[4]= (CHAR16 *) L"Set FLAG";
  menu[0]= (CHAR16 *) L"Exit";
	printMenu(menu);


	CHAR16 *arrow;
	uint8_t current , item;
	current=selected;

SELECT:
	selected=current;
  for (int i = 1; i < 6; i++)  {
		arrow=select[((i%5 == selected ) ? 1 : 0 )];
		PrintXY(arrow,0,9+i) ;
	}
	item=HexBoard(current,4);
	if (item == 254){
		goto ENDMENU;
	} else if (item==255) {
		selected=current;
		goto ENDMENU;
	} else {
		current=item;
		goto SELECT;
	}
ENDMENU:
return selected;
}

void printMenu(CHAR16 *menu[]){
	setCursor(0,8);
  tcOPrint((CHAR16*)L"Menu:");
  for (int i = 1; i < 6; i++)  {
		PrintXY(CHARSET[i%5],3,9+i) ;
		tcOPrint((CHAR16 *)L". ") ;
		tcOPrint(menu[(i)%5]);
		tcOPrint(CRLF);
	}
}

// void MSRAdrress(){
// 	uint64_t default_address=0x01FC;
// 	CHAR16 *strADDR[4];
//
// 	}

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
Charset(){
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
	uint8_t end=0;
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
	}else if(key.ScanCode == SCAN_LEFT){
		current=keyint;
		end=1;
	}else if(key.ScanCode == SCAN_RIGHT){
		current=keyint;
		end=1;
	}else if (keychar == CHAR_CARRIAGE_RETURN ) {
		current=keyint;
		end=1;
	}else if ( key.ScanCode == SCAN_ESC){
		keyint=current;
		end=1;
	}else{
		keyint=current;
	}
	if (end==0){
		goto KEYIN;
	}
	return keyint;
}


