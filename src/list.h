/*
Copyright 2021-2022, 2025 Dotz0cat

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

struct linked_node {
	char *image;

	void *next;
};

struct linked_node *add_node_to_list(struct linked_node *prev, char *data);
struct linked_node *wind_to_tail(struct linked_node *node);
struct linked_node *wind_to_x(struct linked_node *node, int x);
void free_list(struct linked_node *head);
void free_circular_list(struct linked_node *head);
struct linked_node *shuffle(struct linked_node *head);
struct linked_node *get_images(struct linked_node *source);
struct linked_node *list_files_full(const char *directory);
char **list_to_null_termed_string_array(struct linked_node *head, size_t *len);

#endif /* LIST_H */
