#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdlib.h>
#include <string.h>
#include <libconfig.h>

#include "list.h"

typedef struct _settings settings;

struct _settings {
	int colors;

	linked_node* sources;
};

settings* read_config(const char* config_file, const char* home_dir);
linked_node* list_file_parse(const char* list_file);

#endif /*__CONFIG_H__*/
