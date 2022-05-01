#ifndef FIRST_PASS_UTILS_H
#define FIRST_PASS_UTILS_H
#include "constants.h"
#include "label_data_structure.h"
#include "first_pass.h"
#include "constants.h"
#include <math.h>

enum{ERR, ADD, SUB, AND, OR, NOR, MOVE, MVHI, MVLO, ADDI, SUBI, ANDI, ORI, NORI, BNE, BEQ,
    BLT, BGT, LB, SB, LW, SW, LH, SH, JMP, LA, CALL, STOP};
enum{DB = 1, DW, DH, ASCIZ, ENTRY, EXTERN};

int no_more_chars(char* line, int* p);
int get_string(char* line, int* p, int* strLength);
int get_last_quote(char* line, int i);
int check_line_length(char* line, FILE * file);

int get_first_number(char line[], int * p, long numbers[], int* valid, long maxNum);
int get_numbers(char line[], int * p, long numbers[], int size);
int next_num(char line[], int* p);
long get_num(char line[], int* p, int* valid, long maxNum);
int valid_num(char*, int*, double);

int get_instructive(char * line, int *p);
int get_directive(char * line, int *p);
int instructive_name(char* temp);
int directive_name(char* temp);
int get_register(char* line, int* p);
int valid_operand_label(char* label, int isExternal);
int get_operand_label(char* line, int* p, char* label, int isExternal);
int next_parameter(char line[], int* p);
int valid_label(char* label);
int get_label(char* line, int* p, char* label, int* gotLabel);

int directive_check(char * line, int *p,  char* label, int* gotLabel);
int process_directive(char* line, int* p, int directive, char* label, int* gotLabel);
int data_storage_process(char* line, int* p, int directive);
int asciz_process(char* line, int* p);
int entry_process(char* line, int* p);
int extern_process(char* line, int* p);

int I_arithmetic_process(char* line, int* p);
int I_branched_process(char* line, int* p);
int I_memory_process(char* line, int* p);
int La_Call_process(char* line, int* p);
int Jmp_process(char* line, int* p);
int R_copy_process(char* line, int* p);
int R_arithmetic_process(char* line, int* p);
int instructive_check(char * line, int * p, char* label, int* gotLabel);
int process_instructive(char* line, int* p, int instructive);

int first_get_line_number();

void print_err(char* error);
void print_warning(char* error);

#endif
