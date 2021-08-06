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

#ifndef __MAGIC_H__
#define __MAGIC_H__

#include <ImageMagick-7/MagickCore/MagickCore.h>
#include <stdlib.h>
#include <string.h>

typedef enum _file_type file_type;

enum _file_type {
	DIRECTORY,
	IMAGE,
	LIST,
	ERROR
};

file_type image_or_text(const char* path);
void magick_start(const char* argv1);
void magick_stop(void);
void magick_threads(const int threads);

#endif /* __MAGIC_H__*/
