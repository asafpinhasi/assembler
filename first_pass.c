/* 
This file is in charge of te first pass over the input. It uses first_pass_utils for most of the hard work.
In general in the first pass the program checks that the input is valid and raises error messages when needed. 
In addition it fills the lable table for use in the second pass
*/

#include "constants.h"
#include "first_pass.h"

int firstLineNumber = 1;

int first_pass(FILE * file)
{
    int retval = 1;

    char line[MAX_LINE_LENGTH + 1];
    firstLineNumber = 1;
    while (fgets(line, sizeof(line), file))
    {
        if (!analyze_line(line, file))
            retval = ERROR;
        firstLineNumber++;
    }

    return retval;
}


int analyze_line(char* line, FILE * file)
{
    int retval = 1, i = 0, gotLabel;
    char label[MAX_LABEL_LENGTH];
    if(!no_more_chars(line, &i) && line[i] != ';')  /* not an empty line or comment */
    {
		if(!check_line_length(line, file))
	    	retval = ERR;
        else if (!get_label(line, &i, label, &gotLabel))
            retval = ERR;
        else if (gotLabel && no_more_chars(line, &i)) {
            printf("In line %d: error: illegal command - line cannot contain only a label\n",
                   first_get_line_number());
            retval = ERR;
        }
        else if (line[i] == '.'){
            i++;
            retval = directive_check(line, &i, label, &gotLabel);
        }
        else
            retval = instructive_check(line, &i, label, &gotLabel);
    }
    return retval;
}



int first_get_line_number()
{
    return firstLineNumber;
}
