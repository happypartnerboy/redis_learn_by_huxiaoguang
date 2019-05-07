
#include "malloc.h"
#include <stdio.h>
#include <string.h>
#define PREFIX_SIZE (0)

void *zmalloc(size_t size)
 {
    void *ptr = malloc(size+PREFIX_SIZE);

    if (!ptr) 
        return NULL; // TO DO  内存失败zmalloc_oom_handler(size);

    *((size_t*)ptr) = size;
    
    return (char*)ptr+PREFIX_SIZE;
}

void *zfree(void *ptr)
{
    if(NULL == ptr)
    {
        return NULL;
    }

    free(ptr);
    ptr = NULL;
    return ptr;
}