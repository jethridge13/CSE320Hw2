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

/* TODO Make sure that there is enough room for the requested space.*/
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

		/* The requested memory will fit in the block */
		if(payloadSize <= ((*freelist_head).header.block_size << 4)){
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

			/* Move the freelist_head */
			void* realignHeader = (void*) blockFooter;
			realignHeader += 8;
			freelist_head = (sf_free_header*) realignHeader;
			(*freelist_head).header.alloc = 0;
			(*freelist_head).header.block_size = (sizeHolder >> 4) - ((*header).block_size);

			//printf("%d\n", (*freelist_head).header.block_size << 4);

			void* footerRealign = (void*) freelist_head;
			footerRealign += 8;
			void* returnable = footerRealign;
			footerRealign += ((*freelist_head).header.block_size << 4) - 16;
			sf_footer* freeFooter = (sf_footer*) footerRealign;
			(*freeFooter).block_size = (*freelist_head).header.block_size;

			//printf("%p\n", freeFooter);

			return returnable;
		}
		return pointer;
	}
	return NULL;
}

/* TODO */
void sf_free(void *ptr) {

}

/* TODO */
void* sf_realloc(void *ptr, size_t size) {
    return NULL;
}

/* TODO */
void* sf_calloc(size_t nmemb, size_t size) {
    return sf_malloc(size);
}
