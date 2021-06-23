#include "config.h"

settings* read_config(const char* config_file, const char* home_dir) {
	settings* options = malloc(sizeof(settings));

	config_t config;
	config_setting_t* setting;
	const char* source;
	int colors;

	config_init(&config);

	if (config_read_file(&config, config_file) == CONFIG_TRUE) {
		config_destroy(&config);
		return NULL;
	}

	if (config_lookup_bool(&config, "colors", &colors)) {
		options->colors = colors;
	}
	else {
		options->colors = 0;
	}

	if (config_lookup_string(&config, "source_to_use", &source)) {
		//NOP
	}
	else {
		source = "default";
	}

	setting = config_lookup(&config, source);

	if (setting != NULL) {
		int count = config_setting_length(setting);

		linked_node* entry_point = malloc(sizeof(linked_node));

		linked_node* current = entry_point;

		options->sources = entry_point;


		for (int i = 0; i < count; i++) {
			char* item = config_setting_get_string_elem(setting, i);

			current = add_node_to_list(current, item);
		}
	}
	else {
		options->sources = malloc(sizeof(linked_node));
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

		options->sources->image = default_folder;
	}

	config_destroy(&config);

	return options;
}

linked_node* list_file_parse(const char* list_file) {
	linked_node* entry_point = malloc(sizeof(linked_node));

	linked_node* current = entry_point;

	config_t config;
	config_setting_t* setting;
	const char* source;

	config_init(&config);

	if (config_read_file(&config, list_file) == CONFIG_TRUE) {
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
			char* item = config_setting_get_string_elem(setting, i);

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
