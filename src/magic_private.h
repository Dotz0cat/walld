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

#ifndef MAGIC_PRIVATE_H
#define MAGIC_PRIVATE_H

#include <ImageMagick-7/MagickCore/MagickCore.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "magic.h"

struct hsl {
    float h;
    float s;
    float l;
};

struct rgb {
    unsigned int r;
    unsigned int g;
    unsigned int b;
};

char **get_colors(const char *path, int num_of_colors, size_t *actual_colors);
void put_colors_in_file(const char *home_dir, const char *image, int dark);
struct rgb *hex_to_rgb(const char *hex_string);
struct hsl *rgb_to_hsl(const struct rgb *color);
struct rgb *hsl_to_rgb(const struct hsl *color);
char *rgb_to_hex(const struct rgb *color);
void lighten_rgb(struct rgb *color, float factor);
void darken_rgb(struct rgb *color, float factor);
struct rgb *blend_colors(struct rgb *color1, struct rgb *color2);
char *remove_alpha(const char *hex);
void write_default(const char *color_file);

#endif /* MAGIC_PRIVATE_H */
