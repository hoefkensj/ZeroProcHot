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
#define CHAR_SET_SIZE 255

struct MenuItem {
	CHAR16 *select;
	int inum;
	CHAR16 snum;
	CHAR16 *label;
};
struct MenuItem rMenu[10];

EFI_SYSTEM_TABLE              *EST;
EFI_BOOT_SERVICES             *tbS;
SIMPLE_TEXT_OUTPUT_INTERFACE  *tcO;
SIMPLE_INPUT_INTERFACE        *tcI;
EFI_TEXT_OUTPUT_STRING         PRINT;
EFI_TEXT_RESET                 CLEAR;
EFI_INPUT_READ_KEY             READK;
EFI_STALL                      SLEEP;
CHAR16                         CHARSET[256];
CHAR16                        *pCHRS[128];
CHAR16                         LINE[78];



// CHAR16 *cat(char* str1, char* str2);
uint8_t menu(uint8_t current);
UINT64 StrToHex(CHAR16 *Input, UINTN Pos, UINTN NumChars);
// Returns TRUE if replacement was done, FALSE otherwise.
// BOOLEAN ReplaceSubstring(IN OUT CHAR16 **MainString, IN CHAR16 *SearchString, IN CHAR16 *ReplString);

VOID uCase(CHAR16 * MyString) ;

// Converts consecutive characters in the input string into a
// number, interpreting the string as a hexadecimal number, starting
// at the specified position and continuing for the specified number
// of characters or until the end of the string, whichever is first.
// NumChars must be between 1 and 16. Ignores invalid characters.
UINT64 StrToHex(CHAR16 *Input, UINTN Pos, UINTN NumChars) {
    UINT64 retval = 0x00;
    UINTN  NumDone = 0, InputLength;
    CHAR16 a;

    if ((Input == NULL) || (NumChars == 0) || (NumChars > 16)) {
        return 0;
    }

    InputLength = StrLen(Input);
    while ((Pos <= InputLength) && (NumDone < NumChars)) {
        a = Input[Pos];
        if ((a >= '0') && (a <= '9')) {
            retval *= 0x10;
            retval += (a - '0');
            NumDone++;
        }
        if ((a >= 'a') && (a <= 'f')) {
            retval *= 0x10;
            retval += (a - 'a' + 0x0a);
            NumDone++;
        }
        if ((a >= 'A') && (a <= 'F')) {
            retval *= 0x10;
            retval += (a - 'A' + 0x0a);
            NumDone++;
        }
        Pos++;
    } // while()
    return retval;
} // StrToHex()
VOID uCase(CHAR16 * MyString) {
	UINTN i = 0;
	if (MyString) {
		while (MyString[i] != L'\0') {
			if ((MyString[i] >= L'a') && (MyString[i] <= L'z'))
				MyString[i] = MyString[i] - L'A' + L'a';
			i++;
		} // while
	} // if
}

// typedef unsigned short CHAR16;

CHAR16 *cat(CHAR16 *str1, char *str2) {
    // Calculate the lengths of the input strings
    int len1 = 0;
		int len2 = 0;
		CHAR16* result;
    while (str1[len1] != '\0') {
        len1++;
    }
    while (str2[len2] != '\0') {
        len2++;
    }

    // Allocate memory for the concatenated string
    result = (CHAR16*) malloc((len1 + len2 + 1) * sizeof(CHAR16));
    if (result == NULL) {
        return NULL;
    }

    // Copy the first string into the result buffer
    for (int i = 0; i < len1; i++) {
        result[i] = (CHAR16) str1[i];
    }

    // Append the second string to the result buffer
    for (int i = 0; i < len2; i++) {
        result[len1 + i] = (CHAR16) str2[i];
    }
    // Add a null terminator to the end of the concatenated string
    result[len1 + len2] = '\0';

    // Return the concatenated string
    return result;
}


// Convert input string to all-uppercase.
// DO NOT USE the standard StrLwr() function, since it's broken on some EFIs!





// Function to initialize the character set and return an array of pointers to its elements
void Charset() {
  // CHAR16 CHARSET[256];
	// CHAR16 *pCHRS[128];
	for (UINT16 i = 0; i < 256 ; i++) {
		if ((i&&1) == 0){
			CHARSET[i] = L'0' + i;
			pCHRS[i/2]= &CHARSET[i];
		}else{
			CHARSET[i] = L'\0';
		}//fi
	}//rof
}



// void charset(){
// 	char end=L'\0';
// 	CHAR16 string;
// 	CHAR16 czero=L'0';
//
// 	for (UINT16 i = 0; i < 10; i++){
// 		string=czero+i;
// 		CHARSET[i]= *cat(&string,&end);
// 	}
// 	czero=L'A';
// 	for (UINT16 i = 10; i < 36; i++){
// 		string=czero+(i-10);
// 		CHARSET[i]= *cat(&string,&end);
// 	}
// 	czero=L'a';
// 	for (UINT16 i = 36; i < 62; i++){
// 		string=czero+(i-36);
// 		CHARSET[i]= *cat(&string,&end);
// 	}
// 	czero=L'.';
// 	for (UINT16 i = 62; i <= 255; i++){
// 		string=L'.';
// 		CHARSET[i]= *cat(&string,&end);
// 	}
// }
EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systemTable) {
	// int num;
	// CHAR16 linebuffer[80];

	EST   = systemTable;
	tbS   = EST->BootServices;
	tcO   = EST->ConOut;
	tcI   = EST->ConIn;
	PRINT = tcO->OutputString;
	CLEAR = tcO->Reset;
	READK = tcI->ReadKeyStroke;
	SLEEP = tbS->Stall;
	CHAR16 *pCHRS;
	Charset();

	for (UINT16 i = 0; i < 128; i++){
		tcO->OutputString(tcO,pCHRS[i]);
		// SLEEP(100000);
	}
	return EFI_SUCCESS;
}

uint8_t  menu(uint8_t current) {
	// menuitems:
	CHAR16 *item=L"Exit";
	for (int i = 0; i <= 10; i++){
		if (i == current ){
			rMenu[i].select=L" >";
		} else {
			rMenu[i].select=L"  ";
		}
		rMenu[i].inum=i;
		rMenu[i].snum= CHARSET[i];
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
