#ifndef LIST_H
#define LIST_H

typedef struct list list_t;

struct list {
    list_t *prev;
    list_t *next;
};

static inline void list_init(list_t *list)
{
    list->next = list;
    list->prev = list;
}

static inline void list_unlink(list_t *node)
{
    node->next->prev = node->prev;
    node->prev->next = node->next;
}

static inline void list_push(list_t *list, list_t *node)
{
    node->prev = list->prev;
    node->next = list;
    node->prev->next = node;
    node->next->prev = node;
}

static inline list_t *list_pop(list_t *list)
{
    list_t *node = list->prev;
    list_unlink(node);
    return node;
}

static inline int list_empty(list_t *list)
{
	return list->next == list && list->prev == list;
}

#define list_foreach(list, iterator)                \
	for (iterator = (list)->next; iterator != list; \
	     iterator = (iterator)->next)


#endif /* LIST_H */