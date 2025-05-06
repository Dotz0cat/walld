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

#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <string.h>
#include <libconfig.h>
#include <sys/stat.h>

#include "list.h"

struct settings {
	int colors;

	int dark;

	int xrdb_use;

	char *feh_path;

	char *xrdb_path;

	int minutes;

	char *bg_style;

	char *x_auth;

	char *display;

	int monitors;

	struct linked_node *sources;

	struct linked_node *xrdb_argv;
};

struct xdg_directories {
	char *home_dir;
	char *xdg_config_home;
	char *xdg_config_dir;
	char *xdg_data_home;
	char *xdg_data_dirs;
	char *xdg_state_home;
	char *xdg_runtime_dir;
};

struct settings *read_config(const char *restrict config_file, const struct xdg_directories *restrict xdg_dir, const char *source_from_line);
struct linked_node *list_file_parse(const char *list_file);
void produce_default_config(const char *output_file, const char *home_dir);

#endif /* CONFIG_H */
