#include "config.h"

settings* read_config(const char* config_file) {
	settings* options = malloc(sizeof(settings));

	config_t config;
	config_setting_t* settings;
	const char* source;
	int colors;

	config_init(&config);

	if (! config_read_file(&config, config_file)) {
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

	settings = config_lookup(&config, source);

	if (settings != NULL) {
		int count = config_setting_length(settings);

		linked_node* entry_point = malloc(sizeof(linked_node));

		linked_node* current = entry_point;

		options->sources = entry_point;


		for (int i = 0; i < count; i++) {
			char* item = config_setting_get_string_elem(settings, i);

			current = add_node_to_list(current, item);
		}
	}
	else {
		options->sources = malloc(sizeof(linked_node));
		options->sources->image = strdup("/home/seth/.walld/images");
	}

	config_destroy(&config);

	return options;
}

linked_node* list_file_parse(const char* list_file) {
	linked_node* entry_point = malloc(sizeof(linked_node));

	linked_node* current = entry_point;

	config_t config;
	config_setting_t* settings;
	const char* source;

	config_init(&config);

	if (! config_read_file(&config, list_file)) {
		config_destroy(&config);
		return NULL;
	}

	if (config_lookup_string(&config, "list", &source)) {
		//NOP
	}
	else {
		source = "default";
	}

	settings = config_lookup(&config, source);

	if (settings != NULL) {
		int count = config_setting_length(settings);

		for (int i = 0; i < count; i++) {
			char* item = config_setting_get_string_elem(settings, i);

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
