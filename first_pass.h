#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#include "first_pass_utils.h"
#include "second_pass_utils.h"
#include "second_pass.h"
#include "binary_data_structure.h"
#include "label_data_structure.h"

int first_pass(FILE * file);
int analyze_line(char* line, FILE * file);

int get_DC();
int get_IC();

void increment_DC_by(int strLength);
void increment_IC();

void get_tail(row_ptr* ptrtail);
void get_head(row_ptr* ptrhead);
void progress_tail();

void init_symbol_table();

#endif
