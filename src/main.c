#include <stdio.h>
#include <getopt.h>

#include "main.h"

int main(int argc, char *argv[])
{
	const char *input_file = NULL;
	const char *output_file = NULL;
	int c;
	int long_option_index = 0;
	static struct option long_opts[] = {
		{"help", no_argument, NULL, 'h'},
		{"input", required_argument, NULL, 'i'},
		{"output", required_argument, NULL, 'o'},
		{"blur", no_argument, NULL, 'b'},
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
			printf("Selected box blur\n");
			break;
		case 'g':
			printf("Selected gaussian blur\n");
			break;
		case 's':
			printf("Selected sharpen\n");
			break;
		case 'u':
			printf("Selected unsharpen\n");
			break;
		case 'l':
			printf("Selected outline\n");
			break;
		case 'e':
			printf("Selected emboss\n");
			break;
		case 1:
			printf("Selected top sobel filter\n");
			break;
		case 2:
			printf("Selected bottom sobel filter\n");
			break;
		case 3:
			printf("Selected right sobel filter\n");
			break;
		case 4:
			printf("Selected left sobel filter\n");
			break;
		case 'c':
			printf("Selected custom '%s'\n", optarg);
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
