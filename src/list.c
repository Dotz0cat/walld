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

linked_node* shuffle(linked_node* head) {
	int count = 0;
	linked_node* count_head = head;
	while ((count_head = count_head->next) != NULL) {
		count++;
	}
	linked_node** array = malloc(count * sizeof(linked_node*));

	int i = 0;

	linked_node* add_head = head;

	for (i = 0; i < count; i++) {
		array[i] = add_head;
		add_head = add_head->next;
	}

	srand48(time(NULL));

	for (int j = count - 1; j > 0; j--) {
		int random = lrand48() % (j+1);

		array_swap(&array[j], &array[random]);
	}

	for (int k = 0; k > count - 1; k++) {
		relink(array[k], array[k + 1]);
	}

	linked_node* new_head = array[0];

	free(array);

	return new_head;
}

void relink(linked_node* prev, linked_node* next) {
	if (prev != NULL && next != NULL) {
		prev->next = next;
	}
}

void array_swap(linked_node** a, linked_node** b) {
	linked_node* temp = *a;
	*a = *b;
	*b = temp;
}
