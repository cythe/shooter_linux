#pragma once

/* 这个文件抄的linux源码: include/linux/list.h */

struct list_head {
    struct list_head* next, * prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

/* 创建一个列表用该接口 */
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head* list)
{
    list->next = list;
    list->prev = list;
}

static inline int list_is_head(const struct list_head* list,
	const struct list_head* head)
{
    return list == head;
}

static inline int list_empty(const struct list_head* head)
{
    return head->next == head;
}

static inline void __list_add(struct list_head* new,
	struct list_head* prev,
	struct list_head* next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/* 头插法插入node */
static inline void list_add(struct list_head* new, struct list_head* head)
{
    __list_add(new, head, head->next);
}

/* 尾插法插入node */
static inline void list_add_tail(struct list_head* new, struct list_head* head)
{
    __list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head* prev, struct list_head* next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void __list_del_entry(struct list_head* entry)
{
    __list_del(entry->prev, entry->next);
}

/* 删除node (迭代中会断链) */
static inline void list_del(struct list_head* entry)
{
    __list_del_entry(entry);
    entry->next = (struct list_head*)0x100;
    entry->prev = (struct list_head*)0x200;
}

/* 删除node (更新迭代器) */
static inline struct list_head* list_del_update_pos(struct list_head* entry)
{
    struct list_head* p = entry->prev;
    __list_del_entry(entry);
    entry->next = (struct list_head*)0x100;
    entry->prev = (struct list_head*)0x200;
    return p;
}

/* 对list进行遍历 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; !list_is_head(pos, (head)); pos = pos->next)

/*
 * 和GNU_C不同, 没有typeof()推断, 也就无法实现container_of()这个宏定义.
 * 万幸offsetof()还能用...所以手动将list转为容器指针, 适用于包含list_head的结构体
 * 
 * note: 还有个比较骚的方法, 就是把list放在结构体首位...
 */
#define list_to_(name) \
    static name* list_to_##name(struct list_head *list) \
{ \
    char* __mptr = (void*)(list); \
    return (name *)(__mptr - offsetof(name, list)); \
}
