#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "denbu_mem.h"

int main(int argc, const char *argv[]){
	printf("Testing out malloc...\n");

	int size = 4096;
    assert( Denbu_Mem_Init(size) == 0);

	int* ptr = (int*) Denbu_Mem_Alloc(sizeof(int), 1);
	printf("\nptr : %p", ptr);
	// ptr = &size;
	// printf("\nValue at ptr : %d\n", *ptr);

	int* ptr1 = (int*) Denbu_Mem_Alloc(sizeof(int), 1);
	printf("\nptr1 : %p", ptr1);
	
	

	exit(1); 
}
