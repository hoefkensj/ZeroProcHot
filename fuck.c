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


CHAR16                         CHARSET;
UINT32                         CHARNULL;
char		                       chr;
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
UINT16
main() {
	char lib[11]="0123456789";//ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz ~!@#$%^&*()_+{}|:<>?`-=[]\\;,./";
	CHAR16 libc16[11]="0123456789";;
	for (UINT32 i = 0; i < 10; i++){
		printf("%c\n" ,(char)lib[i]);
		printf("%c\n" ,(char)lib[2]);
// char *a;//=//&CHARSET[10];
// char *b;//=&CHARSET[20];
// CHAR16 *c;
// Charset();

	// for (UINT16 i = 0; i < 128; i++){
		// a=(char *)(&CHARSET[i]);
		// printf("%s",a );
		// PRINT(&CHARSET[i]);
		// SLEEP(100000);
	}
	return 1;
}
