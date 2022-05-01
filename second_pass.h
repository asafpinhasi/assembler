#ifndef SECOND_PASS
#define SECOND_PASS

#include <ctype.h>
#include <stdio.h>

void skip_label(char* line, int* p);
int second_pass_analyze_line(char* line);
int second_pass(FILE * file);

#endif
