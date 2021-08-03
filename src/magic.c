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
