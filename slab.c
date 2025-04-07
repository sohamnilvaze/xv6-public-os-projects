// #include "mmu.h"
#include "file.h"
#include "types.h"
#include "fs.h"
// #include "spinlock.h"
#include "spinlock.c"
#include "slab.h"

typedef unsigned int uint;

void slab_free(struct slab_cache *cache, struct slab *s)
{
    if (!s)
        return;

    acquire(&(cache->lock));

    struct slab *prev = 0;
    struct slab *curr = cache->slab_list;

    while (curr && curr != s) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == s) {
        if (prev)
            prev->next = s->next;
        else
            cache->slab_list = s->next;
    }

    release(&(cache->lock));

    // Actual memory free
    if (s->ref)
        kfree((char *)s->ref);
    if (s->start_address)
        kfree((char *)s->start_address);
}



void slab_cache_init(struct slab_cache * cache,uint file_size)
{
    initlock(&cache->lock,"slab_cache");
    cache->file_size = sizeof(struct file);
    cache->slab_size = PGSIZE;
    cache->slab_list = 0;
    return;
}

struct slab * slab_init(struct slab_cache * cache)
{
    struct slab * s = (struct slab *)kalloc();
    if(!s)
    {
        panic("allocate slab memory");
    }
    s->start_address = kalloc();
    if (!s->start_address)
    {
        return 0;
    }

    s->total_files = no_of_files_in_slab;
    s->free_count = no_of_files_in_slab;

    s->ref = (int*)kalloc();
    if(!s->ref)
    {
        s->start_address = 0;
        slab_free(cache,s);
        return 0;
    }

    for(int i=0;i<s->total_files;i++)
    {
        s->ref[i] = 1;
    }

    s->next = 0;
    return s;

}

struct slab * create_new_slab(struct slab_cache * cache)
{
    struct slab * s = slab_init(cache);

    struct slab * prev;
    struct slab * curr = cache->slab_list;
    while(curr && curr->next)
    {
        prev = curr;
        curr = curr->next;
    }

    curr->next = s;

    return s;
}

char * file_alloc(struct slab_cache * cache)
{
    acquire(&(cache->lock));

    struct slab * curr = cache->slab_list;

    while(curr && curr->free_count == 0)
    {
        curr = curr->next;
    }

    if(!curr)
    {
        curr = slab_init(cache);
        if(!curr)
        {
            release(&(cache->lock));
            return 0;
        }

        curr->next = cache->slab_list;
        cache->slab_list = curr;
    }

    for(int i = 0; i < curr->total_files; i++)
    {
        if(curr->ref[i] == 1)
        {
            curr->ref[i] = 0;
            curr->free_count--;
            release(&cache->lock);
            return (char*)curr->start_address + i * cache->file_size;
        }
    }

    release(&cache->lock);
    return 0;
}


void free_file(struct slab_cache * cache , char * addr)
{
    acquire(&(cache->lock));

    struct slab * curr = cache->slab_list;

    while(curr)
    {
        char * start = (char *)curr->start_address;
        char* end = start + (curr->total_files*cache->file_size);

        if(addr >= start && addr< end)
        {
            int index = (addr -start) / cache->file_size;

            if(curr->ref[index] == 0)
            {
                curr->ref[index] = 1;
                curr->free_count++;
            }
            break;
        }
        curr = curr->next;
    }

    release(&(cache->lock));

}