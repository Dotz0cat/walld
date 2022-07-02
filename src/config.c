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

#include "config.h"

settings* read_config(const char* config_file, const char* home_dir, const char* source_from_line) {
	settings* options = malloc(sizeof(settings));

	config_t config;
	config_setting_t* setting;
	const char* source;
	int colors;
	int dark;
	int use_xrdb;
	const char* bg_style;
	const char* x_auth;
	const char* display;
	int minutes;
	int monitors;
	const char* feh_path;
	const char* xrdb_path;
	const char* xresources;

	//check if file exsits
	if (file_exsits(config_file) != 0) {

		int char_count = snprintf(NULL, 0, "%s%s", home_dir, "/.walld");
		if (char_count <= 0) {
			//tough luck
			abort();
		}
		char* walld_folder = malloc(char_count + 1U);

		if (walld_folder == NULL) {
			//tough luck
			abort();
		}

		snprintf(walld_folder, char_count + 1U, "%s%s", home_dir, "/.walld");

		if (folder_exsits(walld_folder) != 0) {
			if (mkdir(walld_folder, 0777) == -1) {
				abort();
			}
		}

		free(walld_folder);

		produce_default_config(config_file, home_dir);
	}

	check_default_image_folder(home_dir);

	config_init(&config);

	if (config_read_file(&config, config_file) != CONFIG_TRUE) {
		free(options);
		config_destroy(&config);
		return NULL;
	}

	if (config_lookup_bool(&config, "colors", &colors)) {
		options->colors = colors;
	}
	else {
		options->colors = CONFIG_FALSE;
	}

	if (config_lookup_bool(&config, "dark", &dark)) {
		options->dark = dark;
	}
	else {
		options->dark = CONFIG_FALSE;
	}

	if (config_lookup_bool(&config, "use-xrdb", &use_xrdb)) {
		options->xrdb_use = use_xrdb;
	}
	else {
		options->xrdb_use = CONFIG_FALSE;
	}

	if (config_lookup_string(&config, "bg-style", &bg_style)) {
		options->bg_style = strdup(bg_style);
	}
	else {
		options->bg_style = strdup("--bg-scale");
	}

	if (config_lookup_string(&config, "XAUTHORITY", &x_auth)) {
		options->x_auth = strdup(x_auth);
	}
	else {
		options->x_auth = NULL;
	}

	if (config_lookup_string(&config, "DISPLAY", &display)) {
		options->display = strdup(display);
	}
	else {
		options->display = NULL;
	}

	if (config_lookup_int(&config, "monitors", &monitors)) {
		options->monitors = monitors;
	}
	else {
		options->monitors = 1;
	}

	if (config_lookup_string(&config, "feh-path", &feh_path)) {
		options->feh_path = strdup(feh_path);
	}
	else {
		options->feh_path = NULL;
	}

	if (config_lookup_int(&config, "minutes", &minutes)) {
		options->minutes = minutes;
	}
	else {
		options->minutes = 30;
	}

	if (source_from_line != NULL) {
		source = source_from_line;
	}
	else {
		if (config_lookup_string(&config, "source_to_use", &source)) {
		//NOP
		}
		else {
			source = "default";
		}
	}
	
	setting = config_lookup(&config, source);

	if (setting != NULL) {
		int count = config_setting_length(setting);

		linked_node* entry_point = add_node_to_list(NULL, NULL);

		linked_node* current = entry_point;

		options->sources = entry_point;


		for (int i = 0; i < count; i++) {
			char* item = strdup(config_setting_get_string_elem(setting, i));

			current = add_node_to_list(current, item);
		}
	}
	else {
		options->sources = add_node_to_list(NULL, NULL);
		int char_count = snprintf(NULL, 0, "%s%s", home_dir, "/.walld/images");
		if (char_count <= 0) {
			//tough luck
			abort();
		}
		char* default_folder = malloc(char_count + 1U);

		if (default_folder == NULL) {
			//tough luck
			abort();
		}

		snprintf(default_folder, char_count + 1U, "%s%s", home_dir, "/.walld/images");

		options->sources = add_node_to_list(options->sources, default_folder);
	}

	if (config_lookup_string(&config, "xrdb-path", &xrdb_path)) {
		options->xrdb_path = strdup(xrdb_path);
	}
	else {
		options->xrdb_path = NULL;
	}

	config_setting_t* xrdb_args;

	linked_node* entry_point = add_node_to_list(NULL, NULL);

	linked_node* current = entry_point;

	options->xrdb_argv = entry_point;

	xrdb_args = config_lookup(&config, "xrdb-args");

	if (xrdb_args != NULL) {
		int count = config_setting_length(xrdb_args);

		for (int i = 0; i < count; i++) {
			char* item = strdup(config_setting_get_string_elem(setting, i));

			current = add_node_to_list(current, item);
		}
	}
	else {

		current = add_node_to_list(current, strdup("--load"));
	}

	//xresources

	if (config_lookup_string(&config, "xresources", &xresources)) {
		current = add_node_to_list(current, strdup(xresources));
	}
	else {
		int char_count = snprintf(NULL, 0, "%s%s", home_dir, "/.Xresources");
		if (char_count <= 0) {
			//tough luck
			abort();
		}
		char* xres = malloc(char_count + 1U);

		if (xres == NULL) {
			//tough luck
			abort();
		}

		snprintf(xres, char_count + 1U, "%s%s", home_dir, "/.Xresources");

		current = add_node_to_list(current, xres);
	}

	config_destroy(&config);

	return options;
}

linked_node* list_file_parse(const char* list_file) {
	linked_node* entry_point = add_node_to_list(NULL, NULL);

	linked_node* current = entry_point;

	config_t config;
	config_setting_t* setting;
	const char* source;

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
			char* item = strdup(config_setting_get_string_elem(setting, i));

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

void produce_default_config(const char* output_file, const char* home_dir) {
	config_t cfg;

	config_init(&cfg);

	config_setting_t* root;

	root = config_root_setting(&cfg);

	config_setting_t* colors;

	colors = config_setting_add(root, "colors", CONFIG_TYPE_BOOL);
	config_setting_set_bool(colors, CONFIG_FALSE);

	config_setting_t* dark;

	dark = config_setting_add(root, "dark", CONFIG_TYPE_BOOL);
	config_setting_set_bool(dark, CONFIG_TRUE);

	config_setting_t* minutes;

	minutes = config_setting_add(root, "minutes", CONFIG_TYPE_INT);
	config_setting_set_int(minutes, 30);

	config_setting_t* monitors;

	monitors = config_setting_add(root, "monitors", CONFIG_TYPE_INT);
	config_setting_set_int(monitors, 1);

	config_setting_t* source;

	source = config_setting_add(root, "source_to_use", CONFIG_TYPE_STRING);
	config_setting_set_string(source, "default");

	config_setting_t* default_source;

	default_source = config_setting_add(root, "default", CONFIG_TYPE_ARRAY);

	//make a string
	int char_count = snprintf(NULL, 0, "%s%s", home_dir, "/.walld/images");
	if (char_count <= 0) {
		//tough luck
		abort();
	}
	char* default_folder = malloc(char_count + 1U);

	if (default_folder == NULL) {
		//tough luck
		abort();
	}

	snprintf(default_folder, char_count + 1U, "%s%s", home_dir, "/.walld/images");

	config_setting_t* array;

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
static inline int file_exsits(const char* file) {
	FILE* fp;

	if ((fp = fopen(file, "r"))) {
		fclose(fp);
		return 0;
	}
	else {
		return 1;
	}
}

static inline int folder_exsits(const char* folder) {
	DIR* dir;

	dir = opendir(folder);

	if (dir) {
		closedir(dir);
		return 0;
	}
	else {
		return 1;
	}
}

static inline void check_default_image_folder(const char* home_dir) {
	int image_folder_char_count = snprintf(NULL, 0, "%s%s", home_dir, "/.walld/images");
	if (image_folder_char_count <= 0) {
		//tough luck
		abort();
	}
	char* default_image_folder = malloc(image_folder_char_count + 1U);

	if (default_image_folder == NULL) {
		//tough luck
		abort();
	}

	snprintf(default_image_folder, image_folder_char_count + 1U, "%s%s", home_dir, "/.walld/images");

	if (folder_exsits(default_image_folder) != 0) {
		mkdir(default_image_folder, 0777);
	}

	free(default_image_folder);
}
