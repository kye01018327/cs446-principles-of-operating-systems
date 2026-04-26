#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>

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

mlist_t mlist = { NULL };
#define MBLOCK_HEADER_SZ offsetof(mblock_t, payload)

#define ALIGN(size) (((size) + 7) & ~7)


mblock_t * findFreeBlockOfSize(size_t size) {
    mblock_t * curr = mlist.head;

    while (curr) {
        if (curr->status == 0 && curr->size >= size) {
            return curr;
        }

        curr = curr->next;
    }
    return NULL;
}

void splitBlockAtSize(mblock_t * block, size_t size) {
    if (block->size >= size + sizeof(mblock_t) + 8) {
        mblock_t * remaining = (mblock_t *)((char *)block + MBLOCK_HEADER_SZ + size);
        
        remaining->size = block->size - size - MBLOCK_HEADER_SZ;
        remaining->status = 0;
        remaining->next = block->next;
        remaining->prev = block;

        if (block->next) {
            block->next->prev = remaining;
        }

        block->next = remaining;
        block->size = size;
    }
}

mblock_t * growHeapBySize(size_t size) {
    size_t request_size = (size + MBLOCK_HEADER_SZ > 1024) ? (size + MBLOCK_HEADER_SZ) : 1024;
    
    void * prev_break = sbrk(request_size);

    if (prev_break == (void *)-1) {
        return NULL;
    }

    mblock_t * new_block = (mblock_t *)prev_break;
    new_block->size = request_size - MBLOCK_HEADER_SZ;
    new_block->status = 0;
    new_block->next = NULL;
    new_block->prev = NULL;

    if (!mlist.head) {
        mlist.head = new_block;
    }
    else {
        mblock_t * last = mlist.head;
        while (last->next) {
            last = last->next;
        }
        
        last->next = new_block;
        new_block->prev = last;
    }
    return new_block;
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

    size = ALIGN(size);

    mblock_t * block = findFreeBlockOfSize(size);

    if (!block) {
        block = growHeapBySize(size);

        if (!block) {
            return NULL;
        }
    }

    splitBlockAtSize(block, size);
    block->status = 1;
    return (void *)((char *)block + MBLOCK_HEADER_SZ);
}

void myfree(void * ptr) {
    if (!ptr) {
        return;
    }

    mblock_t * block = (mblock_t *)((char *)ptr - MBLOCK_HEADER_SZ);

    if ((void *)block < (void *)mlist.head || (void *)block > sbrk(0)) {
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
        mblock_t * prev_b = block->prev;
        prev_b->size += MBLOCK_HEADER_SZ + block->size;
        prev_b->next = block->next;

        if (block->next) {
            block->next->prev = prev_b;
        }
    }
}

int main(int argc, char *argv[]){
    printMemList(mlist.head);
    void * p1 = mymalloc(10);
    printMemList(mlist.head);
    void * p2 = mymalloc(100);
    printMemList(mlist.head);
    void * p3 = mymalloc(200);
    printMemList(mlist.head);
    void * p4 = mymalloc(500);
    printMemList(mlist.head);
    myfree(p3); p3 = NULL;
    printMemList(mlist.head);
    myfree(p2); p2 = NULL;
    printMemList(mlist.head);
    void * p5 = mymalloc(150);
    printMemList(mlist.head); 
    void * p6 = mymalloc(500);
    printMemList(mlist.head);
    myfree(p4); p4 = NULL;
    printMemList(mlist.head);
    myfree(p5); p5 = NULL;
    printMemList(mlist.head);
    myfree(p6); p6 = NULL;
    printMemList(mlist.head);
    myfree(p1); p1 = NULL;
    printMemList(mlist.head);
    return 0;
}