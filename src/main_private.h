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

#endif /* MAIN_PRIVATE_H */