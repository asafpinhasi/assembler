#ifndef LABEL_DATA_STRUCTURE_H
#define LABEL_DATA_STRUCTURE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct row * row_ptr;
typedef struct row
{
    char * symbol;
    int value;
    char * attributes;
    row_ptr next;
} symbol_table_row;

row_ptr symbol_head, symbol_tail;

int add_entry_to(char * symbol);
void add_to_data(int ICF);
void get_symbol_head(row_ptr* ptrhead);
void get_symbol_tail(row_ptr* ptrtail);
void insert_symbol(char * symbol, int value, char * attributes);
void progress_symbol_tail();
int get_symbol_value(char * symbol);
int get_symbol_attributes(char * symbol, char * attributes);
int symbol_exists(char * symbol);
void free_symbol_table(row_ptr * head);
void init_symbol_table();
void get_symbol_head_to_free(row_ptr* ptrhead);

#endif
