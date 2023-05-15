
//efi
#include <efi.h>
#include <efilib.h>
// #include <efibind.h>
#include <efidef.h>
// #include <efidevp.h>
// #include <eficon.h>
// #include <efiprot.h>
// #include <efiapi.h>
// #include <efierr.h>
//std
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>


int main(){
	CHAR16 *test;
	char start[]="string %s\ndigit %d\n";
	char fmt[1024];
	int num;
	printf(start,"a", 7);
	test=(CHAR16 *)start;

	num=sprintf(fmt,test,"a",19);
	printf(fmt);
	printf("%d",num);

}
