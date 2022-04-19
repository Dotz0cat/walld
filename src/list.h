/*
Copyright 2021-2022 Dotz0cat

This file is part of walld.

    walld is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    walld is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with walld.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef LIST_H
#define LIST_H

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
linked_node* wind_to_x(linked_node* node, int x);
void free_list(linked_node* head);
void free_circular_list(linked_node* head);
linked_node* shuffle(linked_node* head);
static inline void relink(linked_node* prev, linked_node* next);
void array_swap(linked_node** a, linked_node** b);
linked_node* get_images(linked_node* source);
file_type get_file_type(const char* path);
linked_node* list_files_full(const char* directory);
char* realpath_wrap(const char* path, const char* dir);
char** list_to_null_termed_string_array(linked_node* head, size_t* len);

#include "config.h"

#endif /* LIST_H */
