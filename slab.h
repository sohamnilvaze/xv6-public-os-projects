#define no_of_files_in_slab PGSIZE / sizeof(struct file)

struct slab{
    struct slab * next;
    void * start_address;
    uint free_count;
    uint total_files;
    int * ref;
};

struct slab_cache{
    struct spinlock lock;
    struct slab * slab_list;
    uint file_size;
    uint slab_size;
};


void slab_free(struct slab_cache *cache, struct slab *s);

void slab_cache_init(struct slab_cache * cache,uint file_size);

struct slab * slab_init(struct slab_cache * cache);

char * file_alloc(struct slab_cache * cache);

void free_file(struct slab_cache * cache , char * addr);





