/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include "sfmm.h"

/**
 * You should store the head of your free list in this variable.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
sf_free_header* freelist_head = NULL;

#define PAGE_SIZE 4096

/* TODO Make sure to properly update the freelist when stuff is added. 
		Right now I am not properly using next and prev. */
void* sf_malloc(size_t size) {
	if(size == 0){
		return NULL;
	} else {
		if(freelist_head == NULL){
			/* Get the current pointer to the heap and assign it to freelist_head*/
			void* pointer = sf_sbrk(PAGE_SIZE);
			pointer += -8;
			freelist_head = (sf_free_header*) pointer;
			pointer += 8;
			(*freelist_head).header.alloc = 0;
			(*freelist_head).header.block_size = PAGE_SIZE;
			(*freelist_head).header.block_size = (*freelist_head).header.block_size >> 4;
			int payloadSize = (*freelist_head).header.block_size << 4;
			void* footerPointer = pointer;
			footerPointer += payloadSize -16;
			struct sf_footer * footer = (sf_footer*) footerPointer;
			(*footer).alloc = 0;
			(*footer).block_size = (*freelist_head).header.block_size;

			//printf("%p\n", freelist_head);

			/* Freelist now instantiated? */
		}

		void* pointer = (void*) freelist_head;
		pointer += 8;
		void* footerPointer = pointer;
		footerPointer += ((*freelist_head).header.block_size << 4) - 16;
		pointer += -8;

		//printf("%p\n", pointer);
		
		/* Get the correct payload size. It must be divisible by 16 
		to align correctly */
		int payloadSize = size;
		if(size < 16){
			payloadSize = 16;
		} else {
			int i = 0;
			for(; i < 16; i++){
				if(payloadSize % 16 == 0){
					break;
				} else {
					payloadSize += 1;
				}
			}
		}

		//printf("%d\n", (*freelist_head).header.block_size << 4);

		/* First fit */
		bool fit = true;
		sf_free_header* fitHeader = freelist_head;

		while(fit) {
			/* The requested memory will fit in the block */
			if(payloadSize <= ((*fitHeader).header.block_size << 4)){
				sf_header * header = (sf_header*) pointer;
				int sizeHolder = (*header).block_size << 4;
				(*header).alloc = 1;
				(*header).block_size = (payloadSize + 16) >> 4;
				(*header).requested_size = size;

				//printf("%d\n", sizeHolder);

				pointer += 8;

				void* blockFooter = pointer;
				blockFooter += ((*header).block_size << 4) - 16;
				struct sf_footer * bFooter = (sf_footer*) blockFooter;
				(*bFooter).alloc = 1;
				(*bFooter).block_size = (*header).block_size;

				//printf("%p\n", bFooter);


				/* Update the freelist*/
				void* realignHeader = (void*) blockFooter;
				realignHeader += 8;
				fitHeader = (sf_free_header*) realignHeader;
				(*fitHeader).header.alloc = 0;
				(*fitHeader).header.block_size = (sizeHolder >> 4) - ((*header).block_size);

				//printf("%d\n", (*freelist_head).header.block_size << 4);

				void* footerRealign = (void*) fitHeader;
				footerRealign += 8;
				footerRealign += ((*fitHeader).header.block_size << 4) - 16;
				sf_footer* freeFooter = (sf_footer*) footerRealign;
				(*freeFooter).block_size = (*fitHeader).header.block_size;

				sf_free_header* tempHeader = freelist_head;
				while((*tempHeader).next != NULL){
					tempHeader = (*tempHeader).next;
				}
				/* tempHeader.next now equals NULL */
				(*tempHeader).next = fitHeader;

				bool adjustHeader = true;
				while(adjustHeader){
					if((*freelist_head).header.alloc == 1){
						freelist_head = (*freelist_head).next;
					} else {
						adjustHeader = false;
					}
				}

				/* TODO Test thing, delete before submission */
				adjustHeader = true;
				sf_free_header* testHeader = freelist_head;
				while(adjustHeader){
					printf("%p\n", testHeader);
					if((*testHeader).next != NULL) {
						testHeader = (*testHeader).next;
					} else {
						adjustHeader = false;
					}
				}


				//printf("%p\n", freeFooter);

				fit = false;
			} else {
				/* Current block couldn't store it. Check next block */
				if((*fitHeader).next != NULL){
					fitHeader = (*fitHeader).next;
				} else {
					/* Request more memory to store value */
					int blocks = size / PAGE_SIZE;
					/* Need one more block to fulfill */
					if(!((blocks * PAGE_SIZE) > size)){
						blocks += 1;
					}

					//printf("%d\n%d\n", blocks, blocks * PAGE_SIZE);

					fitHeader = (sf_free_header*) sf_sbrk(blocks * PAGE_SIZE);
					(*fitHeader).header.block_size = blocks * PAGE_SIZE >> 4;
					(*fitHeader).header.requested_size = size;
					(*fitHeader).header.alloc = 1;

					void* fitFooterTemp = (void*) fitHeader;
					fitFooterTemp += 8;
					fitFooterTemp += ((*fitHeader).header.block_size << 4) - 16;
					sf_footer* fitFooter = (sf_footer*) fitFooterTemp;
					(*fitFooter).block_size = (*fitHeader).header.block_size;
					(*fitFooter).alloc = 1;
				}
			}
		}
		return pointer;
	}
	return NULL;
}

/* TODO The argument given is the pointer given by sf_alloc. 
		Don't need to worry about behavior with incorrect pointer given.
		That is the l_user's fault. */
void sf_free(void *ptr) {

}

/* TODO */
void* sf_realloc(void *ptr, size_t size) {
    return NULL;
}

void* sf_calloc(size_t nmemb, size_t size) {
	void* mem = sf_malloc(size * nmemb);
	int i = 0;
	/* char pointers have size 1. This means we will only overwrite what we want */
	char* index = mem;
	for(; i < nmemb * size; i++){
		*index = 0;
		index += 1;
	}
    return mem;
}
