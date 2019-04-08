#ifndef __SDS_H
#define __SDS_H
typedef char *sds;

struct sdshdr {
    unsigned int len;
    unsigned int free;
    char buf[];
};

/*!
 * \brief 计算sds s中buf的长度
 输入参数:
 * \param   s sds类型的字符串
 * \return  返回s对应sdshdr的len的属性
 */
static inline size_t sdslen(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->len;
}

/*!
 * \brief 计算sds s中free的多余空间
 输入参数:
 * \param   s sds类型的字符串
 * \return  返回s对应sdshdr的free的属性
 */
static inline size_t sdsavail(const sds s) {
    struct sdshdr *sh = (void*)(s-(sizeof(struct sdshdr)));
    return sh->free;
}

/*!
 * \brief 通过init指针和initlen长度创建一个新的sds数据串类型
 *  You can print the string with printf() as there is an implicit \0 at the
 *  end of the string. However the string is binary safe and can contain
 *  '\0' characters in the middle, as the length is stored in the sds header
 输入参数:
 * \param : init 指向一个空间   
 * \param : initlen 申请的长度
 * \return  成功则返回有效空间的指针 ，失败则返回NULL
 */
sds sdsnewlen(const void *init, size_t initlen);

#endif
