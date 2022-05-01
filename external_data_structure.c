/*
This file holds functions to deal with the data structure that holds external commands.
It holds both the line of the address of the command in memory and the name of the external variable.
Later it will be put into an output file.
*/

#include "external_data_structure.h"

void insert_external(char * symbol, long address)
{
    /* Initialize new row*/
    external_row_ptr new_row = (external_row_ptr)malloc(sizeof(external_row));
    external_row_ptr last_row;
    int len_symbol;

    len_symbol = strlen(symbol);
    get_external_tail(&last_row);

    new_row->symbol = (char *) malloc(len_symbol+1);
    strcpy(new_row->symbol, symbol);

    new_row->address = address;

    new_row->next = NULL;
    last_row->next = new_row;
    progress_external_tail();
}

void get_external_head_to_free(external_row_ptr* ptrhead)
{
    *ptrhead = external_head;
}


void get_external_head(external_row_ptr* ptrhead)
{
	/* returning head->next since first node is not used (dummy) */
    (*ptrhead) = (external_head->next);
}

void get_external_tail(external_row_ptr* ptrtail)
{
    (*ptrtail) = external_tail;
}

void progress_external_tail()
{
    external_tail = external_tail->next;
}

void free_external_table(external_row_ptr * head)
{
    external_row_ptr tmp;
	tmp = (*head);
    (*head) = (*head)->next;
    free(tmp);

    while ((*head) != NULL)
    {
        tmp = (*head);
        (*head) = (*head)->next;
        free(tmp->symbol);
        free(tmp);
    }
}
