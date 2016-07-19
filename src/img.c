#include <stdio.h>
#include <stdlib.h>

#include "img.h"

static png_structp png_p = NULL; // loaded PNG struct
static png_infop info_p = NULL; // loaded PNG info struct
static int w, h; // width and height
static png_bytep *rows_p = NULL; // pixel data

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
		fprintf(stderr, "img_read: File %s could not be opened for reading\n", fn);
		ret = IMG_FAIL;
		goto img_read_cleanup;
	}

	/* confirm valid PNG signature in file header */
	if (fread(sig, 1, 8, fp) != 8) {
		fprintf(stderr, "img_read: File %s reading stopped short\n", fn);
		ret = IMG_FAIL;
		goto img_read_cleanup;
	}
	if (png_sig_cmp(sig, 0, 8)) {
		fprintf(stderr, "img_read: File %s is not recognized as a PNG file\n", fn);
		ret = IMG_FAIL;
		goto img_read_cleanup;
	}

	/* initialize libpng structs */
	png_p = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_p) {
		fprintf(stderr, "img_read: png_create_read_struct failed", fn);
		ret = IMG_FAIL;
	}
	info_p = png_create_info_struct(png_p);
	if (!info_p) {
	        fprintf(stderr, "img_read: png_create_info_struct failed\n");
		ret = IMG_FAIL;
	        goto img_read_cleanup;
	}

	/* setup jump buffer for error handling */
	if (setjmp(png_jmpbuf(png_p))) {
		fprintf(stderr, "img_read: Error setting jump buffer\n");
		ret = IMG_FAIL;
	        goto img_read_cleanup;
	}

	png_init_io(png_p, fp);
	png_set_sig_bytes(png_p, 8);
	png_read_info(png_p, info_p);

	w = png_get_image_width(png_p, info_p);
	h = png_get_image_height(png_p, info_p);
	png_set_interlace_handling(png_p);
	png_read_update_info(png_p, info_p);

	/* read file */
	if (setjmp(png_jmpbuf(png_p))) {
		fprintf(stderr, "img_read: Error setting jump buffer\n");
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
	if (ret == IMG_FAIL && png_p)
		png_destroy_read_struct(&png_p, &info_p, NULL);
	if (fp)
		fclose(fp);
	return ret;
}

int img_is_loaded(void)
{
	return imgLoaded;
}

png_bytep *img_get_rows(void)
{
	if (!img_is_loaded()) {
		fprintf(stderr, "img_get_rows: No image is loaded\n");
		return NULL;
	}
	return rows_p;
}

int img_write(const char *fn)
{
	FILE *fp;
	int ret;
	png_structp wPng_p;
	png_infop wInfo_p;
	png_byte colorType, bitDep;

	if (!img_is_loaded()) {
		fprintf(stderr, "write_img: No image is loaded\n");
		return IMG_FAIL;
	}

	fp = fopen(fn, "wb");
	if (!fp) {
		fprintf(stderr, "img_write: File %s could not be opened for reading\n", fn);
		ret = IMG_FAIL;
		goto img_write_cleanup;
	}

	/* initialize libpng structs */
        wPng_p = png_create_write_struct(PNG_LIBPNG_VER_STRING,
					 NULL, NULL, NULL);
	if (!wPng_p) {
		fprintf(stderr, "img_write: png_create_write_struct failed\n", fn);
		ret = IMG_FAIL;
		goto img_write_cleanup;
	}
	wInfo_p = png_create_info_struct(wPng_p);
	if (!wInfo_p) {
		fprintf(stderr, "img_write: png_create_info_struct failed\n", fn);
		ret = IMG_FAIL;
		goto img_write_cleanup;
	}
	if (setjmp(png_jmpbuf(wPng_p))) {
		fprintf(stderr, "img_read: Error setting jump buffer while initializing IO\n");
		ret = IMG_FAIL;
	        goto img_write_cleanup;
	}
	png_init_io(wPng_p, fp);

	/* write PNG header */
	if (setjmp(png_jmpbuf(wPng_p))) {
		fprintf(stderr, "img_read: Error setting jump buffer while writing header\n");
		ret = IMG_FAIL;
	        goto img_write_cleanup;
	}

	colorType = png_get_color_type(png_p, info_p);
	bitDep = png_get_bit_depth(png_p, info_p);
	png_set_IHDR(wPng_p,
		     wInfo_p,
		     w,
		     h,
		     bitDep,
		     colorType,
		     PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE,
		     PNG_FILTER_TYPE_BASE);
	png_write_info(wPng_p, wInfo_p);

	/* write loaded PNG bytes from rows_p */
	if (setjmp(png_jmpbuf(wPng_p))) {
		fprintf(stderr, "img_read: Error setting jump buffer while writing bytes\n");
		ret = IMG_FAIL;
		goto img_write_cleanup;
	}
	png_write_image(wPng_p, rows_p);

	if (setjmp(png_jmpbuf(wPng_p))) {
		fprintf(stderr, "img_read: Error setting jump buffer during end of write\n");
		ret = IMG_FAIL;
		goto img_write_cleanup;
	}
	png_write_end(wPng_p, NULL);
	ret = IMG_SUCCESS;

 img_write_cleanup:
	if (ret == IMG_FAIL && wPng_p)
		png_destroy_write_struct(&wPng_p, &wInfo_p);
	fclose(fp);
	return ret;
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
