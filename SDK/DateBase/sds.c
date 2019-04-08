#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "sds.h"
#include "malloc.h"
sds sdsnewlen(const void *init, size_t initlen) {
    struct sdshdr *sh;
    //第一步分配initlen + 1 的空间
    sh = zmalloc(sizeof(struct sdshdr)+initlen+1);
    if (sh == NULL) 
    {
        return NULL;
    }

    //对结构体进行赋值
	sh->len = initlen;
	sh->free = 0;

    //如果初始化，就可以进行赋值操作
    if (initlen && init)
    {
        memcpy(sh->buf, init, initlen);
    }

    //注意最后加上'\0' 
    sh->buf[initlen] = '\0';

	return (char*)sh->buf;
}


