/*
Copyright 2021-2022, 2025 Dotz0cat

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

#ifndef MAGIC_H
#define MAGIC_H

enum file_type {
	DIRECTORY,
	IMAGE,
	LIST,
	ERROR
};

enum file_type image_or_text(const char *path);
void magick_start(const char *argv1);
void magick_stop(void);
void magick_threads(const int threads);
char **get_colors(const char *path, int num_of_colors, size_t *actual_colors);
void put_colors_in_file(const char *home_dir, const char *image, int dark);

#endif /* MAGIC_H */
