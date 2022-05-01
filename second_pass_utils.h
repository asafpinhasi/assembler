#ifndef SECOND_PASS_UTILS
#define SECOND_PASS_UTILS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>


int second_pass_get_numbers(char line[], int * p, long numbers[]);
void second_pass_get_first_number(char line[], int * p, long numbers[]);
long second_pass_get_num(char line[], int* p);

void second_pass_asciz_process(char* line, int* p);
void second_pass_data_storage_process(char* line, int* p, int directive);
void second_pass_get_operand_label(char* line, int* p, char* label);
int second_pass_entry_process(char* line, int* p);
void second_pass_print_error(char* error);
int second_get_line_number();
int second_pass_get_register(char* line, int* p);
int second_pass_get_immed(char* line, int* p);
void second_pass_I_memory_process(char* line, int* p, int* rs, int* rt, int* immed);
int second_pass_I_branched_process(char* line, int* p, int* rs, int* rt, int* immed);
void second_pass_I_arithmetic_process(char* line, int* p, int* rs, int* rt, int* immed);
void second_pass_R_arithmetic_process(char* line, int* p, int* rs, int* rt, int* rd);
void second_pass_R_copy_process(char* line, int* p, int* rs, int* rt);
void stop_process(int* reg, int* address);


int second_pass_process_instructive(char* line, int* p, int instructive);
int second_pass_instructive_name(char* temp);
int second_pass_get_instructive(char * line, int *p);
int second_pass_instructive_check(char * line, int * p);
int second_pass_get_directive(char * line, int *p);
int second_pass_directive_name(char* temp);
int second_pass_process_directive(char* line, int* p, int directive);
int second_pass_directive_check(char * line, int *p);
int second_pass_Jmp_La_Call_process(char* line, int* p, int instructive, int* reg, int* address);

int next_char(char* line, int* p);
void second_pass_next_parameter(char line[], int* p);

int get_opcode(int instructive);
int get_funct(int funct);
int second_pass_get_last_quote(char* line, int i);

int get_IC();
void increment_IC();
void increment_DC_by(int strLength);


#endif
