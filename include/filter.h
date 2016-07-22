#ifndef QFILT_FILTER_H
#define QFILT_FILTER_H

#include <png.h>

struct Filter {
	int size;
	double *array;
};

/**
 * Apply a kernel filter to image data.
 *
 * @param filter The filter to use.
 * @param img The image data.
 * @param w Image width.
 * @param h Image height.
 * @return A copy of the image data with filter applied, or NULL on error.
 */
png_bytep *filter_apply(struct Filter filter, png_bytep *img, int w, int h);

#endif /* #ifndef QFILT_FILTER_H */
