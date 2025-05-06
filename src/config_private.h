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

#ifndef CONFIG_PRIVATE_H
#define CONFIG_PRIVATE_H

#include "config.h"

static const struct settings DEFAULT_CONFIG = {
    /* colors */     0,
    /* dark */       1,
    /* xrdb_use */   0,
    /* feh_path */   NULL,
    /* xrdb_path */  NULL,
    /* minutes */    30,
    /* bg_style */   NULL,
    /* x_auth */     NULL,
    /* display */    NULL,
    /* monitors */   1,
    /* sources */    NULL,
    /* xrdb_argv */  NULL
};

static inline int file_exsits(const char *file);
static inline int folder_exsits(const char *folder);
static inline void check_image_folder(const char *image_folder);
static void default_settings(const struct xdg_directories *restrict xdg_dir, struct settings *options);

#endif /* CONFIG_PRIVATE_H */