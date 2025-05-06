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

#ifndef MAIN_PRIVATE_H
#define MAIN_PRIVATE_H

#include "main.h"

static void init_daemon(void);
static struct pre_init_stuff *pre_init(char *config, int time, char *source, int monitors);
static inline void set_xdg_directory(char **target, const char *env_var, const char *string0, const char *string2);
static inline void set_xdg_directories(struct xdg_directories *restrict xdg_dir);

#define STRCONCAT(s1, s2, s3) \
do { \
    size_t char_count = snprintf(NULL, 0U, "%s%s", s1, s2); \
\
    if (char_count <= 0) abort(); \
\
    s3 = malloc(char_count + 1U); \
\
    if (s3 == NULL) abort(); \
\
    snprintf(s3, char_count + 1U, "%s%s", s1, s2); \
\
} while(0);

#endif /* MAIN_PRIVATE_H */