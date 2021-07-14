#include "magic.h"

file_type image_or_text(const char* path) {
	Image* image;

	ImageInfo* info;

	ExceptionInfo* exception;

	MagickCoreGenesis(path, MagickFalse);

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
    	MagickCoreTerminus();
    	return LIST;
    }

    DestroyImage(image);
    info = DestroyImageInfo(info);
    exception = DestroyExceptionInfo(exception);
    MagickCoreTerminus();
    return IMAGE;
}
