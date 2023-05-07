
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>



void print_bit(uint64_t val, int bit)
{
    if (val & ((uint64_t)1 << bit))
        printf("%i", 1);
    else
        printf("%i", 0);

}
void print_bits(uint64_t val)
{
    for (int i = 63; i >= 0; i--)
        print_bit( val, i);
    printf("\n");
}

int main(){
	float x= -0.0;
	uint32_t xb= 0b10000000000000000000000000000000;
	int32_t xub= 0b10000000000000000000000000000000;
	float xbf=xb;
	float y= +0.0;
	printf("x: %f\n", x);
	printf("xb: %i\n", xb);
	printf("xub: %i\n", xub);
	printf("xbf: %f\n", xbf);
	uint64_t xx = x;
	print_bits(xx);
	print_bits(y);
	// return;
}