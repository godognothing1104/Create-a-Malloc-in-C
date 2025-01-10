#include <string.h>
#include <stdio.h>
#include <unistd.h>

/* The standard allocator interface from stdlib.h.  These are the
 * functions you must implement, more information on each function is
 * found below. They are declared here in case you want to use one
 * function in the implementation of another. */
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

/* When requesting memory from the OS using sbrk(), request it in
 * increments of CHUNK_SIZE. */
#define CHUNK_SIZE (1<<12)
#define LIST_SIZE 13
/*
 * This function, defined in bulk.c, allocates a contiguous memory
 * region of at least size bytes.  It MAY NOT BE USED as the allocator
 * for pool-allocated regions.  Memory allocated using bulk_alloc()
 * must be freed by bulk_free().
 *
 * This function will return NULL on failure.
 */
extern void *bulk_alloc(size_t size);

/*
 * This function is also defined in bulk.c, and it frees an allocation
 * created with bulk_alloc().  Note that the pointer passed to this
 * function MUST have been returned by bulk_alloc(), and the size MUST
 * be the same as the size passed to bulk_alloc() when that memory was
 * allocated.  Any other usage is likely to fail, and may crash your
 * program.
 *
 * Passing incorrect arguments to this function will result in an
 * error message notifying you of this mistake.
 */
extern void bulk_free(void *ptr, size_t size);

/*
 * This function computes the log base 2 of the allocation block size
 * for a given allocation.  To find the allocation block size from the
 * result of this function, use 1 << block_index(x).
 *
 * This function ALREADY ACCOUNTS FOR both padding and the size of the
 * header.
 *
 * Note that its results are NOT meaningful for any
 * size > 4088!
 *
 * You do NOT need to understand how this function works.  If you are
 * curious, see the gcc info page and search for __builtin_clz; it
 * basically counts the number of leading binary zeroes in the value
 * passed as its argument.
 */
static inline __attribute__((unused)) int block_index(size_t x) {
    if (x <= 8) {
        return 5;
    } else {
        return 32 - __builtin_clz((unsigned int)x + 7);
    }
}

/*
 * You must implement malloc().  Your implementation of malloc() must be
 * the multi-pool allocator described in the project handout.
 */

typedef struct {
		size_t metadata;	
		struct block *next;
		int flag;	
	}block ;


typedef struct 
{
	block *block;
}freeListTable;

static freeListTable *head = NULL;


freeListTable *initialize_list(){
	head = (freeListTable *)sbrk(LIST_SIZE * CHUNK_SIZE);


	for(int i = 0; i< LIST_SIZE; i++){
		head[i].block = NULL;
	}
	return head;
		
}

void *allocateblock_null(block *none, size_t size){
	none = (block *)sbrk(CHUNK_SIZE);
	none->metadata = size;
	none->flag = 1;
	none->next = NULL;
	return (void *) (none + 1 );
}
void *allocateblock_free(block *found, size_t size){
	found->flag = 1;
	found->metadata = size;
	return (void *) ( found + 1);
}
block *insert(block *header){
	int iteration =0;
	while(header != NULL){
		if(header->flag ==0){
		return header;
		}
		if(header == (block*) header->next){
		header->next = NULL;
		return (block*)header->next;
		}
		if(iteration >5000){
			printf("crash");
			return (block*)header->next;
		}	
		iteration ++;
		header = (block *)header->next;
	}
	return header;
}
void *malloc(size_t size) {
if(size <= 0){
	return NULL;}
if((size + 8) > CHUNK_SIZE){
	block * large_block = (block *) bulk_alloc(size+8);
	if(large_block == NULL){
		printf("failed bulk_alloc\n");
		return NULL;
	}
	large_block -> metadata = size;
	
	return (void*)(large_block +1);;
}

size_t true_index = block_index(size);


if(true_index < 0 || true_index > 12){
//printf("%s", "cant");
	return NULL;
}


if(head == NULL){
	head = initialize_list();
}

block *header = head[true_index].block; 
block *current = insert(header);
if(current == NULL){
	return allocateblock_null(current,size);
}else{
	return allocateblock_free(current,size);

}

	return NULL;
}	


/*
 * You must also implement calloc().  It should create allocations
 * compatible with those created by malloc().  In particular, any
 * allocations of a total size <= 4088 bytes must be pool allocated,
 * while larger allocations must use the bulk allocator.
 *
 * calloc() (see man 3 calloc) returns a cleared allocation large enough
 * to hold nmemb elements of size size.  It is cleared by setting every
 * byte of the allocation to 0.  You should use the function memset()
 * for this (see man 3 memset).
 */

void *calloc(size_t nmemb, size_t size) {
    if(nmemb == 0 || size == 0){
    return NULL;
    }
	if(size > 4088){
   	void *ptr = bulk_alloc(nmemb * size);
    	memset(ptr, 0, nmemb * size);

    	return ptr;
    	}
	void *ptr = malloc(nmemb * size);
	memset(ptr,0,nmemb *size);
	return ptr;
}

/*
 * You must also implement realloc().  It should create allocations
 * compatible with those created by malloc(), honoring the pool
 * alocation and bulk allocation rules.  It must move data from the
 * previously-allocated block to the newly-allocated block if it cannot
 * resize the given block directly.  See man 3 realloc for more
 * information on what this means.
 *
 * It is not possible to implement realloc() using bulk_alloc() without
 * additional metadata, so the given code is NOT a working
 * implementation!
 */
void *realloc(void *ptr, size_t size) {
   if (size == 0) { 
        free(ptr);
        return NULL;
    }else{
    if (ptr == NULL) {
        return malloc(size);
    }
    }
    
    void* new_ptr = malloc(size);


    size_t current_size = ((block *)ptr-1)->metadata; 
    if(current_size < size){
    size = current_size;
    }
    if(size<current_size){
    size = current_size;
    return ptr;
    }
    memcpy(new_ptr, ptr, size);

 
    free(ptr);

    return new_ptr;
}/**
 * You should implement a free() that can successfully free a region of
 * memory allocated by any of the above allocation routines, whether it
 * is a pool- or bulk-allocated region.
 *
 * The given implementation does nothing.
 **/

void free(void *ptr) {
	
	if(ptr == NULL){return;}
		block *current = ((block *)ptr) - 1;
		if(current->metadata + 8 > CHUNK_SIZE){
		bulk_free(current,current->metadata + 8);
		return;
		}
		current ->flag = 0;
		size_t index = block_index(current->metadata);
		current->next = (struct block*)head[index].block;
		head[index].block = current;

	
	
	return;
}
