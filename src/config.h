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

#endif /*__CONFIG_H__*/
