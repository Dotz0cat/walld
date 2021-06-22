#include "magic.h"

file_type image_or_text(const char* path) {
	Image* image;

	ImageInfo* info;

	ExceptionInfo* exception;

	MagickCoreGensis(path, MAGICK_FALSE);

	exception = AcqureExceptionInfo();

	info = CloneImageInfo((ImageInfo* ) NULL);

	strcpy(info->filename, path);

	image = ReadImage(info, exception);

	if (exception->severity != UndefinedException) {
		CatchException(exception);
	}
    
    if (image == (image* ) NULL) {
    	//cleanup
    	return LIST;
    }

    DestroyImage(image);
    info = DestroyImageInfo(info);
    exception = DestroyExceptionInfo(exception);
    MagickCoreTerminus();
    return IMAGE;
}
