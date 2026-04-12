#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

typedef struct _mblock_t {
    struct _mblock_t * prev;
    struct _mblock_t * next;
    size_t size;
    int status;
    void * payload;
} mblock_t;

typedef struct _mlist_t {
    mblock_t * head;
} mlist_t;

#define MIN_ALLOC_SZ 1
#define MBLOCK_HEADER_SZ offsetof(mblock_t, payload)
mlist_t mlist = { NULL };


void printMemList(const mblock_t* headptr);
void * mymalloc(size_t size);
void myfree(void *ptr);

int main(int argc, char *argv[]){
    printMemList(mlist.head); // 1
    void * p1 = mymalloc(10);
    printMemList(mlist.head); // 2
    void * p2 = mymalloc(100);
    printMemList(mlist.head); // 3
    void * p3 = mymalloc(200);
    printMemList(mlist.head); // 4
    void * p4 = mymalloc(500);
    printMemList(mlist.head); // 5
    myfree(p3); p3 = NULL;
    printMemList(mlist.head); // 6
    myfree(p2); p2 = NULL;
    printMemList(mlist.head); // 7
    void * p5 = mymalloc(150);
    printMemList(mlist.head); // 8
    void * p6 = mymalloc(500);
    printMemList(mlist.head); // 9
    myfree(p4); p4 = NULL;
    printMemList(mlist.head); // 10
    myfree(p5); p5 = NULL;
    printMemList(mlist.head); // 11
    myfree(p6); p6 = NULL;
    printMemList(mlist.head); // 12
    myfree(p1); p1 = NULL;
    printMemList(mlist.head); // 13
    return 0;
}
void printMemList(const mblock_t* head) {
    const mblock_t* p = head;
    size_t i = 0;
    while(p != NULL) {
        printf("[%ld] p: %p\n", i, (void*)p);
        printf("[%ld] p->size: %ld\n", i, p->size);
        printf("[%ld] p->status: %s\n", i, p->status > 0 ? "allocated" : "free");
        printf("[%ld] p->prev: %p\n", i, (void*)p->prev);
        printf("[%ld] p->next: %p\n", i, (void*)p->next);
        printf("___________________________\n");
        ++i;
        p = p->next;
    }
    printf("======================================================\n");
}

void * mymalloc(size_t size) {
    if (size <= 0) {
        return NULL;
    }

    mblock_t *current = mlist.head;
    mblock_t *last = NULL;

    while (current != NULL) {
        if (current->status == 0 && current->size >= size) {
            if (current->size >= size + MBLOCK_HEADER_SZ + MIN_ALLOC_SZ) {
                mblock_t *remaining = (mblock_t *)((char *)current + MBLOCK_HEADER_SZ + size);
                remaining->size = current->size - size - MBLOCK_HEADER_SZ;
                remaining->status = 0;
                remaining->next = current->next;
                remaining->prev = current;

                if (current->next) {
                    current->next->prev = remaining;
                }
                current->next = remaining;
                current->size = size;
            }

            current->status = 1;
            return &(current->payload);
        }
        last = current;
        current = current->next;
    }

    size_t request_size;
    if (size + MBLOCK_HEADER_SZ > 1024) {
        request_size = size + MBLOCK_HEADER_SZ;
    }
    else {
        request_size = 1024;
    }
    
    mblock_t *new_block = (mblock_t *)sbrk(request_size);

    if (new_block == (void *)-1) {
        return NULL;
    }

    new_block->size = request_size - MBLOCK_HEADER_SZ;
    new_block->status = 1;
    new_block->next = NULL;
    new_block->prev = last;

    if (last) {
        last->next = new_block;
    }
    else {
        mlist.head = new_block;
    }

    return &(new_block->payload);
}

void myfree(void * ptr){
    if (!ptr) {
        return;
    }
    
    mblock_t *block = (mblock_t *)((char *)ptr - MBLOCK_HEADER_SZ);

    if (block < mlist.head || (void *)block > sbrk(0)) {
        return;
    }

    block->status = 0;

    if (block->next && block->next->status == 0) {
        block->size += MBLOCK_HEADER_SZ + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }

    if (block->prev && block->prev->status == 0) {
        mblock_t *prev_block = block->prev;
        prev_block->size += MBLOCK_HEADER_SZ + block->size;
        prev_block->next = block->next;
        if (block->next) {
            block->next->prev = prev_block;
        }
    }
}