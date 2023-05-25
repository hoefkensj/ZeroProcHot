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


CHAR16                         CHARSET[128];
void GENERATE_CHARSET();
UINT32                         CHARNULL;
void GENERATE_CHARSET();
void GENERATE_CHARSET2();



// void

// PRINT(CHAR16* string){
//
// 	// CHAR16 cstr[2] =*string;
// 	printf("%s",* string);
// // }
// CHAR16 *cat(char *str1, char *str2) {
//     // Calculate the lengths of the input strings
//     int len1 = 0;
// 		int len2 = 0;
// 		CHAR16* result;
//     while (str1[len1] != '\0') {
//         len1++;
//     }
//     while (str2[len2] != '\0') {
//         len2++;
//     }
//
//     // Allocate memory for the concatenated string
//     result = (CHAR16*) malloc((len1 + len2 + 1) * sizeof(CHAR16));
//     if (result == NULL) {
//         return NULL;
//     }
//
//     // Copy the first string into the result buffer
//     for (int i = 0; i < len1; i++) {
//         result[i] = (CHAR16) str1[i];
//     }
//
//     // Append the second string to the result buffer
//     for (int i = 0; i < len2; i++) {
//         result[len1 + i] = (CHAR16) str2[i];
//     }
//     // Add a null terminator to the end of the concatenated string
//     result[len1 + len2] = '\0';
//
//     // Return the concatenated string
//     return result;
// }
// void
// Charset() {
//   // CHAR16 CHARSET[256];
// 	CHAR16 *pCHRS[128] ;
// 	for (UINT16 i = 0; i < 256 ; i++) {
// 		if ((i&&1) == 0){
// 			CHARSET[i] = L'Z' + 2*i;
// 			pCHRS[i/2]= &CHARSET[i];
// 		}else{
// 			CHARSET[i] = L'\0';
// 		}//fi*/
// 	}//rof
// }
void
qprint() {for (int i =0; i < 128; i++)	printf("|%2i: %3c %7i |\n" ,i,CHARSET[i],CHARSET[i]);}
void
print(CHAR16 *string){
	int len =0;
	while (string[len] != '\0') {
		len=len+ sizeof(string[len]);
	}
	for (UINT16 i = 0 ; i < len ; i ++){
		printf("%c", string[i]);
	}
	printf("\n");
}
int
main() {
	GENERATE_CHARSET();
	// // qprint();
	CHAR16 *string=(CHAR16 *) L"test_string";
	print(string);
	return 1;
}
void
GENERATE_CHARSET(){                            // ONCE PER LOOKUP
	for (UINT16 i = 0 ; i < 128 ; i ++){
		CHARSET[i] =                               // glob CHAR16 CHARSET[62];
			(           i <= 9 )*( 48 + i)           // 10      [0  - 9]
		+ (10 <= i && i <= 35)*( 65 + i-10)        // +26=36  [10 - 35]
		+ (36 <= i && i <= 61)*( 97 + i-36)        // +26=62  [36 - 61]
		+ (62 <= i && i <= 77)*( 32 + i-62)        // +16=78  [62 - 78]
		+ (78 <= i && i <= 83)*( 91 + i-78)        // +6=84   [80 - 85]
		+ (84 <= i && i <= 87)*( 123+ i-84)        // +4=88   [86 - 89]
		+ (88 <= i && i <= 94)*( 58 + i-88);       // +7=95   [90 - 96]
	}//rof
}// END
