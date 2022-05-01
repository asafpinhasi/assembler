/* 
This file holds functions to deal with the binary data structures. 
The binary data structures holds in machine code the coding of the commands.
This data structures will later be translated to be the output of the assembler.
*/

#include "binary_data_structure.h"
#include "external_data_structure.h"
#include "constants.h"

/* Add word of R type to the binary code image */
void add_R_row(int opcode, int rs, int rt, int rd, int funct)
{
    unsigned int temp;

    code_row_ptr last_row, code;
    get_code_tail(&last_row);

    code = (code_row_ptr)malloc(sizeof(code_row));
    code->next = NULL;
    code->word = 0;

    code->type = (char *)malloc(sizeof(char) * (TYPE_SIZE + 1));
    strcpy(code->type, "word");

    temp = opcode;
    code->word |= (temp << 26);

    temp = rs;
    code->word |= (temp << 21);

    temp = rt;
    code->word |= (temp << 16);

    temp = rd;
    code->word |= (temp << 11);

    temp = funct;
    code->word |= (temp << 6);

    temp = 0; /* Unused attribute */
    code ->word |= (temp);

    last_row->next = code;
    progress_code_tail();
}

/* Add word of I type to the binary code image */
void add_I_row(int opcode, int rs, int rt, int immed)
{
    int temp;

    code_row_ptr last_row, code;
    get_code_tail(&last_row);

    code = (code_row_ptr)malloc(sizeof(code_row));
    code->next = NULL;
    code->word = 0;

    code->type = (char *)malloc(sizeof(char) * (TYPE_SIZE + 1));
    strcpy(code->type, "word");

    temp = opcode;
    code->word |= (temp << 26);

    temp = rs;
    code->word |= (temp << 21);

    temp = rt;
    code->word |= (temp << 16);

    temp = immed;
    temp &= MASK_16_BITS; /* Take only the least significant 16 bits */
    code->word |= (temp);

    last_row->next = code;
    progress_code_tail();
}

/* Add word of J type to the binary code image */
void add_J_row(int opcode, int reg, int address)
{
    unsigned int temp;

    code_row_ptr last_row, code;
    get_code_tail(&last_row);

    code = (code_row_ptr)malloc(sizeof(code_row));
    code->next = NULL;
    code->word = 0;

    code->type = (char *)malloc(sizeof(char) * (TYPE_SIZE + 1));
    strcpy(code->type, "word");

    temp = opcode;
    code->word |= (temp << 26);

    temp = reg;
    code->word |= (temp << 25);

    temp = address;
    code->word |= (temp);

    last_row->next = code;
    progress_code_tail();
}

/* Add word from char array (asciz) to the binary code image */
void add_char_array(char * array)
{
    int i;

    code_row_ptr last_row;
    get_data_tail(&last_row);

    for (i=0; i < strlen(array) + 1; i++)
    {
        code_row_ptr code = (code_row_ptr)malloc(sizeof(code_row));
        code->next = NULL;
        code->word = 0;

        code->type = (char *)malloc(sizeof(char) * (TYPE_SIZE + 1));
        strcpy(code->type, "byte");

        code->word |= array[i];

        last_row->next = code;
        last_row = last_row->next;
        progress_data_tail();
    }
}

/* Add word from integer array (db / dh / dw) to the binary code image */
void add_integer_array(long * array, char * type, int length)
{
    int i, temp;

    code_row_ptr last_row;
    get_data_tail(&last_row);

    for (i=0; i < length; i++)
    {
        code_row_ptr code = (code_row_ptr)malloc(sizeof(code_row));
        code->next = NULL;
        code->word = 0;

        code->type = (char *)malloc(sizeof(char) * (strlen(type) + 1));
        strcpy(code->type, type);
        temp = array[i];

        if (!strcmp(type, "half_word"))
            temp &= MASK_16_BITS;
        else if (!strcmp(type, "byte"))
            temp &= MASK_8_BITS;

        code->word |= temp;

        last_row->next = code;
        last_row = last_row->next;
        progress_data_tail();
    }
}

void init_binary_tables()
/* Function to initialize table for data image, code image, and external labels image */
{
    data_head = (code_row_ptr)malloc(sizeof(code_row));
    data_tail = data_head;
    data_tail->next = NULL;

    code_head = (code_row_ptr)malloc(sizeof(code_row));
    code_tail = code_head;
    code_tail->next = NULL;

    external_head = (external_row_ptr)malloc(sizeof(external_row));
    external_tail = external_head;
    external_tail->next = NULL;
}

void get_data_head_to_free(code_row_ptr* ptrhead)
{
    (*ptrhead) = data_head;
}

void get_data_tail(code_row_ptr* ptrtail)
{
    (*ptrtail) = data_tail;
}

void get_data_head(code_row_ptr* ptrhead)
{
    /* Returning next since first is a a dummy initialization row */
    (*ptrhead) = (data_head->next);
}

void progress_data_tail()
{
    data_tail = data_tail->next;
}

void get_code_head_to_free(code_row_ptr* ptrhead)
{
    (*ptrhead) = code_head;
}

void get_code_tail(code_row_ptr* ptrtail)
{
    (*ptrtail) = code_tail;
}

void get_code_head(code_row_ptr* ptrhead)
{
    /* Returning next since first is a a dummy initialization row */
    (*ptrhead) = (code_head->next);
}

void progress_code_tail(){
    code_tail = code_tail->next;
}

void free_binary_table(code_row_ptr * head)
{
    code_row_ptr tmp;
	tmp = (*head);
    (*head) = (*head)->next;
    free(tmp);

    while ((*head) != NULL)
    {
        tmp = (*head);
        (*head) = (*head)->next;
        free(tmp->type);
        free(tmp);
    }
}


