/*
Copyright 2021 Dotz0cat

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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdlib.h>
#include <string.h>
#include <libconfig.h>
#include <sys/stat.h>

#include "list.h"

typedef struct _settings settings;

struct _settings {
	int colors;

	int dark;

	char* feh_path;

	char* xrdb_path;

	int minutes;

	char* bg_style;

	char* x_auth;

	char* display;

	linked_node* sources;

	linked_node* xrdb_argv;
};

settings* read_config(const char* config_file, const char* home_dir);
linked_node* list_file_parse(const char* list_file);
void produce_default_config(const char* output_file, const char* home_dir);
static inline int file_exsits(const char* file);
static inline int folder_exsits(const char* folder);
static inline void check_default_image_folder(const char* home_dir);

#endif /*__CONFIG_H__*/
