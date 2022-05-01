#include "utils.h"


void init_data_structures()
{
	init_binary_tables();
	init_symbol_table();
}


void free_data_structures()
{
    code_row_ptr code_head;
	row_ptr symbol_head;
	external_row_ptr external_head;
    get_code_head_to_free(&code_head);
    free_binary_table(&code_head);
    get_data_head_to_free(&code_head);
    free_binary_table(&code_head);
	get_symbol_head_to_free(&symbol_head);
    free_symbol_table(&symbol_head);
	get_external_head_to_free(&external_head);
    free_external_table(&external_head);
}

int given_files(int n)
{
    if (n > 1)
        return 0;

    printf("No files given to analyze. Aborting...");
    return 1;
}

void increment_DC_by(int i)
{
    DC += i;
}

void increment_IC()
{
    IC+=4;
}

int get_DC()
{
    return DC;
}

int get_IC()
{
    return IC;
}
