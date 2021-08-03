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
