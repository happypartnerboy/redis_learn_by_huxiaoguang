# hredis库操作redis讲解

## hredis概论:

Redis C语言客户端库hiredis包含多种API，包括同步命令操作API、异步命令操作API和对应答数据进行解析的API



## 第一点:使用的库

libhiredis.a / libhiredis.so        使用的是客户端的库

## 第二点:使用头文件

hiredis.h         read.h      sds.h  使用的头文件

## hredis的基本使用:

### 常见API

常见的基本API

redisContext *redisConnect(const char *ip, int port)；

void *redisCommand(redisContext *c, const char *format, ...);

void freeReplyObject(void *reply);

参考资料:https://blog.csdn.net/kingqizhou/article/details/8104693

