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

#include "magic.h"

file_type image_or_text(const char* path) {
	Image* image;

	ImageInfo* info;

	ExceptionInfo* exception;

	exception = AcquireExceptionInfo();

	info = CloneImageInfo((ImageInfo* ) NULL);

	strcpy(info->filename, path);

	image = PingImage(info, exception);

	if (exception->severity != UndefinedException) {
		CatchException(exception);
	}
    
    if (image == (Image*) NULL) {
    	//cleanup
    	info = DestroyImageInfo(info);
    	exception = DestroyExceptionInfo(exception);
    	return LIST;
    }

    DestroyImage(image);
    info = DestroyImageInfo(info);
    exception = DestroyExceptionInfo(exception);
    return IMAGE;
}

void magick_start(const char* argv1) {
	MagickCoreGenesis(argv1, MagickFalse);
}

void magick_stop(void) {
	//RelinquishMagickResource(ThreadResource, 1);
	MagickCoreTerminus();
}

void magick_threads(const int threads) {
	SetMagickResourceLimit(ThreadResource, threads);
}

char** get_colors(const char* path, int num_of_colors, size_t* actual_colors) {
	Image* image;

	ImageInfo* info;

	ExceptionInfo* exception;

	exception = AcquireExceptionInfo();

	info = CloneImageInfo((ImageInfo* ) NULL);

	strcpy(info->filename, path);

	image = ReadImage(info, exception);

	if (exception->severity != UndefinedException) {
		CatchException(exception);
	}
    
    if (image == (Image*) NULL) {
    	return NULL;
    }

    SyncImageSettings(info, image, exception);

    Image* fifty;

    fifty = MinifyImage(image, exception);

    SyncImageSettings(info, fifty, exception);

    if (exception->severity != UndefinedException) {
		CatchException(exception);
	}

    Image* quarter;

    quarter = MinifyImage(fifty, exception);

    SyncImageSettings(info, quarter, exception);

    if (exception->severity != UndefinedException) {
		CatchException(exception);
	}

    QuantizeInfo* quant;

    quant = AcquireQuantizeInfo(info);

    quant->number_colors = num_of_colors;

    QuantizeImage(quant, quarter, exception);

    SyncImageSettings(info, quarter, exception);

    if (exception->severity != UndefinedException) {
		CatchException(exception);
	}

    Image* unique;

    unique = UniqueImageColors(quarter, exception);

    //SyncImageSettings(unique, fifty, exception);

    if (exception->severity != UndefinedException) {
		CatchException(exception);
	}

	size_t color_num = GetNumberColors(unique, NULL, exception);

	char** color_array = malloc(color_num * sizeof(char*));

	PixelInfo* histo;

	histo = GetImageHistogram(unique, &color_num, exception);

	PixelInfo* p;
	PixelInfo pixel;

	p = histo;

	GetPixelInfo(unique, &pixel);

	for (int i = 0; i < (int) color_num; i++) {
		pixel = (*p);

		//get memory
		char* hex = malloc(4096);

		GetColorTuple(&pixel, MagickTrue, hex);

		color_array[i] = strdup(hex);

		free(hex);

		p++;
	}

	*actual_colors = color_num;

	histo = (PixelInfo* ) RelinquishMagickMemory(histo);

	DestroyQuantizeInfo(quant);
    DestroyImage(image);
    DestroyImage(unique);
    DestroyImage(fifty);
    info = DestroyImageInfo(info);
    exception = DestroyExceptionInfo(exception);

    return color_array;
}

//dark is 0 light is anything not 0
void put_colors_in_file(const char* home_dir, const char* image, int dark) {
	int char_count = snprintf(NULL, 0, "%s%s", home_dir, "/.walld/colors");
	if (char_count <= 0) {
		//tough luck
		abort();
	}
	char* color_file = malloc(char_count + 1U);

	if (color_file == NULL) {
		//tough luck
		abort();
	}

	snprintf(color_file, char_count + 1U, "%s%s", home_dir, "/.walld/colors");

	int i = 16;

	char** list = NULL;

	size_t len;

	do {
		if (list != NULL) {
			for (int j = 0; j < (int) len; j++) {
				free(list[j]);
			}
			free(list);
		}
		list = get_colors(image, i, &len);
		i++;
	} while (len != 16);

	//adjustments
	//hsl

	char** list2 = malloc(sizeof(char*) * len);

	if (dark == 0) {
		for (int k = 0; k < (int) len; k++) {
			list2[k] = strdup(list[k]);
		}

		rgb* rgb1 = hex_to_rgb(list2[7]);
		rgb* rgb2 = hex_to_rgb("#eeeeee");

		rgb* rgb3 = blend_colors(rgb1, rgb2);

		free(list2[7]);
		list2[7] = rgb_to_hex(rgb3);

		free(rgb1);
		free(rgb2);
		free(rgb3);

		rgb* rgb4 = hex_to_rgb(list2[7]);

		darken_rgb(rgb4, 0.30);

		free(list2[8]);
		list2[8] = rgb_to_hex(rgb4);

		free(rgb4);

		rgb* rgb5 = hex_to_rgb(list2[15]);
		rgb* rgb6 = hex_to_rgb("#eeeeee");

		rgb* rgb7 = blend_colors(rgb5, rgb6);

		free(list2[15]);
		list2[15] = rgb_to_hex(rgb7);

		free(rgb5);
		free(rgb6);
		free(rgb7);

	}
	else {
		for (int k = 0; k < (int) len; k++) {
			rgb* rgb1 = hex_to_rgb(list[k]);

			hsl* hsl1 = rgb_to_hsl(rgb1);

			hsl1->s = 0.5;

			rgb* rgb2 = hsl_to_rgb(hsl1);

			char* hex = rgb_to_hex(rgb2);

			list2[k] = hex;

			free(rgb1);
			free(hsl1);
			free(rgb2);
		} 

		rgb* rgb1 = hex_to_rgb(list[0]);
		free(list2[0]);

		lighten_rgb(rgb1, 0.85);

		list2[0] = rgb_to_hex(rgb1);
		free(rgb1);

		free(list2[7]);
		list2[7] = strdup(list[0]);

		rgb* rgb2 = hex_to_rgb(list[0]);
		free(list2[8]);

		darken_rgb(rgb2, 0.4);

		list2[8] = rgb_to_hex(rgb2);
		free(rgb2);

		free(list2[15]);
		list2[15] = strdup(list[0]);

	}
	
	FILE* fp = fopen(color_file, "w");

	for (int j = 0; j < (int) len; j++) {
		fprintf(fp, "color%i:\t%s\n", j, list2[j]);
		free(list2[j]);
		free(list[j]);
	}

	fclose(fp);

	free(list2);
	free(list);
	free(color_file);
}

rgb* hex_to_rgb(const char* hex_string) {
	rgb* hex_as_rgb = malloc(sizeof(rgb));

	sscanf(hex_string, "#%2x%2x%2x", &hex_as_rgb->r, &hex_as_rgb->g, &hex_as_rgb->b);

	return hex_as_rgb;
}

hsl* rgb_to_hsl(const rgb* color) {
	hsl* hsl_conv = malloc(sizeof(hsl));

	float r = color->r / 255.0;

	float g = color->g / 255.0;

	float b = color->b / 255.0;

	float max_c = fmax(fmax(r, g), b);

	float min_c = fmin(fmin(r, g), b);

	hsl_conv->l = (max_c + min_c)/2;

	hsl_conv->s = (int) (max_c - min_c)/(1 - fabsf(2 * hsl_conv->l - 1));

	if ((max_c - min_c) == 0) {
		hsl_conv->h = 0;
	}
	else if (max_c == r) {
		int h = ( (int) ((g - b)/ (max_c - min_c))) % 6;
		hsl_conv->h = h * 60;
	}
	else if (max_c == g) {
		int h = ((b - r)/ (max_c - min_c)) + 2;
		hsl_conv->h = h * 60;
	}
	else if (max_c == b) {
		int h = ((r - g)/ (max_c - min_c)) + 4;
		hsl_conv->h = h * 60;
	}

	return hsl_conv;
}

rgb* hsl_to_rgb(const hsl* color) {
	rgb* rgb_conv = malloc(sizeof(rgb));

	float c = (2 * fabsf(2 * color->l - 1)) * color->s;

	float x = c * (1 - abs(( (int) (color->h / 60.0) % 2) -1));

	float m = (color->l - (c / 2));

	float r;
	float g;
	float b;

	if (color->h >= 0 && color->h < 60) {
		r = c;
		g = x;
		b = 0;
	}
	else if (color->h >= 60 && color->h < 120) {
		r = x;
		g = c;
		b = 0;
	}
	else if (color->h >= 120 && color->h < 180) {
		r = 0;
		g = c;
		b = x;
	}
	else if (color->h >= 180 && color->h < 240) {
		r = 0;
		g = x;
		b = c;
	}
	else if (color->h >= 240 && color->h < 300) {
		r = x;
		g = 0;
		b = c;
	}
	else if (color->h >= 300 && color->h < 360) {
		r = c;
		g = 0;
		b = x;
	}
	else {
		r = 0;
		g = 0;
		b = 0;
	}

	rgb_conv->r = (r + m) * 255;
	rgb_conv->g = (g + m) * 255;
	rgb_conv->b = (b + m) * 255;

	return rgb_conv;
}

char* rgb_to_hex(const rgb* color) {
	char* hex = malloc(1 + 2 + 2 + 2 + 1);

	sprintf(hex, "#%2x%2x%2x", color->r, color->g, color->b);

	return hex;
}

void lighten_rgb(rgb* color, float factor) {
	color->r = (unsigned int) (color->r + ((255 - color->r) * factor));
	color->g = (unsigned int) (color->g + ((255 - color->g) * factor));
	color->b = (unsigned int) (color->b + ((255 - color->b) * factor));
}

void darken_rgb(rgb* color, float factor) {
	color->r = (unsigned int) (color->r * (1 - factor));
	color->g = (unsigned int) (color->g * (1 - factor));
	color->b = (unsigned int) (color->b * (1 - factor));
}

rgb* blend_colors(rgb* color1, rgb* color2) {
	rgb* rgb3 = malloc(sizeof(rgb));

	rgb3->r = (unsigned int) ((color1->r * .5) + (color2->r * .5));
	rgb3->g = (unsigned int) ((color1->g * .5) + (color2->g * .5));
	rgb3->b = (unsigned int) ((color1->b * .5) + (color2->b * .5));

	return rgb3;
}
