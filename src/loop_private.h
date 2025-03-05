/*
Copyright 2025 Dotz0cat

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

#ifndef LOOP_PRIVATE_H
#define LOOP_PRIVATE_H

#include "loop.h"

static inline void feh_exec(const char *path, char **feh_argv, char **env);
static inline void write_color_file(const char *home_dir, const char *image, int dark);
static inline void xrdb_exec(const char *path, char** xrdb_argv);

//callbacks
static void sig_int_quit_term_cb(evutil_socket_t sig, short events, void *user_data);
static void sighup_cb(evutil_socket_t sig, short events, void *user_data);
static void sigusr1_cb(evutil_socket_t sig, short events, void *user_data);
static void sigusr2_cb(evutil_socket_t sig, short events, void *user_data);
static void timer_expire_cb(evutil_socket_t fd, short events, void *user_data);

#endif /* LOOP_PRIVATE_H */
