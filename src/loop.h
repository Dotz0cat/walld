#ifndef __LOOP_H__
#define __LOOP_H__

#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <errno.h>

#include "config.h"
#include "list.h"

typedef struct _pre_init_stuff pre_init_stuff;

struct _pre_init_stuff {

	//store the home dir becuase once forked it will loose it
	char* home_dir;

	char* x_auth;

	char* display;

	char* config;

	settings* options;

	linked_node* picture_list;
};

typedef struct _loop_context loop_context;

struct _loop_context {
	pre_init_stuff* info;

	sigset_t sigs;
};

int event_loop_run(loop_context* context);
static inline void feh_exec(const char* path, const char* bg_style, const char* image, char** env);
char** prep_enviroment(const char* display, const char* x_auth, const char* home);
pre_init_stuff* regen_config(pre_init_stuff* info);
void free_env(char** env);

#endif /*__LOOP_H__*/