#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>

typedef struct _linked_node linked_node;

struct _linked_node {
	char* image;

	void* next;
};

linked_node* add_node_to_list(linked_node* prev, char* data);
linked_node* wind_to_tail(linked_node* node);
void free_list(linked_node* head);

#endif /*__LIST_H__*/
