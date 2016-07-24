#ifndef QFILT_FILTER_H
#define QFILT_FILTER_H

#include <png.h>

struct Filter {
	int size;
	double *array;
};

/**
 * Creates a filter to pass to `filter_apply()`.
 *
 * @param size Number of rows and columns (individually, not cumulatively) in
 *    the filter's matrix. Must be an odd number, greater than or equal to 1.
 * @param matrix Array of matrix values. Every increment of length `size`
 *    is a row.
 * @return Complete filter on success, a filter with size 0 on failure.
 */
struct Filter filter_create(int size, double *matrix);

/**
 * Deletes a filter created by `filter_create()`.
 *
 * @param filter Filter to delete.
 */
void filter_delete(struct Filter *filter);

/**
 * Multiplies all the values in a matrix by a value.
 *
 * @param mult The multiplier to apply.
 */
void filter_mult(struct Filter *filter, double mult);

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
