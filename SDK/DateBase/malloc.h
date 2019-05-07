#ifndef __MALLOC_H
#define __MALLOC_H
 
#include <stdlib.h> 
 
/*!
 * \brief 调用malloc分别，大小为 size的空间
 输入参数:
 * \param   size空间的大小
 * \return  失败则返回NULL,成功则返回指向有效地址的空间
 */
void *zmalloc(size_t size);

void *zfree(void *ptr);
#endif