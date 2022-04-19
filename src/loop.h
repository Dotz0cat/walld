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

#ifndef LOOP_H
#define LOOP_H

#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <errno.h>
#include <syslog.h>

//new stuff
#include <event2/event.h>

#include "config.h"
#include "list.h"

typedef struct _pre_init_stuff pre_init_stuff;

struct _pre_init_stuff {

	//store the home dir becuase once forked it will loose it
	char* home_dir;

	//this is from the command line
	char* source;

	char* x_auth;

	char* display;

	char* config;

	settings* options;

	//only used when the time is passed on command line
	int time;

	//only used when passed on comand line
	int monitors;

	linked_node* picture_list;
};

typedef struct _events_box events_box;

struct _events_box {
	struct event_base* base;
	struct event* signal_sigquit;
	struct event* signal_sigterm;
	struct event* signal_sigint;
	struct event* signal_sighup;
	struct event* signal_sigusr1;
	struct event* signal_sigusr2;
	struct event* timer;
};

typedef struct _loop_context loop_context;

struct _loop_context {
	pre_init_stuff* info;

	linked_node* current;

	char** feh_argv;

	size_t feh_len;

	char** env;

	size_t env_len;

	char** xrdb_argv;

	size_t xrdb_len;

	struct timeval* seconds;

	events_box* event_box;
};

int event_loop_run(loop_context* context);
static inline void feh_exec(const char* path, char** feh_argv, char** env);
static inline void write_color_file(const char* home_dir, const char* image, int dark);
static inline void xrdb_exec(const char* path, char** xrdb_argv);
char** prep_enviroment(const char* display, const char* x_auth, const char* home, size_t* size);
pre_init_stuff* regen_config(pre_init_stuff* info);
void free_env(char** env, size_t env_len);
char** prep_xrdb_argv(linked_node* node, size_t* xrdb_len);
char** prep_feh_argv(const char* bg_style, linked_node* node, int monitors, size_t* feh_len);
void edit_feh_argv(char** feh_argv, linked_node* node, size_t feh_len);
void free_feh_argv(char** feh_argv);

//callbacks
static void sig_int_quit_term_cb(evutil_socket_t sig, short events, void* user_data);
static void sighup_cb(evutil_socket_t sig, short events, void* user_data);
static void sigusr1_cb(evutil_socket_t sig, short events, void* user_data);
static void sigusr2_cb(evutil_socket_t sig, short events, void* user_data);
static void timer_expire_cb(evutil_socket_t fd, short events, void* user_data);

#endif /* LOOP_H */
