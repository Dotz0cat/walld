/*
Copyright 2021 Dotz0cat

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

#ifndef __LOOP_H__
#define __LOOP_H__

#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <errno.h>
#include <syslog.h>

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