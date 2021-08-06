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

	//MagickCoreGenesis(NULL, MagickFalse);

	exception = AcquireExceptionInfo();

	info = CloneImageInfo((ImageInfo* ) NULL);

	strcpy(info->filename, path);

	image = ReadImage(info, exception);

	// if (exception->severity != UndefinedException) {
	// 	CatchException(exception);
	// }
    
    if (image == (Image*) NULL) {
    	//cleanup
    	info = DestroyImageInfo(info);
    	exception = DestroyExceptionInfo(exception);
    	//MagickCoreTerminus();
    	return LIST;
    }

    DestroyImage(image);
    info = DestroyImageInfo(info);
    exception = DestroyExceptionInfo(exception);
    //MagickCoreTerminus();
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
