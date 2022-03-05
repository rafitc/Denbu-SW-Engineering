#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "denbu_mem.h"

int m_error;
// number of calls made to mem_init
int callsToInit = 0;
// size of page in virtual memory
int pageSize;

//Payload header to notify free or allocated
// 0 -> Not allocated
// 1 -> Allocated
typedef struct header{
    struct header* next;
    
    // 0 -> Not allocated
    // 1 -> Allocated
    int alloc_flag;
    int size;
    
}payload_header;

payload_header* head_block = NULL;

int checkFree(payload_header* test_block);
void* check_best_fit(int mem_size);
//function to check whether the space is free or not 

int Denbu_Mem_Init(int sizeOfRegion)
{
    void *memmptr;

    // example failure cases
    if (sizeOfRegion <= 0 || callsToInit > 0) {
        m_error = E_BAD_ARGS;
        return -1;
    }
    
    // rounding up the size to page size
    pageSize = getpagesize();
    printf("Page Size: %d\n", pageSize);
    // make sure region is evenly divisible by page size
    if ( (sizeOfRegion % pageSize) != 0) {
        sizeOfRegion += (pageSize - (sizeOfRegion % pageSize));
    }
    printf("Size of Region: %d\n", sizeOfRegion);
    
    
    // call to mmep to get memory from OS 
    int fd = open("/dev/zero", O_RDWR);
    memmptr = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (memmptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    //save the mem space 
    head_block = (payload_header*)memmptr;
    printf("Head block space %p\n", head_block);
    head_block->next = NULL;
    head_block->alloc_flag = 1;
    head_block->size = sizeOfRegion - (int)sizeof(payload_header);
    printf("Allocation status of head block %d\n", head_block->size);
    
    // this will not free the memory we got from OS.
    close(fd);
    
    callsToInit++;

    // TODO: you need to assume the library will manage the memory
    // allocated by mmap here (memptr)
    return 0;
}

// 
// TODO: Library should fill the rest of the functions
// Denbu_Mem_Alloc, Denbu_Mem_Free, Denbu_Mem_Dump
//
void* Denbu_Mem_Alloc(int mem_size_test, int style){

    while(!(mem_size_test % 8 == 0))
	{
		++mem_size_test;
	}
    printf("mem size after updation %d\n", mem_size_test);


    switch(style){
        case 1:
            printf("BESTFIT \n");
            return(check_best_fit(mem_size_test));
            break;
        case 2:
            printf("FIRSTFIT \n");
            break;
        case 3:
            printf("WORSTFIT \n");
            break;
        default:
            printf("Default case \n");
            break;

    }
    return 0;
}

void* check_best_fit(int mem_size){
    printf("Best fit fn call\n");
    int best_fit_block_size = 0; // neet to change change var to hold size (including header) of bf_block
    payload_header* current_block = head_block; 
    payload_header* best_fit_block = NULL;

    while(current_block != NULL){

        //chekc is the location free or not
        if(checkFree(current_block)){
            printf("IF check free fn\n");
            if(best_fit_block_size == 0){ //Look for previous allocation
                if(current_block->size >= mem_size){
                    best_fit_block_size = current_block->size + 8;
                    printf("\nhere is the freeBlockSize : %d", best_fit_block_size);
                    best_fit_block = current_block;
                }
            }
            if((current_block->size + 8) < best_fit_block_size && current_block->size >= mem_size){

                best_fit_block_size = current_block->size + 8;
                best_fit_block = current_block;
            }
        }
        printf("\n Next block");
        current_block = current_block->next;
    }

    if(best_fit_block == NULL)
	{
	    m_error = E_NO_SPACE;
        return NULL;
	}

    best_fit_block->alloc_flag = 1;
    head_block = best_fit_block+8;
    head_block->alloc_flag = 1;
    return ((void*) best_fit_block + 8);
}

int checkFree(payload_header* test_block){
    printf("Alloc flag %d",test_block->alloc_flag);
    if(test_block == NULL){
        return 0;
    }
    else if(test_block->alloc_flag == 1){
        return 1;
    }
    return 0;  
}

int Denbu_Mem_Free(void *ptr){
    printf("Denbu Mem Free Fn \n");
    return 0;
}

void Denbu_Mem_Dump(){
    printf("Denbu Mem Dump \n");
}