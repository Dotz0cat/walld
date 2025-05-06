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

#include "config_private.h"

struct settings *read_config(const char *restrict config_file, const struct xdg_directories *restrict xdg_dir, const char *source_from_line) {
	struct settings *options = malloc(sizeof(struct settings));

	config_t config;
	config_setting_t *setting;
	const char *source;
	int colors;
	int dark;
	int use_xrdb;
	const char *bg_style;
	const char *x_auth;
	const char *display;
	int minutes;
	int monitors;
	const char *feh_path;
	const char *xrdb_path;
	const char *xresources;

	default_settings(xdg_dir, options);

	//check if file exsits
	if (file_exsits(config_file) != 0) {
		return options;
	}

	// check_default_image_folder(home_dir);

	config_init(&config);

	if (config_read_file(&config, config_file) != CONFIG_TRUE) {
		config_destroy(&config);
		return options;
	}

	if (config_lookup_bool(&config, "colors", &colors)) {
		options->colors = colors;
	}

	if (config_lookup_bool(&config, "dark", &dark)) {
		options->dark = dark;
	}

	if (config_lookup_bool(&config, "use-xrdb", &use_xrdb)) {
		options->xrdb_use = use_xrdb;
	}

	if (config_lookup_string(&config, "bg-style", &bg_style)) {
		free(options->bg_style);
		options->bg_style = strdup(bg_style);
	}


	if (config_lookup_string(&config, "XAUTHORITY", &x_auth)) {
		//NULL by default
		options->x_auth = strdup(x_auth);
	}

	if (config_lookup_string(&config, "DISPLAY", &display)) {
		//NULL by default
		options->display = strdup(display);
	}

	if (config_lookup_int(&config, "monitors", &monitors)) {
		options->monitors = monitors;
	}

	if (config_lookup_string(&config, "feh-path", &feh_path)) {
		//NULL by default
		options->feh_path = strdup(feh_path);
	}

	if (config_lookup_int(&config, "minutes", &minutes)) {
		options->minutes = minutes;
	}

	if (source_from_line != NULL) {
		source = source_from_line;
	}
	else {
		if (!config_lookup_string(&config, "source_to_use", &source)) {
			source = "default";
		}
	}
	
	setting = config_lookup(&config, source);

	if (setting != NULL) {
		free_list(options->sources);

		int count = config_setting_length(setting);

		struct linked_node *entry_point = add_node_to_list(NULL, NULL);

		struct linked_node *current = entry_point;

		options->sources = entry_point;


		for (int i = 0; i < count; i++) {
			char *item = strdup(config_setting_get_string_elem(setting, i));

			current = add_node_to_list(current, item);
		}
	}

	if (config_lookup_string(&config, "xrdb-path", &xrdb_path)) {
		options->xrdb_path = strdup(xrdb_path);
	}

	config_setting_t *xrdb_args;

	xrdb_args = config_lookup(&config, "xrdb-args");

	if (xrdb_args != NULL) {
		free_list(options->xrdb_argv);

		struct linked_node *entry_point = add_node_to_list(NULL, NULL);

		struct linked_node *current = entry_point;

		options->xrdb_argv = entry_point;

		int count = config_setting_length(xrdb_args);

		for (int i = 0; i < count; i++) {
			char *item = strdup(config_setting_get_string_elem(setting, i));

			current = add_node_to_list(current, item);
		}

		if (config_lookup_string(&config, "xresources", &xresources)) {
			current = add_node_to_list(current, strdup(xresources));
		}
		else {
			int char_count = snprintf(NULL, 0, "%s%s", xdg_dir->home_dir, "/.Xresources");

			char *xresources_new = malloc(char_count + 1U);

			if (xresources_new == NULL) {
				abort();
			}

			snprintf(xresources_new, char_count + 1U, "%s%s", xdg_dir->home_dir, "/.Xresources");
			current = add_node_to_list(current, xresources_new);
		}
	}
	else if (config_lookup_string(&config, "xresources", &xresources)) {
		struct linked_node *current = wind_to_tail(options->xrdb_argv);
		replace_node(current, xresources);
	}

	config_destroy(&config);

	return options;
}

struct linked_node *list_file_parse(const char *list_file) {
	struct linked_node *entry_point = add_node_to_list(NULL, NULL);

	struct linked_node *current = entry_point;

	config_t config;
	config_setting_t *setting;
	const char *source;

	config_init(&config);

	if (config_read_file(&config, list_file) != CONFIG_TRUE) {
		config_destroy(&config);
		return NULL;
	}

	if (config_lookup_string(&config, "list", &source)) {
		//NOP
	}
	else {
		source = "default";
	}

	setting = config_lookup(&config, source);

	if (setting != NULL) {
		int count = config_setting_length(setting);

		for (int i = 0; i < count; i++) {
			char *item = strdup(config_setting_get_string_elem(setting, i));

			current = add_node_to_list(current, item);
		}
	}
	else {
		free(entry_point);
		config_destroy(&config);
		return NULL;
	}

	config_destroy(&config);

	return entry_point;
}

void produce_default_config(const char *output_file, const char *home_dir) {
	config_t cfg;

	config_init(&cfg);

	config_setting_t *root;

	root = config_root_setting(&cfg);

	config_setting_t *colors;

	colors = config_setting_add(root, "colors", CONFIG_TYPE_BOOL);
	config_setting_set_bool(colors, CONFIG_FALSE);

	config_setting_t *dark;

	dark = config_setting_add(root, "dark", CONFIG_TYPE_BOOL);
	config_setting_set_bool(dark, CONFIG_TRUE);

	config_setting_t *minutes;

	minutes = config_setting_add(root, "minutes", CONFIG_TYPE_INT);
	config_setting_set_int(minutes, 30);

	config_setting_t *monitors;

	monitors = config_setting_add(root, "monitors", CONFIG_TYPE_INT);
	config_setting_set_int(monitors, 1);

	config_setting_t *source;

	source = config_setting_add(root, "source_to_use", CONFIG_TYPE_STRING);
	config_setting_set_string(source, "default");

	config_setting_t *default_source;

	default_source = config_setting_add(root, "default", CONFIG_TYPE_ARRAY);

	//make a string
	int char_count = snprintf(NULL, 0, "%s%s", home_dir, "/.walld/images");
	if (char_count <= 0) {
		//tough luck
		abort();
	}
	char *default_folder = malloc(char_count + 1U);

	if (default_folder == NULL) {
		//tough luck
		abort();
	}

	snprintf(default_folder, char_count + 1U, "%s%s", home_dir, "/.walld/images");

	config_setting_t *array;

	array = config_setting_add(default_source, NULL, CONFIG_TYPE_STRING);
	config_setting_set_string(array, default_folder);

	if (config_write_file(&cfg, output_file) != CONFIG_TRUE) {
		//if this fails I cannot help you
		abort();
	}

	config_destroy(&cfg);
	free(default_folder);
}

//0 if exsits 1 if not
static inline int file_exsits(const char *file) {
	FILE *fp;

	if ((fp = fopen(file, "r"))) {
		fclose(fp);
		return 0;
	}
	else {
		return 1;
	}
}

static inline int folder_exsits(const char *folder) {
	DIR *dir;

	dir = opendir(folder);

	if (dir) {
		closedir(dir);
		return 0;
	}
	else {
		return 1;
	}
}

static inline void check_image_folder(const char *image_folder) {
	if (folder_exsits(image_folder) != 0) {
		mkdir(image_folder, 0777);
	}
}

static void default_settings(const struct xdg_directories *restrict xdg_dir, struct settings *options) {
	memcpy(options, &DEFAULT_CONFIG, sizeof(struct settings));

	//set bg_style
	options->bg_style = strdup("--bg-scale");

	//populate sources
	struct linked_node *entry_point = add_node_to_list(NULL, NULL);
	struct linked_node *current = entry_point;
	options->sources = entry_point;
	int char_count = snprintf(NULL, 0, "%s%s", xdg_dir->xdg_data_home, "/walld/images");
	if (char_count <= 0) {
		//tough luck
		abort();
	}
	char *default_folder = malloc(char_count + 1U);

	if (default_folder == NULL) {
		//tough luck
		abort();
	}

	snprintf(default_folder, char_count + 1U, "%s%s", xdg_dir->xdg_data_home, "/walld/images");

	current = add_node_to_list(current, default_folder);

	char *system_folder;

	char *save_dir;

	if (xdg_dir->xdg_data_dirs == NULL) {
		return;
	}

	while ((system_folder = strtok_r(xdg_dir->xdg_data_dirs, ":", &save_dir)) != NULL) {
		char_count = snprintf(NULL, 0, "%s%s", system_folder, "/walld/images");
		if (char_count <= 0) {
			//tough luck
			abort();
		}
		char *default_system_folder = malloc(char_count + 1U);

		if (default_system_folder == NULL) {
			//tough luck
			abort();
		}

		snprintf(default_system_folder, char_count + 1U, "%s%s", system_folder, "/walld/images");

		current = add_node_to_list(current, default_system_folder);
	}

	//populate xrdb arguments
	entry_point = add_node_to_list(NULL, NULL);
	current = entry_point;
	options->xrdb_argv = current;

	current = add_node_to_list(current, strdup("--load"));

	char *xresources;

	char_count = snprintf(NULL, 0, "%s%s", xdg_dir->home_dir, "/.Xresources");

	xresources = malloc(char_count + 1U);

	if (xresources == NULL) {
		abort();
	}

	snprintf(xresources, char_count + 1U, "%s%s", xdg_dir->home_dir, "/.Xresources");

	current = add_node_to_list(current, xresources);

	return;
}
