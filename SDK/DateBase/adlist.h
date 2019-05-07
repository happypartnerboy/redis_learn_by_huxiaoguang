
#ifndef __ADLIST_H__
#define __ADLIST_H__

/* Node, List, and Iterator are the only data structures used currently. */

typedef struct listNode {
    struct listNode *prev;
    struct listNode *next;
    void *value;			//方便链表保存不同的数据的类型 ，注意空间的释放
} listNode;

typedef struct listIter {
    listNode *next;
    int direction;
} listIter;

typedef struct list {
    listNode *head;
    listNode *tail;
    void *(*dup)(void *ptr);
    void (*free)(void *ptr);
    int (*match)(void *ptr, void *key);
    unsigned long len;
} list;

/* Functions implemented as macros */
#define listLength(l) ((l)->len)
#define listFirst(l) ((l)->head)
#define listLast(l) ((l)->tail)
#define listPrevNode(n) ((n)->prev)
#define listNextNode(n) ((n)->next)
#define listNodeValue(n) ((n)->value)

#define listSetDupMethod(l,m) ((l)->dup = (m))
#define listSetFreeMethod(l,m) ((l)->free = (m))
#define listSetMatchMethod(l,m) ((l)->match = (m))

#define listGetDupMethod(l) ((l)->dup)
#define listGetFree(l) ((l)->free)
#define listGetMatchMethod(l) ((l)->match)

/* Prototypes */
//创建一个头结点
list *listCreate(void);

//删除整个list链表
void listRelease(list *list);
void listEmpty(list *list);

//插入相关的函数         
list *listAddNodeHead(list *list, void *value);
list *listAddNodeTail(list *list, void *value);
list *listInsertNode(list *list, listNode *old_node, void *value, int after);

//删除某个节点
void listDelNode(list *list, listNode *node);

//创建一个 listIter方面为direction 
listIter *listGetIterator(list *list, int direction);

//获取依据 iter->next 值，之后在依据方向移动iter->next
listNode *listNext(listIter *iter);

//释放iter空间大小
void listReleaseIterator(listIter *iter);

//重新设置listIter li的值
void listRewind(list *list, listIter *li);
void listRewindTail(list *list, listIter *li);

//赋值orig到新的list空间中,采用一个一个拷贝的方法
list *listDup(list *orig);

//查找 通过KEY值和索引
listNode *listSearchKey(list *list, void *key);
listNode *listIndex(list *list, long index);

//将为节点放到头部
void listRotate(list *list);

//两个list的用法 目标将o 合并到 l上
void listJoin(list *l, list *o);

/* Directions for iterators */
#define AL_START_HEAD 0
#define AL_START_TAIL 1

#endif /* __ADLIST_H__ */
