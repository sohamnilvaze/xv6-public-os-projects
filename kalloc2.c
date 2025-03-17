#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h" 
#include "mmu.h"
#include "spinlock.h"

#define MAX_PAGES 524288

void freerange(void* vstart,void* vend);
extern char end[];

struct{
    struct spinlock lock;
    int use_lock;
    char* free_pages[MAX_PAGES];
    int top;
}kmem;

void kinit1(void* vstart,void* vend)
{
    initlock(&kmem.lock,"kmem");
    kmem.use_lock = 0;
    kmem.top = 0;
    freerange(vstart,vend);
}

void kinit2(void* vstart,void*vend)
{
    freerange(vstart,vend);
    kmem.use_lock = 1;
}

void freerange(void* vstart,void* vend)
{
    char* p;
    p = (char*)PGROUNDUP((uint)vstart);
    for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    {
        kfree(p);
    }
}

void kfree(char* v)
{
    if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

    // Fill with junk to catch dangling refs.
    memset(v, 1, PGSIZE);

    if(kmem.use_lock)
    {
        acquire(&kmem.lock);
    }
    
    if(kmem.top < MAX_PAGES)
    {
        kmem.free_pages[kmem.top++] = v;
    }
    else
    {
        panic("kfree: No free space");
    }
    if(kmem.use_lock)
    {
        release(&kmem.lock);
    }

}

char* kalloc(void)
{
    char* r = 0;

    if(kmem.use_lock)
    {
        acquire(&kmem.lock);
    }
    if(kmem.top > 0)
    {
        r = kmem.free_pages[--kmem.top];
    }
    if(kmem.use_lock)
    {
        release(&kmem.lock);
    }

    return r;
}