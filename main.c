#include "first_pass.h"
#include "external_data_structure.h"
#include "binary_data_structure.h"
#include "output.h"
#include "utils.h"

int DC = 0;
int IC = INITIAL_ADDRESS;

int main(int argc, char *argv[])
{
    int retval = 1, i = 1;
    int ICF, DCF;
    FILE *file;

    retval = given_files(argc);
    if (retval == 0)
    {
        while (i < argc)
        {
            file = fopen(argv[i], "r");
            if (!file)
            {                
				printf("Couldn't open file %s\n", argv[i]);
                retval = ERROR;
            }
            else
            {
				printf("analyzing file %s...\n", argv[i]);
				init_data_structures();
                if (first_pass(file))
                {
                    ICF = IC;
                    DCF = DC;
                    add_to_data(ICF);
                    IC = INITIAL_ADDRESS;
                    DC = 0;
                    rewind(file);
                    if (second_pass(file)){
						make_output_files(argv[i], ICF - INITIAL_ADDRESS, DCF); /* add creation of output files */	
					}
                }
                free_data_structures();
				fclose(file);
            }
            i++;
			IC = INITIAL_ADDRESS;
			DC = 0;
        }
    }
    return retval;
}
