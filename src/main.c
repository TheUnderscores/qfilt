#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <png.h>

#include "main.h"
#include "img.h"
#include "filter.h"

/* TODO:
 *   - create function for passing errors
 *   - take parameters for some filters (e.g. radius for gaussian blur)
 *   - allow for stacking of filters (applying multiple filters)
 *   - use function to determine values of gaussian blur
 *   - safely exit qfilt if a filter is not successfully selected; do not write to output
 *   - restrict parameter values
 *   - using threading to increase performance
 */

int filtSelected = 0;

double mat_gauss[] = {
	1.0/16, 2.0/16, 1.0/16,
	2.0/16, 4.0/16, 2.0/16,
	1.0/16, 2.0/16, 1.0/16
};
double mat_sharp[] = {
	 0, -1,  0,
	-1,  5, -1,
	 0, -1,  0,
};
double mat_unsharp[] = {
	-1.0/256, -4.0/256,  -6.0/256,   -4.0/256,  -1.0/256,
	-4.0/256, -16.0/256, -24.0/256,  -16.0/256, -4.0/256,
	-6.0/256, -24.0/256,  476.0/256, -24.0/256, -6.0/256,
	-4.0/256, -16.0/256, -24.0/256,  -16.0/256, -4.0/256,
	-1.0/256, -4.0/256,  -6.0/256,   -4.0/256,  -1.0/256
};
double mat_outline[] = {
	-1, -1, -1,
	-1,  8, -1,
	-1, -1, -1
};
double mat_emboss[] = {
	-2, -1,  0,
	-1,  1,  1,
	 0,  1,  2
};
double mat_topSob[] = {
	 1,  2,  1,
	 0,  0,  0,
	-1, -2, -1
};
double mat_btmSob[] = {
	-1, -2, -1,
	 0,  0,  0,
	 1,  2,  1
};
double mat_rtSob[] = {
	-1, 0, 1,
	-2, 0, 2,
	-1, 0, 1
};
double mat_ltSob[] = {
	1, 0, -1,
	2, 0, -2,
	1, 0, -1
};

void selectFilter(struct Filter *filt, unsigned int size, double mat[], const char *filtName) {
	struct Filter newFilt;
	if (filtSelected)
		printf("Cannot select %s; filter stacking is not yet supported\n", filtName);
	else {
		newFilt = filter_create(size, mat);
		if (newFilt.size == 0)
			printf("Failed to select %s\n", filtName);
		else {
			if (!filt) filter_delete(filt);
			*filt = newFilt;
			printf("Selected %s\n", filtName);
			filtSelected = 1;
		}
	}
}

int main(int argc, char *argv[])
{
	const char *input_file = NULL;
	const char *output_file = NULL;
	char *end_p;
	int c;
        int filtRadius;
	int long_option_index = 0;
	struct Filter filter;
	static struct option long_opts[] = {
		{"help", no_argument, NULL, 'h'},
		{"input", required_argument, NULL, 'i'},
		{"output", required_argument, NULL, 'o'},
		{"blur", optional_argument, NULL, 'b'},
		{"gaussian", no_argument, NULL, 'g'},
		{"sharpen", no_argument, NULL, 's'},
		{"unsharpen", no_argument, NULL, 'u'},
		{"outline", no_argument, NULL, 'l'},
		{"emboss", no_argument, NULL, 'e'},
		{"topsobel", no_argument, NULL, 1},
		{"bottomsobel", no_argument, NULL, 2},
		{"rightsobel", no_argument, NULL, 3},
		{"leftsobel", no_argument, NULL, 4},
		{"custom", required_argument, NULL, 'c'},
		{NULL, 0, NULL, 0}
	};

	while (1) {
		c = getopt_long(argc, argv, "hi:o:bgsulec:",
			long_opts, &long_option_index);

		if (c == -1) break;

		switch (c) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 'i':
			input_file = optarg;
			printf("Set input file to '%s'\n", input_file);
			break;
		case 'o':
			output_file = optarg;
			printf("Set output file to '%s'\n", output_file);
			break;
		case 'b':
			if (filtSelected)
				printf("Cannot select box blur; filter stacking is not yet supported\n");
			else {
				if (optarg) {
					filtRadius = strtol(optarg, &end_p, 10);
					if (*end_p != '\0' || filtRadius <= 0) {
						printf("Radius invalid: %s\n", optarg);
						break;
					}
				} else
					filtRadius = 1;
				filter = filter_boxblur((unsigned int)filtRadius * 2 + 1);
				filtSelected = 1;
				printf("Selected box blur with radius %d\n", filtRadius);
			}
			break;
		case 'g':
			selectFilter(&filter, size_gauss, mat_gauss, "gaussian");
			break;
		case 's':
			selectFilter(&filter, size_sharp, mat_sharp, "sharpen");
			break;
		case 'u':
			selectFilter(&filter, size_unsharp, mat_unsharp, "unsharpen");
			break;
		case 'l':
			selectFilter(&filter, size_outline, mat_outline, "outline");
			break;
		case 'e':
			selectFilter(&filter, size_emboss, mat_emboss, "emboss");
			break;
		case 1:
			selectFilter(&filter, size_topSob, mat_topSob, "top sobel");
			break;
		case 2:
			selectFilter(&filter, size_btmSob, mat_btmSob, "bottom sobel");
			break;
		case 3:
			selectFilter(&filter, size_rtSob, mat_rtSob, "right sobel");
			break;
		case 4:
			selectFilter(&filter, size_ltSob, mat_ltSob, "ltSob");
			break;
		case 'c':
			if (filtSelected)
				printf("Cannot select custom; filter stacking is not yet supported\n");
			else {
				printf("Selected custom '%s'\n", optarg);
				printf("WARNING: custom filter loading not supported yet!\n");
				filtSelected = 1;
			}
			break;
		case '?':
			/*
			 * getopt_long sees an invalid flag or missing required
			 * argument. Since getopt_long has told the user of the
			 * issue, all we do now is bail out.
			 */
			return -1;
		}
	}

	/*
	 * Do we have both input and output files? If not, check if we have
	 * enough extra arguments to fill the slots. We'll just implicitly
	 * assume them to be input and output file in that order.
	 */
	if ((!input_file && !output_file) && argc >= optind + 2) {
		input_file = argv[optind];
		output_file = argv[optind + 1];
	} else if (!input_file || !output_file) {
		fprintf(stderr, "%s: missing file arguments\n", argv[0]);
		return -1;
	}

	if (img_read(input_file) == IMG_FAIL) {
		fprintf(stderr, "%s: main: Failed to load image\n", argv[0]);
		return -1;
	}

	int img_w, img_h;
	png_bytep *src_rows = img_get_rows(&img_w, &img_h);

	if (!src_rows) return -1;

	png_bytep *new_rows = filter_apply(filter, src_rows, img_w, img_h);

	if (!new_rows) {
		perror("filter_apply: couldn't apply filter");
		return -1;
	}

	img_replace_rows(new_rows, img_w, img_h);

	if (img_write(output_file) == IMG_FAIL) {
		fprintf(stderr, "%s: main: Failed to write image\n", argv[0]);
		return -1;
	}

        filter_delete(&filter);
	img_cleanup();
	printf("Filter successfully applied\n");
	return 0;
}

void usage(char *utilname)
{
	printf("Usage: %s <input_file> <output_file> [options]\n", utilname);
	printf("WARNING: UTILITY IS NOT FULLY FUNCTIONAL YET\n");
	printf("Options:\n");
	/* TODO: add optional arguments for each filter option */
	printf("  -h --help             Display this help\n");
	printf("  -i --input            Input file\n");
	printf("  -o --output           Output file\n");
	printf("  -b --blur             Apply box blur\n");
	printf("  -g --gaussian         Apply Gaussian blur\n");
	printf("  -s --sharpen          Sharpen image\n");
	printf("  -u --unsharpen        Unsharpen image\n");
	printf("  -l --outline          Outline image\n");
	printf("  -e --emboss           Emboss image\n");
	printf("  --topsobel            Apply top Sobel filter\n");
	printf("  --bottomsobel         Apply bottom Sobel filter\n");
	printf("  --rightsobel          Apply right Sobel filter\n");
	printf("  --leftsobel           Apply left Sobel filter\n");
	printf("  -c FILE --custom=FILE Apply custom filter\n");
}
