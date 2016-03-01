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

/* TODO Make sure that there is enough room for the requested space.
		Make sure to reassign freelist at the end. */
void* sf_malloc(size_t size) {
	if(size == 0){
		return NULL;
	} else {
		if(freelist_head == NULL){
			/* Assign the information to the header */
			struct sf_header header;
			header.requested_size = size;
			header.block_size = size + sizeof(header);
			header.alloc = 1;
			/* Assign the information to the footer */
			struct sf_footer footer;
			footer.alloc = 1;

			header.block_size = header.block_size + sizeof(footer);
			footer.block_size = header.block_size;

			/* Get the current pointer to the heap */
			void* pointer = sf_sbrk(0);

			void* movingPointer;
			struct sf_header* headerPointer = pointer;
			headerPointer += 1;
			*headerPointer = header;
			pointer = headerPointer + 1;
			movingPointer = pointer + header.block_size*16 - 16;
			struct sf_footer* footerPointer = movingPointer;
			*footerPointer = footer;
			printf("%p\n%p\n%p\n", headerPointer, pointer, footerPointer);
			
			return pointer;
		}
	}
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
