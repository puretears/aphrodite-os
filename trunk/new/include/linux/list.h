#ifndef LIST_H
#define LIST_H

#include "linux/type.h"

// member is a field of type
// offset_of gets the offset of member in type
#define offset_of(type, member) \
	((size_t)&((type *)0)->member)

#define container_of(ptr, type, member) ({ \
	typeof( ((type *)0)->member ) *_ptr = ptr; \
	(type *)( (char *)_ptr - offset_of(type, member) ); })

struct list_head {
	struct list_head *prev, *next;
};

// Initialize
#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)
// Init a list at run time.
#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->prev = (ptr); (ptr)->next = (ptr); \
	} while (0)

static inline void __list_add(struct list_head *new_node, 
		struct list_head *prev, struct list_head *next) {
	new_node->prev = prev;
	prev->next = new_node;
	new_node->next = next;
	next->prev = new_node;
}

static inline void list_add(struct list_head *new_node, 
		struct list_head *head) {
	__list_add(new_node, head, head->next);
}

static inline void list_add_tail(struct list_head *new_node,
		struct list_head *head) {
	__list_add(new_node, head->prev, head);
}

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/* Double linked lists with a single pointer list head
 * Most useful for hash tables where the two pointer list head is too
 * wasteful.
 * */

struct hlist_node {
	struct hlist_node *next;
	struct hlist_node **pprev;
};

struct hlist_head {
	struct hlist_node *first;
};

#define HLIST_HEAD_INIT { .first = NULL } 
#define HLIST_HEAD(name) struct hlist_head name = HLIST_HEAD_INIT
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)
#define INIT_HLIST_NODE(ptr) ((ptr)->prev = NULL, (ptr)->pprev = NULL)

static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h) {
	struct hlist_node *first = h->first;
	n->next = first;
	
	if (first)
		first->pprev = &n->next;

	h->first = n;
	n->pprev = &h->first;
}

#endif
