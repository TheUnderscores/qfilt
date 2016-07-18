#include <stdio.h>
#include <stdlib.h>
#include <png.h>

#include "img.h"

static png_structp png_p = NULL;
static png_infop info_p = NULL;

static int w, h; //width and height
static png_byte colorType, bitDep;
static png_bytep *rows_p = NULL;

static int imgLoaded = 0;

int img_read(const char *fn)
{
	/* TODO: reject image if it has an alpha channel */
	FILE *fp;
	png_byte sig[8];
	int y, ret;

	if (img_is_loaded())
		img_cleanup();

        fp = fopen(fn, "rb");
	if (!fp) {
		fprintf(stderr, "read_img: File %s could not be opened for reading\n", fn);
		ret = IMG_FAIL;
		goto img_read_cleanup;
	}

	/* confirm valid PNG signature in file header */
	if (fread(sig, 1, 8, fp) != 8) {
		fprintf(stderr, "read_img: File %s reading stopped short\n", fn);
		ret = IMG_FAIL;
		goto img_read_cleanup;
	}
	if (png_sig_cmp(sig, 0, 8)) {
		fprintf(stderr, "read_img: File %s is not recognized as a PNG file\n", fn);
		ret = IMG_FAIL;
		goto img_read_cleanup;
	}

	/* initialize libpng structs */
	png_p = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_p) {
		fprintf(stderr, "read_img: png_create_read_struct failed", fn);
		ret = IMG_FAIL;
	}
	info_p = png_create_info_struct(png_p);
	if (!info_p) {
	        fprintf(stderr, "read_img: png_create_info_struct failed\n");
		ret = IMG_FAIL;
	        goto img_read_cleanup;
	}

	/* setup jump buffer for error handling */
	if (setjmp(png_jmpbuf(png_p))) {
		fprintf(stderr, "read_img: Error setting jump buffer\n");
		ret = IMG_FAIL;
	        goto img_read_cleanup;
	}

	png_init_io(png_p, fp);
	png_set_sig_bytes(png_p, 8);
	png_read_info(png_p, info_p);

	w = png_get_image_width(png_p, info_p);
	h = png_get_image_height(png_p, info_p);
	colorType = png_get_color_type(png_p, info_p);
	bitDep = png_get_bit_depth(png_p, info_p);
	png_set_interlace_handling(png_p);
	png_read_update_info(png_p, info_p);

	/* read file */
	if (setjmp(png_jmpbuf(png_p))) {
		fprintf(stderr, "read_img: Error setting jump buffer\n");
		ret = IMG_FAIL;
	        goto img_read_cleanup;
	}
	rows_p = malloc(sizeof(png_bytep) * h);
	for (y=0; y<h; y++)
		rows_p[y] = malloc(png_get_rowbytes(png_p, info_p));
	png_read_image(png_p, rows_p);

	imgLoaded = 1;
	ret = IMG_SUCCESS;

 img_read_cleanup:
	if (ret == IMG_FAIL)
		if (png_p && info_p)
			png_destroy_read_struct(&png_p, &info_p, NULL);
	if (fp)
		fclose(fp);
	return ret;
}

int img_is_loaded(void)
{
	return imgLoaded;
}

int img_write(const char *fn)
{
	FILE *fp = fopen(fn, "wb");
	if (!img_is_loaded()) {
		fprintf(stderr, "write_img: No image is loaded\n");
		return IMG_FAIL;
	}
	fclose(fp);
	return IMG_SUCCESS;
}
		
void img_cleanup(void)
{
	png_destroy_read_struct(&png_p, &info_p, NULL);
	int y;
	for (y=0; y<h; y++)
		free(rows_p[y]);
	free(rows_p);
	imgLoaded = 0;
}
