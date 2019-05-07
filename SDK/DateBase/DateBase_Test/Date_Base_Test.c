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
