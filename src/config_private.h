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

static inline int file_exsits(const char *file);
static inline int folder_exsits(const char *folder);
static inline void check_default_image_folder(const char *home_dir);

#endif /* CONFIG_PRIVATE_H */