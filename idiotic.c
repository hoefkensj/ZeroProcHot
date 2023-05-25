#include <stdio.h>

int
main(){

	int mychar=-1;
	int mycharaddr=&mychar;
	int * mycharpointer=&mychar;
	printf("%i\n" , mychar);
	printf("%i\n" , mycharaddr);
	printf("%i\n" , mycharpointer);

}
