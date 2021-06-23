#include "list.h"

linked_node* add_node_to_list(linked_node* prev, char* data) {
	if (prev == NULL) {
		linked_node* node = malloc(sizeof(linked_node));

		node->image = data;

		node->next = NULL;

		return node;
	}
	else {
		if (prev->image == NULL) {
			prev->image = data;
			prev->next = NULL;

			return prev;
		}
		else {
			linked_node* node = malloc(sizeof(linked_node));

			prev->next = node;

			node->image = data;

			node->next = NULL;

			return node;
		}
	}
}

linked_node* wind_to_tail(linked_node* node) {
	while (node->next != NULL) {
		node = node->next;
	}

	return node;
}

void free_list(linked_node* head) {
	linked_node* temp;

	while (head->next != NULL) {
		temp = head;
		if (temp->image != NULL) {
			free(temp->image);
		}
		head = temp->next;
		free(temp);
	}
}
