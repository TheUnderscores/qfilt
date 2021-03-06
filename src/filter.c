#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>

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
