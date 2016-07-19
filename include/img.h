#include <png.h>

#define IMG_SUCCESS 0
#define IMG_FAIL    1

/**
 * Loads a PNG file into memory. Overwrites previously read PNG if
 * successfully called more than once.
 *
 * @param fn Path to PNG file to read.
 * @return IMG_SUCCESS on success and IMG_FAIL on error
 */
int img_read(const char *fn);

/**
 * Checks if a PNG is loaded.
 *
 * @return 1 if PNG loaded, 0 if not
 */
int img_is_loaded(void);

/**
 * Gets the rows of pixel data gotten procured the loaded PNG.
 * Editting these values directly edits the loaded PNG's pixels.
 * Such changes will be reflected in img_write().
 *
 * @param width User-supplied pointer to store width in.
 * @param height User-supplied pointer to store height in.
 * @return Pointer to rows of pixel data on success, NULL on failure.
 */
png_bytep *img_get_rows(int *width, int *height);

/**
 * Replaces the local images rows with the supplied new rows.
 *
 * @param new_rows Pointer to new rows array.
 * @param new_w Width of new image.
 * @param new_h Height of new image.
 * @return Nothing.
 */
void img_replace_rows(png_bytep *new_rows, int new_w, int new_h);

/**
 * Writes PNG in memory to disk.
 *
 * @param fn Path to new PNG file.
 * @return IMG_SUCCESS on success and IMG_FAIL on error
 */
int img_write(const char *fn);

/**
 * Cleanup image data.
 */
void img_cleanup(void);
