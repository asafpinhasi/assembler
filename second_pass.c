#include "second_pass.h"
#include "second_pass_utils.h"
#include "constants.h"

int secondLineNumber = 1;
int second_pass(FILE * file){
    int retval = 1;
    char line[MAX_LINE_LENGTH + 1];
    secondLineNumber = 1;
    while (fgets(line, sizeof(line), file)){
        if (!second_pass_analyze_line(line))
            retval = ERROR;
        secondLineNumber++;
    }

    return retval;
}

int second_pass_analyze_line(char* line)
{
    int retval = 1, i = 0;
    if(next_char(line, &i) && line[i] != ';')  /* not an empty line or comment */
    {
        skip_label(line, &i);
        next_char(line, &i);
        if (line[i] == '.'){
            i++;
            retval = second_pass_directive_check(line, &i);
        }
        else
            retval = second_pass_instructive_check(line, &i);
    }
    return retval;
}

/*
 * Reads an label (in the beginning of a command line).
 * Returns 1 if label is valid, 0 otherwise. Prints error messages if needed.
 */
void skip_label(char* line, int* p)
{
    int i = (*p);       /* j - index in la, i index in line*/
    if(isalpha(line[i])){
        while(isalpha(line[i]) || isdigit(line[i]))
            i++;
        if(line[i++] == ':')
            *p = i;
    }
}

int second_get_line_number()
{
    return secondLineNumber;
}


