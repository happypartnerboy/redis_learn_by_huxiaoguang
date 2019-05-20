/*
#include<stdio.h>
#include<string.h>
#include "../sds.h"
int main()
{
    sds sdcTestdate = NULL;
    int nSdcLen = 0;
    int nSdcfree = 0;
    sdcTestdate = sdsnewlen("huxiaoguang",strlen("huxiaoguang"));
    if(NULL == sdcTestdate)
    {
        printf("error");
    }

    nSdcLen = sdslen(sdcTestdate);
    printf("nSdcLen = %d\n",nSdcLen);

    nSdcfree = sdsavail(sdcTestdate);
    printf("nSdcfree = %d\n",nSdcfree);
    
    printf("hello world\n");
    return 0;
}
*/
/*
#include<stdio.h>
#include<string.h>
#include "../adlist.h"
int main()
{
   //插入和打印 
   //创建一个list
    list *LPtest = listCreate();
    listIter *ITPtest = NULL;
    listNode *LNPtest = NULL;
    if(NULL == LPtest)
    {
        printf("%s %d create error\n",__FILE__,__LINE__);
    }
    //插入数据
    listAddNodeTail(LPtest, (void *)1);
    listAddNodeTail(LPtest, (void *)2);
    listAddNodeTail(LPtest, (void *)3);
    listAddNodeTail(LPtest, (void *)4);
    printf("%s %d LPtest->len = %ld\n",__FILE__,__LINE__,LPtest->len);
    //创建iteror
    ITPtest = listGetIterator(LPtest, AL_START_TAIL);
    //打印数据
    while ((LNPtest = listNext(ITPtest)) != NULL) 
    {
        printf("%s %d LNPtest->value = %d\n",__FILE__,__LINE__,(int)LNPtest->value); 
    }

    //释放iteror 和 list
    listReleaseIterator(ITPtest);
    listRelease(LPtest);
    return 0;
}
*/

#include <stdio.h>
#include "../dict.h"
#include <strings.h>
#include <stdlib.h>
//计算hash函数
unsigned int dictSdsHash(const void *key) {
    return dictGenHashFunction((unsigned char*)key, strlen((char*)key));
}

void sdsfree(char* s) {
    if (s == NULL) 
		return;
    free(s);
}

//
int dictSdsKeyCompare(void *privdata, const void *key1,
        const void *key2)
{
 
    return strcasecmp(key1, key2) == 0;
}

static void dictSdsDestructor(void *privdata, void *val)
{
	printf("hello word\n");
//  free(val);
}

static dictType Dicttest = 
{
    dictSdsHash,                /* hash function */
    NULL,                       /* key dup */
    NULL,                       /* val dup */
    dictSdsKeyCompare,          /* key compare */
	dictSdsDestructor,          /* key destructor */
    NULL                        /* val destructor */
};


int main()
{
	//创建 hasi
	dict  * pdicttest  = NULL;	
	dictEntry  * pdictEntry = NULL; //获取value
	//申请一个hash
	pdicttest = dictCreate(&Dicttest, NULL);

	//添加数据
	/*
		hu  	2
		yang	3
		xiao	4
	*/
	dictAdd(pdicttest, "hu", 2);
	dictAdd(pdicttest, "yang", 3);
	dictAdd(pdicttest, "xiao", 4);
	printf("size = %d used = %d\n",pdicttest->size , pdicttest->used);
		
	
	//查找数据
	pdictEntry = dictFind(pdicttest, "hu");
	if(NULL == pdictEntry)
	{
		printf("1_error\n");
		return 0;
	}
	printf("%s %d\n",pdictEntry->key , pdictEntry->val);
	
	pdictEntry = dictFind(pdicttest, "yang");
	if(NULL == pdictEntry)
	{
		printf("2_error\n");
		return 0;
	}
	printf("%s %d\n",pdictEntry->key , pdictEntry->val);
	
	pdictEntry = dictFind(pdicttest, "xiao");
	if(NULL == pdictEntry)
	{
		printf("2_error\n");
		return 0;
	}
	printf("%s %d\n",pdictEntry->key , pdictEntry->val);
	
	
	//删除数据
	dictDelete(&pdicttest, "huxiaoguang");
	printf("size = %d used = %d\n",pdicttest->size , pdicttest->used);
	return 0;
}













