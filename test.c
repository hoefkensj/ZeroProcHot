//efi
#include <efi.h>
#include <efilib.h>
#include <efibind.h>
#include <efidef.h>
#include <efidevp.h>
#include <eficon.h>
#include <efiprot.h>
#include <efiapi.h>
#include <efierr.h>
//std
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>

#define C16              (CHAR16*)
#define MS100            100000
#define CRLF             tcO->OutputString(tcO, (CHAR16*)L"\r\n")
#define CLS              CLEAR(tcO,FALSE)


#define VERSION          C16"0v503"
#define STR_INTERACTIVE  C16"MSR EDITOR : Press Esc for interactive mode "
#define HEAD0            C16"(c)2023 HoefkensJ            MSR EDITOR 0v502        github.com/hoefkensj"

struct MenuItem {
	CHAR16 *select;
	int inum;
	CHAR16 snum;
	CHAR16 *label;
};

EFI_SYSTEM_TABLE *sT;
EFI_BOOT_SERVICES *tbS;
SIMPLE_TEXT_OUTPUT_INTERFACE *tcO;
SIMPLE_INPUT_INTERFACE *tcI;
EFI_TEXT_OUTPUT_STRING PRINT;
EFI_TEXT_RESET CLEAR;
EFI_INPUT_READ_KEY READ;
EFI_STALL SLEEP;
CHAR16 CHARS[255];
CHAR16 LINE[78];
struct MenuItem rMenu[10];



uint8_t menu(uint8_t current);


// CHAR16* format_string(CHAR16* format, ...) {
//     static CHAR16 output[1024]; // buffer to hold the output string
//     va_list args; // variable argument list
//     va_start(args, format); // initialize the argument list
//
//     // use vsnprintf to format the string with the variable arguments
//     int num_chars = vsnprintf(NULL, 0, format, args);
//     if (num_chars < 0) {
//         return NULL;
//     }
//
//     char* tmp = malloc((num_chars + 1) * sizeof(char));
//     if (tmp == NULL) {
//         return NULL;
//     }
//
//     snprintf(tmp, num_chars + 1, format, args);
//
//     va_end(args); // clean up the argument list
//
//     // convert the char string to a wide char string
//     wchar_t* woutput = char_to_wchar(tmp);
//
//     // copy the wide char string to the output buffer
//     wcsncpy(output, woutput, sizeof(output) / sizeof(output[0]) - 1);
//     output[sizeof(output) / sizeof(output[0]) - 1] = L'\0';
//
//     free(tmp);
//     free(woutput);
//
//     return output;
// }


void charset(){
	CHAR16 czero=L'0';
	// CHAR16 izero=(CHAR16) czero;
	for (int i = 0; i <= 10; i++){
		CHARS[i]= (czero+i);
	}
	czero=L'A';
	// izero=(int)czero;
	for (int i = 0; i <= 26; i++){
		CHARS[10+i]=(CHAR16)(czero+i);
	}
}
EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systemTable) {
	// int num;
	// CHAR16 linebuffer[80];

  sT    = systemTable;
  tbS   = sT->BootServices;
  tcO   = sT->ConOut;
  tcI   = sT->ConIn;
	PRINT = tcO->OutputString;
	CLEAR = tcO->Reset;
	READ  = tcI->ReadKeyStroke;
	SLEEP = tbS->Stall;
	charset();
	for (int i = 0; i <= 2; i++){
			tcO->OutputString(tcO,CHARS[i]);
	}

	// C16 &linebuffer);

	SLEEP(1000000);
	menu(2);
	SLEEP(100000000);
;	return EFI_SUCCESS;
}

uint8_t  menu(uint8_t current) {
	for (int i = 0; i <= 10; i++){
		if (i == current ){
			rMenu[i].select=L" >";
		} else {
			rMenu[i].select=L"  ";
		}
		rMenu[i].inum=i;
		rMenu[i].snum= CHARS[i];
	}
	rMenu[0].label=C16 L". Exit";
	rMenu[1].label=C16 L". Change Adress";
  rMenu[2].label=C16 L". Change FLAG bit";
//   menu[3]= (CHAR16*)L"4. Clear FLAG";
//   menu[4]= (CHAR16*)L"5. Set FLAG";
//   menu[5]= (CHAR16*)L"0. Exit";



	for (int i = 1; i <= 5; i++){
		tcO->OutputString(tcO,C16 rMenu[i].select);
		tcO->OutputString(tcO,C16 &rMenu[i].snum);
		tcO->OutputString(tcO,C16 rMenu[i].label);
		CRLF;
	}

	return current;
}
