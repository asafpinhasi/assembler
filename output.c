#include "output.h"

void forward_line(int address, FILE * file)
{
    /* In case reached to an address that is a multiplication of 4, printing new line and the address */

    fprintf(file, "\n");
    fprintf(file, "0%d ", address);
}

int print_code_hex(FILE * ob_file)
{
    int i;
    long word;
    int address = 100;
    code_row_ptr code_head;
    get_code_head(&code_head);

    while (code_head != NULL)
    {
        if ((address % 4) == 0)
            fprintf(ob_file, "\n");

        word = code_head->word;
        fprintf(ob_file, "0%d ", address);
        address += 4;

        for (i = 0; i < 4; i++)
        {
            fprintf(ob_file,"%lX",(word & 0xF0) >> 4); /* high nibble */
            fprintf(ob_file,"%lX ",word & 0x0F); /* low nibble */
            /* finished 1 byte, now shift right 8 bit to get the next byte */
            word >>= 8;
        }
        code_head = code_head->next;
    }
    return address;
}

void print_data_hex(FILE * ob_file, int address)
{
    int i;
    code_row_ptr code_head;
    get_data_head(&code_head);

    if ((address % 4) == 0)
        forward_line(address, ob_file);

    while (code_head != NULL)
    {
        if (!strcmp(code_head->type, "word"))
        {
            long word = code_head->word;
            for (i = 0; i < 4; i++)
            {
                fprintf(ob_file,"%lX",(word & 0xF0) >> 4); /* high nibble */
                fprintf(ob_file,"%lX ",word & 0x0F); /* low nibble */
                /* finished 1 byte, now shift right 8 bit to get the next byte */
                word >>= 8;

                address++;
                if ((address % 4) == 0)
                    forward_line(address, ob_file);
            }
        }
        else if (!strcmp(code_head->type, "half_word"))
        {
            long word = code_head->word;
            for (i = 0; i < 2; i++)
            {
                fprintf(ob_file,"%lX",(word & 0xF0) >> 4); /* high nibble */
                fprintf(ob_file,"%lX ",word & 0x0F); /* low nibble */
                /* finished 1 byte, now shift right 8 bit to get the next byte */
                word >>= 8;

                address++;
                if ((address % 4) == 0)
                    forward_line(address, ob_file);
            }
        }

        else if (!strcmp(code_head->type, "byte"))
        {
            long word = code_head->word;

            fprintf(ob_file,"%lX",(word & 0xF0) >> 4); /* high nibble */
            fprintf(ob_file,"%lX ",word & 0x0F); /* low nibble */

            address++;
            if ((address % 4) == 0)
                forward_line(address, ob_file);
        }
        code_head = code_head->next;
    }
}

void make_ext_file(char * file_name)
{
    /* Makes the .ext file (If needed) */

    external_row_ptr ext_head;
    get_external_head(&ext_head);

    if (ext_head != NULL)
    {
        FILE * ext_file;
        char * ext_file_name = (char *) malloc((strlen(file_name) + 4) * sizeof(char));

        strcpy(ext_file_name, file_name);
        strcat(ext_file_name, ".ext");
        ext_file = fopen(ext_file_name, "w");

        while (ext_head != NULL)
        {
            fprintf(ext_file, "%s 0%d\n", ext_head->symbol, ext_head->address);
            ext_head = ext_head->next;
        }
        fclose(ext_file);
    }
}

void make_ent_file(char * file_name)
{
    /* Makes the .ext output file (If needed) */
    int entry_occ = 0;
    row_ptr symbol_head;
    get_symbol_head(&symbol_head);


    /* First, count the number of occurences of "entry" existence, to determine whether to open a file */
    while (symbol_head != NULL)
    {
        if ((!strcmp(symbol_head->attributes, "code,entry")) || (!strcmp(symbol_head->attributes, "data,entry")))
            entry_occ++;
        symbol_head = symbol_head->next;
    }

    if (entry_occ > 0)
    {
	FILE * ent_file;
	char * ent_file_name;
        get_symbol_head(&symbol_head); /* Rewind head to the start */
        
        ent_file_name = (char *) malloc((strlen(file_name) + 4) * sizeof(char));

        strcpy(ent_file_name, file_name);
        strcat(ent_file_name, ".ent");

        ent_file = fopen(ent_file_name, "w");

        while (symbol_head != NULL)
        {
            if ((!strcmp(symbol_head->attributes, "code,entry")) || (!strcmp(symbol_head->attributes, "data,entry")))
                fprintf(ent_file, "%s 0%d\n", symbol_head->symbol, symbol_head->value);
            symbol_head = symbol_head->next;
        }
        fclose(ent_file);
    }
}

void make_ob_file(char * file_name, int ICF, int DCF)
{
    int address;
    code_row_ptr code_head;
    FILE * ob_file;
    char *  ob_file_name = (char*) malloc((strlen(file_name) + 3) * sizeof(char));

    strcpy(ob_file_name, file_name);
    strcat(ob_file_name, ".ob");

    ob_file = fopen(ob_file_name, "w");
    fprintf(ob_file, "     %d %d     ", ICF, DCF);

    
    get_code_head(&code_head);

    address = print_code_hex(ob_file);
    print_data_hex(ob_file, address);

}

void make_output_files(char * file_name, int ICF, int DCF)
{
    make_ob_file(file_name, ICF, DCF);
    make_ext_file(file_name);
    make_ent_file(file_name);
}
