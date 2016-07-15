#include <stdio.h>
#include "main.h"

int main(int argc, char *argv[]) {
	usage(argv[0]);
}

void usage(char *utilname) {
	printf("Usage: %s <input_file> <output_file> [options]\n", utilname);
	printf("WARNING: UTILITY IS NOT FULLY FUNCTIONAL YET\n");
	printf("Options:\n");
	//TODO: add optional arguments for each filter option
	printf("  -b --blur             Apply box blur\n");
	printf("  -g --gaussian         Apply Gaussian blur\n");
	printf("  -s --sharpen          Sharpen image\n");
	printf("  -u --unsharpen        Unsharpen image\n");
	printf("  -o --outline          Outline image\n");
	printf("  -e --emboss           Emboss image\n");
	printf("  --topsobel            Apply top Sobel filter\n");
	printf("  --bottomsobel         Apply bottom Sobel filter\n");
	printf("  --rightsobel          Apply right Sobel filter\n");
	printf("  --leftsobel           Apply left Sobel filter\n");
	printf("  -c FILE --custom=FILE Apply custom filter\n");
}
