#ifndef MAIN_H
#define MAIN_H

extern double mat_boxblur[];
extern double mat_gauss[];
extern double mat_sharp[];
extern double mat_unsharp[];
extern double mat_outline[];
extern double mat_emboss[];
extern double mat_topSob[];
extern double mat_btmSob[];
extern double mat_rtSob[];
extern double mat_ltSob[];
const int size_boxblur = 3;
const int size_gauss   = 3;
const int size_sharp   = 3;
const int size_unsharp = 5;
const int size_outline = 3;
const int size_emboss  = 3;
const int size_topSob  = 3;
const int size_btmSob  = 3;
const int size_rtSob   = 3;
const int size_ltSob   = 3;

/**
 * Prints usage information for qfilt utility
 *
 * @param utilname the name of program; will be displayed with usage statement
 */
void usage(char *utilname);

#endif /* MAIN_H */
