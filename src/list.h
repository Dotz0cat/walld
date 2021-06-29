#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>
#include <time.h>

typedef struct _linked_node linked_node;

struct _linked_node {
	char* image;

	void* next;
};

linked_node* add_node_to_list(linked_node* prev, char* data);
linked_node* wind_to_tail(linked_node* node);
void free_list(linked_node* head);
linked_node* shuffle(linked_node* head);
void relink(linked_node* prev, linked_node* next);
void array_swap(linked_node** a, linked_node** b);

#endif /*__LIST_H__*/
