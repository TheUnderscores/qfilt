#include <math.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filter.h"

static inline int min(int x, int y)
{
	return (x < y) ? x : y;
}

static inline int max(int x, int y)
{
	return (x > y) ? x : y;
}

static int clamp(int x, int l, int h)
{
	return min(h, max(x, l));
}

/**
 * Wrap x such that `low <= x < high`.
 * Examples:
 * filter_wrap_int(-1, 0, 4) -> 3
 * filter_wrap_int(4, 0, 4) -> 0
 * filter_wrap_int(2, 0, 4) -> 2
 *
 * @param x Integer to wrap.
 * @param low Lowest number of range, inclusive.
 * @param high Highest number of range, exclusive.
 * @return x, wrapped so that `low <= x < high`.
 */
static int wrap_int(int x, int low, int high)
{
	int range = high - low;
	x = (x - low) % range;
	return x + ((x < 0) ? high : low);
}

static void filter_apply_at_pixel(double *filter_tmp, struct Filter filter,
				  png_bytep *img, int w, int h, int x, int y)
{
	int i = 0;
	int range = filter.size / 2;
	int dy, dx, new_y, new_x;

	for (dy = -range; dy <= range; dy++) {
		new_y = wrap_int(y + dy, 0, h);
		for (dx = -range; dx <= range; dx++, i++) {
			new_x = wrap_int(x + dx, 0, w);
			filter_tmp[i * 3] = img[new_y][new_x * 3]
				* filter.array[i];
			filter_tmp[(i * 3) + 1] = img[new_y][(new_x * 3) + 1]
				* filter.array[i];
			filter_tmp[(i * 3) + 2] = img[new_y][(new_x * 3) + 2]
				* filter.array[i];
		}
	}
}

/**
 * A guassian distribution function.
 *
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @param stdev Standard deviation.
 */
static double gauss(double x, double y, double stdev) {
	double frac = 1 / (2 * M_PI * stdev * stdev);
	double expon = -(x * x + y * y) / (2 * stdev * stdev);
	return frac * exp(expon);
}

struct Filter filter_create(int size, double *matrix) {
	struct Filter filter;
	/* ensure `size` if an odd number */
	size = (size / 2) * 2 + 1; // round down
	filter.array = calloc(size * size, sizeof(double));
	if (!filter.array) {
		filter.size = 0;
		return filter;
	} else {
		filter.size = (int)size;
	}
	memcpy(filter.array, matrix, size * size * sizeof(double));
	return filter;
}

void filter_delete(struct Filter *filter) {
	free(filter->array);
}

void filter_mult(struct Filter *filter, double mult) {
	int i;
	for (i = 0; i < filter->size * filter->size; i++)
		filter->array[i] *= mult;
}

void filter_norm(struct Filter *filter) {
	int i;
	double sum = 0;
	for (i = 0; i < filter->size * filter->size; i++)
		sum += filter->array[i];
	filter_mult(filter, 1.0 / sum);
}

struct Filter filter_box_blur(int size) {
	struct Filter filter;
	double *mat;
        int i;
	mat = malloc(size * size * sizeof(double));
	if (!mat) {
		filter.size = 0;
	} else {
		for (i = 0; i < size * size; i++) mat[i] = 1;
		filter = filter_create(size, mat);
		free(mat);
		filter_mult(&filter, 1.0 / (size * size));
	}
	return filter;
}

struct Filter filter_gauss_blur(int radius) {
	struct Filter filter;
	double *mat;
	double stdev;
	int size, x, y;
	size = radius * 2 + 1;
	mat = malloc(size * size * sizeof(double));
	if (!mat) {
		filter.size = 0;
	} else {
		stdev = sqrt(-(radius * radius) / (2 * log(1.0 / 255.0)));
		for (x = 0; x < size; x++)
			for (y = 0; y < size; y++)
				mat[x + y * size] = gauss((double)(x - radius),
							  (double)(y - radius),
							  stdev);
		filter = filter_create(size, mat);
		free(mat);
		filter_norm(&filter);
	}
	return filter;
}

png_bytep *filter_apply(struct Filter filter, png_bytep *img, int w, int h)
{
	png_bytep *filtered_img;

	filtered_img = calloc(h, sizeof(*filtered_img));

	if (!filtered_img) return NULL;

	int i;

	for (i = 0; i < h; i++) {
		filtered_img[i] = malloc(3 * w * sizeof(**filtered_img));
		if (!filtered_img[i]) return NULL;
		memcpy(filtered_img[i], img[i], 3 * w * sizeof(**filtered_img));
	}

	double *filter_tmp;

	filter_tmp = calloc(filter.size * filter.size * 3, sizeof(*filter_tmp));
	if (!filter_tmp) return NULL;

	int y, x;

	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++) {
			filter_apply_at_pixel(filter_tmp, filter, img, w, h, x, y);

			double new_r = 0;
			double new_g = 0;
			double new_b = 0;

			for (i = 0; i < filter.size * filter.size; i++) {
				new_r += filter_tmp[i * 3];
				new_g += filter_tmp[(i * 3) + 1];
				new_b += filter_tmp[(i * 3) + 2];
			}

			filtered_img[y][x * 3] = clamp(new_r, 0, 255);
			filtered_img[y][(x * 3) + 1] = clamp(new_g, 0, 255);
			filtered_img[y][(x * 3) + 2] = clamp(new_b, 0, 255);
		}

	free(filter_tmp);

	return filtered_img;
}
