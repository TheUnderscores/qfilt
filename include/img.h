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
