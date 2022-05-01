#include "label_data_structure.h"
#include "constants.h"

void init_symbol_table()
{
    symbol_head = (row_ptr)malloc(sizeof(symbol_table_row));
    symbol_tail = symbol_head;
    symbol_tail->next = NULL;
}

void add_to_data(int ICF){
    row_ptr temp;
    get_symbol_head(&temp);
    while(temp != NULL)
    {
        if (!strcmp("data", temp->attributes))
            temp->value += ICF;
        temp = temp->next;
    }
}

int add_entry_to(char * symbol)
{
    row_ptr temp_row;
    get_symbol_head(&temp_row);
    while (temp_row != NULL)
    {
        if (!strcmp(temp_row->symbol, symbol)){
            if(!strcmp(temp_row->attributes, "code")){
                strcpy(temp_row->attributes, "code,entry");
            }
            else if(!strcmp(temp_row->attributes, "data"))
                strcpy(temp_row->attributes, "data,entry");
            return 1;
        }
        temp_row = temp_row->next;
    }
    return 0;
}

void insert_symbol(char * symbol, int value, char * attributes)
{
    /* Inserts a new symbol to the symbol table, with given symbol, address, and attributes */

    row_ptr new_row = (row_ptr)malloc(sizeof(symbol_table_row)); /* Initialize new row*/
    row_ptr last_row;
    int len_symbol;
    len_symbol = strlen(symbol);
    get_symbol_tail(&last_row);

    new_row->symbol = (char *) malloc(len_symbol+1);
    strcpy(new_row->symbol, symbol);

    new_row->value = value;
    new_row->attributes = (char *) malloc(MAX_ATTRIBUTE_LENGTH);
    strcpy(new_row->attributes, attributes);

    new_row->next = NULL;
    last_row->next = new_row;
    progress_symbol_tail();
}


int get_symbol_value(char * symbol)
{
    /* Returns the address of a label with a given symbol, -1 if doesn't exist */
    row_ptr temp_row;
    get_symbol_head(&temp_row);
    while (temp_row != NULL)
    {
        if (!strcmp(temp_row->symbol, symbol))
            return temp_row->value;
        temp_row = temp_row->next;
    }
    return -1;
}

int get_symbol_attributes(char * symbol, char * attributes)
{
    /* Copies the attributes for a given symbol, into the given string*/
    row_ptr temp_row;
    get_symbol_head(&temp_row);
    while (temp_row != NULL) {
        if (!strcmp(temp_row->symbol, symbol)) {
            strcpy(attributes, temp_row->attributes);
            return 1;
        }
        temp_row = temp_row->next;
    }
    return 0;
}

int symbol_exists(char * symbol)
{
    /* Checks if a symbol exists in the symbol table */
    row_ptr first_row, temp_row;
    get_symbol_head(&first_row);
    temp_row = first_row;
    while (temp_row != NULL)
    {
        if (!strcmp(temp_row->symbol, symbol))
            return 1;
        temp_row = temp_row->next;
    }
    return 0;
}

void get_symbol_head_to_free(row_ptr* ptrhead)
{
    /* Returning next since first is a a dummy initialization row */
    (*ptrhead) = symbol_head;
}

void get_symbol_tail(row_ptr* ptrtail)
{
    (*ptrtail) = symbol_tail;
}

void get_symbol_head(row_ptr* ptrhead)
{
    /* Returning next since first is a a dummy initialization row */
    (*ptrhead) = (symbol_head->next);
}

void progress_symbol_tail()
{
    symbol_tail = symbol_tail->next;
}

void free_symbol_table(row_ptr * head)
{
    row_ptr tmp;
	tmp = (*head);
    (*head) = (*head)->next;
    free(tmp);
	
    while ((*head) != NULL)
    {
        tmp = (*head);
        (*head) = (*head)->next;
        free(tmp->symbol);
        free(tmp->attributes);
        free(tmp);
    }
}
