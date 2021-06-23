#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <syslog.h>
#include <pwd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "config.h"
#include "magic.h"
#include "list.h"

typedef struct _pre_init_stuff pre_init_stuff;

struct _pre_init_stuff {

	//store the home dir becuase once forked it will loose it
	char* home_dir;

	char* config;

	settings* options;

	linked_node* picture_list;
};

static void init_daemon(void);
static pre_init_stuff* pre_init(void);
linked_node* get_images(linked_node* source);
file_type get_file_type(const char* path);
linked_node* list_files_full(const char* directory);
char* realpath_wrap(const char* path);

#endif /*__MAIN_H__*/
