#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pwd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <regex.h>

#include "magic.h"

typedef struct _linked_node linked_node;

struct _linked_node {
	char* image;

	void* next;
};

linked_node* add_node_to_list(linked_node* prev, char* data);
linked_node* wind_to_tail(linked_node* node);
void free_list(linked_node* head);
linked_node* shuffle(linked_node* head);
static inline void relink(linked_node* prev, linked_node* next);
void array_swap(linked_node** a, linked_node** b);
linked_node* get_images(linked_node* source);
file_type get_file_type(const char* path);
linked_node* list_files_full(const char* directory);
char* realpath_wrap(const char* path, const char* dir);

#include "config.h"

#endif /*__LIST_H__*/
