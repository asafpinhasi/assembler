#ifndef BINARY_DATA_STRUCTURE
#define BINARY_DATA_STRUCTURE

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

typedef struct binary_machine_code * code_row_ptr;
typedef struct binary_machine_code
{
    long word:32; /* The binary word */
    code_row_ptr next; /* Pointer to the next word struct */
    char * type; /* Type of the code inside, i.e 'word' / 'half_word' / 'byte' */

} code_row;

code_row_ptr data_head, data_tail; /* Initialize data image head & tail */
code_row_ptr code_head, code_tail; /* Initialize code image head & tail */

void add_R_row(int opcode, int rs, int rt, int rd, int funct);
void add_I_row(int opcode, int rs, int rt, int immed);
void add_J_row(int opcode, int reg, int address);

void add_char_array(char * array);
void add_integer_array(long * array, char * type, int numOfNums);

void get_code_tail(code_row_ptr * ptrtail);
void get_code_head(code_row_ptr * ptrhead);
void get_code_head_to_free(code_row_ptr* ptrhead);
void progress_code_tail();

void get_data_tail(code_row_ptr * ptrtail);
void get_data_head(code_row_ptr * ptrhead);
void get_data_head_to_free(code_row_ptr* ptrhead);
void progress_data_tail();

void init_binary_tables();
void free_binary_table(code_row_ptr * ptrhead);

#endif
