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
#include <regex.h>

#include "config.h"
#include "magic.h"
#include "list.h"
#include "loop.h"

static void init_daemon(void);
static pre_init_stuff* pre_init(void);
linked_node* get_images(linked_node* source);
file_type get_file_type(const char* path);
linked_node* list_files_full(const char* directory);
char* realpath_wrap(const char* path, const char* dir);

#endif /*__MAIN_H__*/
