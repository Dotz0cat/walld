/*
Copyright 2025 Dotz0cat

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

#ifndef LIST_PRIVATE_H
#define LIST_PRIVATE_H

#include "list.h"

#include "magic.h"
#include "config.h"

static inline void relink(struct linked_node *prev, struct linked_node *next);
static void array_swap(struct linked_node **a, struct linked_node **b);
static enum file_type get_file_type(const char *path);
static char *realpath_wrap(const char *path, const char *dir);

#endif /* LIST_PRIVATE_H */