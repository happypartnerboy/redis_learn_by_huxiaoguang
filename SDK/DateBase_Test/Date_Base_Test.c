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