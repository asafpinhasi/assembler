#ifndef EXTERNAL_DATA_STRUCTURE
#define EXTERNAL_DATA_STRUCTURE

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "constants.h"

typedef struct binary_external_code * external_row_ptr;
typedef struct binary_external_code
{
    int address; /* The address the external label was used*/
    external_row_ptr next; /* Pointer to the next word struct */
    char * symbol; /* Name of the label defined as external */

} external_row;

external_row_ptr external_head, external_tail; /* Initialize external head & tail */

void progress_external_tail();
void get_external_tail(external_row_ptr* ptrtail);
void get_external_head(external_row_ptr* ptrhead);
void insert_external(char * symbol, long address);
void free_external_table(external_row_ptr* ptrhead);
void get_external_head_to_free(external_row_ptr* ptrhead);

#endif
