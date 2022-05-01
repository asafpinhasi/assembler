/*
 * This file contains many functions that are used for analyzing a row read at the
 * first pass of the assembler. The functions check the validity (syntax and logic) of the line ,
 * print error messages, and fill the symbol table with labels.
 */


#include "first_pass_utils.h"

/*
 * Retval is short for return value, and is used numerous time in the code.
 * line[] holds the line read from the given file.
*/


/*
 * Checks if the directive line is valid. returns 1 if valid, 0 otherwise.
 * first read the directive received, and the uses process_directive
 * to continue processing the line. Prints errors if needed.
 */
int directive_check(char * line, int *p,  char* label, int* gotLabel)
{
    int retval = 1, directive;
    /* gets the directive, and checks if it is valid */
    if (!(directive = get_directive(line, p))) {
        print_err("undefined directive name\n");
        retval = ERR;
    }
        /* processes the rest of the line according to the directive read */
    else if(!process_directive(line, p, directive, label, gotLabel))
        retval = ERR;
    return retval;
}


/*
 * Processes the parameters of a directive according to the directive received.
 * Returns 1 if valid, 0 otherwise.
 */
int process_directive(char* line, int* p, int directive, char* label, int* gotLabel)
{
    int retval = 0;
    /* check if the line is valid, and if it is and a label was received, inserts it to the table */
    if (directive >= DB && directive <= ASCIZ) {
        int dc = get_DC();
        if (directive == ASCIZ)     /* processes asciz directive */
            retval = asciz_process(line, p);
        else        /* processes db, dh, dw directives */
            retval = data_storage_process(line, p, directive);
        if(retval && *gotLabel)
            insert_symbol(label, dc, "data");
    }
    else if (directive == EXTERN) {
        if (*gotLabel)
            print_warning("a label was declared before external directive\n");
        retval = extern_process(line, p);   /* processes extern directive */
    }
    else if (directive == ENTRY) {
        if (*gotLabel)
            print_warning("a label was declared before entry directive\n");
        retval = entry_process(line, p);    /* processes entry directive */
    }
    return retval;
}



/*
 * Analyzes asciz command - receives a string in the format "string".
 * Prints error messages. returns 1 if valid, 0 otherwise.
 */
int asciz_process(char* line, int* p)
{
    int retval = 1, strLength = 1; /* strLength - the length of the string */
    if(no_more_chars(line, p))
        print_err("missing string after asciz command\n");
    else if(line[(*p)++] != '"') {
        print_err("illegal start of string - should start with \"\n");
        retval = ERR;
    }
    else if(!get_string(line, p, &strLength))	/* checks if the string received is valid */
        retval = ERR;
    else if(!no_more_chars(line, p)) {
        print_err("extraneous text after end of command\n");
        retval = ERR;
    }
    else
	/* increments DC by the string length +1 - the memory needed to store the string */
        increment_DC_by(strLength);	
    return retval;
}



/*
 * Analyzes .db, .dh, .dw commands - receives numbers in the format - num1, num2  , ... , numN
 * Prints error messages. returns 1 if valid, 0 otherwise.
 */
int data_storage_process(char* line, int* p, int directive)
{
    /* numOfNums - holds the number of numbers read, size - memory size (in bits) to store each number */
    int retval = 1, numOfNums, size;
    long numbers[MAX_LINE_LENGTH];
    if(directive == DB)
        size = BYTE;
    else if(directive == DW)
        size = WORD;
    else if(directive == DH)
        size = HALF_WORD;
    /* gets the number of numbers read in order to calculate the memory needed */
    numOfNums = get_numbers(line, p, numbers, size);
    if(numOfNums <= 0){
        retval = ERR;
    }
    else
        /* increments DC by the memory size needed to store the numbers read */
        increment_DC_by(numOfNums*(size/8));
    return retval;
}

/*
 * Analyzes extern command - receives a label. Prints error messages.
 * Prints error messages. returns 1 if valid, 0 otherwise.
 */
int extern_process(char* line, int* p)
{
    int retval = 1;
    char label[MAX_LABEL_LENGTH];
    if(no_more_chars(line, p)){
        print_err("missing label after external instructive\n");
        retval = ERR;
    }
        /* prints error if the label read is not valid */
    else if(!get_operand_label(line, p, label, 1))
        retval = ERR;
        /* prints error messages if there are characters after end of command */
    else if(!no_more_chars(line, p)){
        print_err("extraneous text after end of command\n");
        retval = ERR;
    }
        /* if all is well, enters the label to symbol table with "external" attribute */
    else
        insert_symbol(label, 0, "external");
    return retval;
}


/*
 * Analyzes entry command - receives a label, and makes sure it is valid.
 * Prints error messages. Returns 1 if valid, 0 otherwise.
 */
int entry_process(char* line, int* p)
{
    int retval = 1;
    char label[MAX_LABEL_LENGTH];
    if(no_more_chars(line, p)){
        print_err("missing label after entry instructive\n");
        retval = ERR;
    }
        /* prints error if the label read is not valid */
    else if(!get_operand_label(line, p, label, 0))
        retval = ERR;
        /* prints error message if there are characters after end of command */
    else if(!no_more_chars(line, p)){
        print_err("extraneous text after end of command\n");
        retval = ERR;
    }
    return retval;
}




/*
 * Checks if the instructive is valid and enters label to the symbol table if received.
 * Returns 1 if valid, 0 otherwise. Prints error messages if needed.
*/
int instructive_check(char * line, int * p, char* label, int* gotLabel)
{
    int retval = 1, instructive;
    /* gets the instructive, and checks if it is valid */
    if (!(instructive = get_instructive(line, p))) {
        print_err("undefined instructive name or illegal label\n");
        retval = ERR;
    }
        /* processes the instructive and checks the line is valid */
    else if(!process_instructive(line, p, instructive))
        retval = ERR;
    else {
        if (*gotLabel) /* if a label was received, inserts it to the symbol table */
            insert_symbol(label, get_IC(), "code");
        increment_IC(); /* increments IC by 4 for every valid instructive received */
    }
    return retval;
}


/*
 * Processes the parameters of an instructive according to the instructive received.
 * returns 1 if valid, 0 otherwise.
 */
int process_instructive(char* line, int* p, int instructive)
{
    int retval = 0;
    if (instructive >= ADD && instructive <= NOR)    /* R logical/arithmetical instructive */
        retval = R_arithmetic_process(line, p);
    else if (instructive >= MOVE && instructive <= MVLO) /* R copy instructive */
        retval = R_copy_process(line, p);
    else if (instructive >= ADDI && instructive <= NORI) /* I arithmetic instructive */
        retval = I_arithmetic_process(line, p);
    else if (instructive >= BNE && instructive <= BGT) /* I branched instructive */
        retval = I_branched_process(line, p);
    else if (instructive >= LB && instructive <= SH) /* I memory instructive */
        retval = I_memory_process(line, p);
    else if (instructive == JMP) /* jmp instructive */
        retval = Jmp_process(line, p);
    else if (instructive >= LA && instructive <= CALL) /* la or call instructive */
        retval = La_Call_process(line, p);
    else if(instructive == STOP)
        retval = 1;
    if(retval && !no_more_chars(line, p)) {
        print_err("extraneous text after end of command\n");
        retval = ERR;
    }
    return retval;
}

/*
 * functions used by process_instructive for further processing and analyzing of the instructive line are:
 * I_memory_process, I_branched_process, I_arithmetic_process, R_arithmetic_process,
 * R_copy_process, Jmp_process and La_Call_process.
 */
/*
 * Analyzes lb, sb, lw, sw, lh, sh commands.
 * Gets parameters in the format - $register1  ,  immed,  $register2.
 * Prints error messages. returns 1 if valid, 0 otherwise.
 */
int I_memory_process(char* line, int* p)
{
    int retval = 0;
    if(no_more_chars(line,p))
        print_err("missing parameter\n");
    else if(line[*p] == ',')
        print_err("illegal comma\n");
        /* receives parameters according to the instructive */
    else if(get_register(line, p) == -1);
    else if(!next_parameter(line, p) || !valid_num(line, p, MAX_IMMED));
    else if(!next_parameter(line, p) || (get_register(line, p) == -1));
    else retval = 1;
    return retval;
}

/*
 * Analyzes beq, bne, blt, bgt commands.
 * Gets parameters in the format - $register1  ,  &register2,  $register3.
 * Prints error messages. returns 1 if valid, 0 otherwise.
 */
int I_branched_process(char* line, int* p)
{
    int retval = 0;
    char label[MAX_LABEL_LENGTH];
    if(no_more_chars(line,p))
        print_err("missing parameter\n");
    else if(line[*p] == ',')
        print_err("illegal comma\n");
    else if(get_register(line, p) == -1);
    else if(!next_parameter(line, p) || (get_register(line, p) == -1));
    else if(!next_parameter(line, p) || !get_operand_label(line, p, label, 0));
    else retval = 1;
    return retval;
}

/*
 * analyzes addi, subi, andi, ori, nori commands.
 * Gets parameters in the format - $register1  ,  immed,  $register3.
 * Prints error messages. returns 1 if valid, 0 otherwise.
 */
int I_arithmetic_process(char* line, int* p)
{
    int retval = 0;
    if(no_more_chars(line,p))
        print_err("missing parameter\n");
    else if(line[*p] == ',')
        print_err("illegal comma\n");
    else if(get_register(line, p) == -1);
    else if(!next_parameter(line, p) || !valid_num(line, p, MAX_IMMED));
    else if(!next_parameter(line, p) || (get_register(line, p) == -1));
    else retval = 1;
    return retval;
}

/*
 * Analyzes add, sub, and, or, nor  commands.
 * Gets parameters in the format - $register1  ,  &register2,  $register3.
 * Prints error messages. returns 1 if valid, 0 otherwise.
 */
int R_arithmetic_process(char* line, int* p)
{
    int retval = 0;
    if(no_more_chars(line,p))
        print_err("missing parameter\n");
    else if(line[*p] == ',')
        print_err("illegal comma\n");
        /* receives parameters */
    else if(get_register(line, p) == -1);
    else if(!next_parameter(line, p) || (get_register(line, p) == -1));
    else if(!next_parameter(line, p) || (get_register(line, p) == -1));
    else retval = 1;
    return retval;
}

/*
 * Analyzes move, mvhi, mvlo commands.
 * Gets parameters in the format - $register1  ,  &register2 .
 * Prints error messages. returns 1 if valid, 0 otherwise.
 */
int R_copy_process(char* line, int* p)
{
    int retval = 0;
    if(no_more_chars(line,p))
        print_err("missing parameter\n");
    else if(line[*p] == ',')
        print_err("illegal comma\n");
        /* receives parameters */
    else if(get_register(line, p) == -1);
    else if(!next_parameter(line, p) || (get_register(line, p) == -1));
    else retval = 1;
    return retval;
}

/*
 * Analyzes jmp command - receives a label or a register. Prints error messages.
 * returns 1 if valid, 0 otherwise.
 */
int Jmp_process(char* line, int* p)
{
    int retval = 0;
    char label[MAX_LABEL_LENGTH];
    if(no_more_chars(line,p))
        print_err("missing parameter\n");
    else if(line[*p] == '$') {
        if(get_register(line, p) != -1)
            retval = 1;
    }
    else if(get_operand_label(line, p, label, 0))
        retval = 1;
    return retval;
}

/*
 * Analyzes la and call commands - receives a label. Prints error messages.
 * returns 1 if valid, 0 otherwise.
 */
int La_Call_process(char* line, int* p)
{
    int retval = 0;
    char label[MAX_LABEL_LENGTH];
    if(no_more_chars(line,p))
        print_err("missing parameter\n");
    else if(get_operand_label(line, p, label, 0))
        retval = 1;
    return retval;
}

/*
 * Functions that are used to read a directive or instructive from the line:
 * get_directive, directive_name, get_instructive, instructive_name
 */
/*
 * Reads the directive from line.
 * Returns the directives number if valid, 0 otherwise. Prints error messages if needed.
 */
int get_directive(char * line, int *p)
{
    int directive, j = 0; /* j - index in dirct_name */
    char dirct_name[MAX_DIRCT_LENGTH+1]; /* dirct_name holds the directive name */
    if(no_more_chars(line, p))
        directive = ERR;
    else
    {
        /* reads directive into dirct_name array */
        while(j < MAX_DIRCT_LENGTH && isalpha(line[*p]))
            dirct_name[j++] = line[(*p)++];
        if(isspace(line[*p]) || line[*p] == '\0')
        {
            dirct_name[j] = '\0'; /* adding terminal to the directive name string */
            directive = directive_name(dirct_name); /* gets the directive */
        }
        else
            directive = ERR;
    }
    return directive;
}

/*
 * Reads the instructive from line.
 * Returns 1 if instructive is valid, 0 otherwise. Prints error messages if needed.
 */
int get_instructive(char * line, int *p)
{
    int instructive, j = 0; /* j - index in inst_name */
    char inst_name[MAX_INST_LENGTH+1];  /* inst_name holds the instructive name */
    if(no_more_chars(line, p))
        instructive = ERR;
    else
    {
        /* reads instructive into inst_name array */
        while(j < MAX_INST_LENGTH && isalpha(line[*p]))
            inst_name[j++] = line[(*p)++];
        if(isspace(line[*p]) || line[*p] == '\0')
        {
            inst_name[j] = '\0';    /* adding terminal to the directive name string */
            instructive = instructive_name(inst_name); /* gets the instructive */
        }
        else
            instructive = ERR;
    }
    return instructive;
}

/*
 * Compares the string received to all of the instructive names.
 * If valid returns the instructive received, otherwise returns 0.
*/
int instructive_name(char* temp)
{
    int instructive;
    if(!(strcmp(temp, "add")))
        instructive = ADD;
    else if(!(strcmp(temp, "sub")))
        instructive = SUB;
    else if(!(strcmp(temp, "and")))
        instructive = AND;
    else if(!(strcmp(temp, "or")))
        instructive = OR;
    else if(!(strcmp(temp, "nor")))
        instructive = NOR;
    else if(!(strcmp(temp, "move")))
        instructive = MOVE;
    else if(!(strcmp(temp, "mvhi")))
        instructive = MVHI;
    else if(!(strcmp(temp, "mvlo")))
        instructive = MVLO;
    else if(!(strcmp(temp, "addi")))
        instructive = ADDI;
    else if(!(strcmp(temp, "subi")))
        instructive = SUBI;
    else if(!(strcmp(temp, "andi")))
        instructive = ANDI;
    else if(!(strcmp(temp, "ori")))
        instructive = ORI;
    else if(!(strcmp(temp, "nori")))
        instructive = NORI;
    else if(!(strcmp(temp, "bne")))
        instructive = BNE;
    else if(!(strcmp(temp, "beq")))
        instructive = BEQ;
    else if(!(strcmp(temp, "blt")))
        instructive = BLT;
    else if(!(strcmp(temp, "bgt")))
        instructive = BGT;
    else if(!(strcmp(temp, "lb")))
        instructive = LB;
    else if(!(strcmp(temp, "sb")))
        instructive = SB;
    else if(!(strcmp(temp, "lw")))
        instructive = LW;
    else if(!(strcmp(temp, "sw")))
        instructive = SW;
    else if(!(strcmp(temp, "lh")))
        instructive = LH;
    else if(!(strcmp(temp, "sh")))
        instructive = SH;
    else if(!(strcmp(temp, "jmp")))
        instructive = JMP;
    else if(!(strcmp(temp, "la")))
        instructive = LA;
    else if(!(strcmp(temp, "call")))
        instructive = CALL;
    else if(!(strcmp(temp, "stop")))
        instructive = STOP;
    else
        instructive = ERR;
    return instructive;
}

/*
 * Compares the string received to all of the directive names.
 * If valid, returns the directive received, otherwise returns 0.
*/
int directive_name(char* temp)
{
    int directive;
    if(!(strcmp(temp, "db")))
        directive = DB;
    else if(!(strcmp(temp, "dw")))
        directive = DW;
    else if(!(strcmp(temp, "dh")))
        directive = DH;
    else if(!(strcmp(temp, "asciz")))
        directive = ASCIZ;
    else if(!(strcmp(temp, "entry")))
        directive = ENTRY;
    else if(!(strcmp(temp, "extern")))
        directive = EXTERN;
    else
        directive = ERR;
    return directive;
}


/*
 * Functions used to read and store numbers:
 * get_numbers, get_first_number, get_num, next_num

 * Reads from the line all numbers until the end of the line (or an error) in the format above.
 * Size is a parameter holding the number of bits to represent each number with.
 * Returns the number of numbers read - -1 if not valid
 */

int get_numbers(char line[], int * p, long numbers[], int size)
{
    /* valid - 1 if a number read is valid, 0 otherwise*/
    int retval = 1, valid;
    long num; /* num - value of current number read  */
    long maxNum = pow(2.0, (long)(size-1))-1; /* calculates the max value of each number */
    /*  reads first number into array, and if valid enters condition   */
    if(get_first_number(line, p, numbers, &valid, maxNum)){
        /* reads numbers from line as long as they exist, and no error was encountered*/
        while(!no_more_chars(line, p) && retval) {
            if (!next_num(line, p))
                retval = ERR;
            else if (line[*p] == ',') {
                print_err("multiple consecutive commas\n");
                retval = ERR;
            }
            else {    /* if valid, puts the number that was read in number array*/
                num = get_num(line, p, &valid, maxNum);
                if (valid)
                    numbers[retval++] = num;
                else
                    retval = ERR;
            }
        }
    }
    else
        retval--;
    return retval;
}

/*
 *  Reads the first number, and prints error messages if needed. Returns 1 if valid, 0 otherwise.
 */
int get_first_number(char line[], int * p, long numbers[], int* valid, long maxNum)
{
    int retval = 1;
    long num;
    if(no_more_chars(line, p)){
        print_err("no numbers received as parameters\n");
        retval = ERR;
    }
    else if(line[*p] == ','){
        print_err("illegal comma\n");
        retval = ERR;
    }
    else {
        num = get_num(line, p, valid, maxNum);
        if (*valid)
            numbers[0] = num;
        else
            retval = ERR;
    }
    return retval;
}

/*
 * Reads the next single number in line for read_set command and returns it.
 * Prints error messages if needed. Gives valid value 1 if number is valid, 0 otherwise.
 */
long get_num(char line[], int* p, int* valid, long maxNum)
{
    int j = 0;  /*  j - index in the current number */
    long retNum;	/* retNum - the number read */
    int  isNegative = 0;    /* holds the sign of the number */
    char temp[MAX_LINE_LENGTH]; /* contains the number being read*/
    char ch;        /* an assisting char */

    if(line[*p] == '-') {
        isNegative = 1;
        (*p)++;
    }
    else if(line[*p] == '+')
        (*p)++;
    while(isdigit(temp[j] = ch = line[(*p)])){	/* while reads digits, puts them in temp[] */
        j++;
        (*p)++;
    }
    /* atol returns the value of a number in a string passed to it (as a long int) */
    if(isspace(ch) || ch ==',' || ch == '\0') { /* a proper end of a number */
        if((retNum = atol(temp)) > maxNum) { 	/* if number is out of range */
			if(isNegative)
				retNum = -retNum;
            printf("In line %d: error: number %ld is out of range for this instructive\n",
                   first_get_line_number(), retNum);
            *valid = ERR;
        }
        else
            *valid = 1;
    }
    else{	/* received a non digit character - invalid number */
        print_err("invalid parameter - not an integer\n");
        *valid = ERR;
    }
    if(isNegative)
        retNum = -retNum;
    return retNum;	/* returns number read */
}

/*
 * Reads a comma, and proceeds to the next number (ignores white spaces).
 * Prints error messages. returns 1 if valid, 0 otherwise.
 */
int next_num(char line[], int* p)
{
    int retval = 1;
    if(line[(*p)++] != ','){    /* gets comma, if not prints error message */
        print_err("missing comma\n");
        retval = ERR;
    }
    else if(no_more_chars(line, p)){
        print_err("list of numbers cannot be terminated with a comma\n");
        retval = ERR;
    }
    return retval;	/* Returns 1 if all is well, 0 otherwise */
}

/*
 * Functions used to read and check label in line:
 * get_operand_label, valid_operand_label, get_label and valid_label.
 *
 *
 * Reads an label as an operand of a command.
 * Returns 1 if label is valid, 0 otherwise. Prints error messages if needed.
 */
int get_operand_label(char* line, int* p, char* label, int isExternal)
{
    int retval = 1, j = 0;       /* j - index in label*/
    /* reads potential label into the array */
    char ch;
    if(isalpha(line[(*p)])){
        /* reads the label into the array while reads digits or characters */
        while(j < MAX_LABEL_LENGTH && (isalpha(line[*p]) || isdigit(line[*p])))
            label[j++] = line[(*p)++];
        if(j == MAX_LABEL_LENGTH) {
            print_err("non valid label - too long\n");
            retval = ERR;
        }
            /* in case encountered non valid character */
        else if(!isspace(ch = line[*p]) && ch != '\n' && ch != '\0'){
            print_err("non valid label - contains characters that are not digits or alphabetic\n");
            retval = ERR;
        }
        else {
            label[j] = '\0'; /* adding terminal sign to the end of the string */
            retval = valid_operand_label(label, isExternal); /* checks if the label is valid (not a saved word) */
        }
    }
    else {
        print_err("non valid label - doesn't start with a character\n");
        retval = ERR;
    }
    return retval;
}

/*
 * Checks if the label for the operand is valid.
 * Returns 1 if label is valid, 0 otherwise. Prints error messages if needed.
 */
int valid_operand_label(char* label, int isExternal)
{
    int retval = 1;
    /* if label is a saved word - non valid */
    if(directive_name(label) || instructive_name(label)) {
        printf("In line %d: error: non valid label - %s is a saved word\n",
               first_get_line_number(), label);
        retval = ERR;
    }
    else if(symbol_exists(label)) {
        char attribute[MAX_ATTRIBUTE_LENGTH];
        get_symbol_attributes(label, attribute);
        if(isExternal && strcmp(attribute, "external")) {
            printf("in line %d: error: %s was already declared as non external\n", 
				first_get_line_number(), label);
            retval = ERR;
        }
    }
    return retval;
}


/*
 * Reads a label (in the beginning of a command line).
 * Returns 1 if label is valid, 0 otherwise. Prints error messages if needed.
 */
int get_label(char* line, int* p, char* label, int* gotLabel)
{
    int retval = 1, j = 0, i = (*p);       /* j - index in la, i index in line*/
    /* reads potential label into the array */
    *gotLabel = 0;
    if(isalpha(line[i])){
        while(j < MAX_LABEL_LENGTH && (isalpha(line[i]) || isdigit(line[i])))
            label[j++] = line[i++];
        if(j == MAX_LABEL_LENGTH) {
            print_err("Non valid start of command\n");
            retval = ERR;
        }
        else if(line[i++] == ':') {
            if (isspace(line[i]) || line[i] == '\0' || line[i] == '\n') {
                label[j] = '\0'; /* adding terminal to the directive name string */
                *p = i + 1; /* only of the word is a label, proceeds with index through the line */
                *gotLabel = 1;
                retval = valid_label(label);    /* checks if the label is valid in case got a label */
            }
            else {
                print_err("Missing space after end of label\n");
                retval = ERR;
            }
        }
    }
    return retval;
}

/*
 * Checks if the label is valid (at the beginning of the line).
 * Returns 1 if label is valid, 0 otherwise. Prints error messages if needed.
 */
int valid_label(char* label)
{
    int retval = 1;
    if(directive_name(label) || instructive_name(label)) {
        printf("In line %d: error: %s is a saved word\n", first_get_line_number(), label);
        retval = ERR;
    }
    else if(symbol_exists(label)) {
        printf("In line %d: error: %s was already declared\n", first_get_line_number(), label);
        retval = ERR;
    }
    return retval;
}

/*
 * Miscellaneous helper functions used in the functions above.
 */

/*
 * Reads a comma and proceeds to the next parameter (ignores white spaces).
 * Prints error messages. returns 1 if valid, 0 otherwise.
 */
int next_parameter(char line[], int* p)
{
    int retval = 1;
    /* checks if there is non space characters in the line, and proceeds to it
    if the line the line is empty, prints error message*/
    if(no_more_chars(line, p)){
        print_err("Missing parameter for this command\n");
        retval = ERR;
    }
        /* gets comma, if not prints error message */
    else if(line[(*p)++] != ','){
        print_err("Missing comma\n");
        retval = ERR;
    }
    else if(no_more_chars(line, p)){ /* same as before */
        print_err("Missing parameter for command\n");
        retval = ERR;
    }
    else if(line[*p] == ','){
        print_err("Multiple consecutive commas\n");
        retval = ERR;
    }
    return retval;	/* Returns 1 if all is well, 0 otherwise */
}


/*
 * Reads a number from the line, and checks if it is valid.
 * If it is valid returns 1, otherwise returns 0.
 */
int valid_num(char* line, int* p, double maxNum)
{
    int valid;
    get_num(line, p, &valid, maxNum);
    return valid;
}



/*
 * Checks if there are more significant characters in line.
 * If there are, proceeds to the next significant character and returns 0, otherwise returns 1.
 */
int no_more_chars(char* line, int* p)
{
    int no_more_chars = 0; /* holds return value */
    char ch;		/* temporary character */
    while((ch = line[*p]) == ' ' || ch == '\t') /* moves through line as long as a space/tab is read */
        (*p)++;
    if(ch == '\0' || ch == '\n')	/* if end of line reached without significant characters, returns 1 */
        no_more_chars = 1;
    return no_more_chars;
}

/*
 * Reads the register from line.
 * Returns the register number, -1 if not valid.
 * Prints error messages if needed.
 */
int get_register(char* line, int* p)
{
    int retval = -1, j = 0;       /* j - index in la, i index in line*/
    char ch;
    char regist[MAX_LINE_LENGTH];   /* holds the number of register read from line (as a string) */
    if(line[*p] != '$')
        print_err("parameter is not a register\n");
    else {
        (*p)++;
        while (isdigit(regist[j] = ch = line[(*p)])) {    /* while reads digits, puts them in temp[] */
            j++;
            (*p)++;
        }
        if(isspace(ch) || ch == '\0' || ch == ',') { /* a proper end of a number */
            regist[j] = '\0';
            if (atoi(regist) > MAX_REGISTER_NUM) /* if number is out of range */
                printf("In line %d: error: illegal register - %d is out of range\n",
                       first_get_line_number(), atoi(regist));
            else
                retval = atoi(regist);
        }
        else
            /* in case got a non digit which is not a space in a middle of a register */
            print_err("illegal register - contains non digits after $ sign\n");
    }
    return retval;
}

/*
 *	get_string is used by asciz to check the validity of the string.
 * 	A valid string is when the first and last character of the string is - ", and all 
 *	characters in between are printable. 
 */
int get_string(char* line, int* p, int* strLength) {
    int retval = 1, lastQuote; /* lastQoute holds the position in line of the last " */ 
    lastQuote = get_last_quote(line, *p);
	/* in case the first and last " are the same (string does not end properly) */
    if(lastQuote == (*p-1)){	
        print_err("illegal end of string - should end with \"\n");
        retval = ERR;
    }
    else {
	/* goes through the string, and checks all characters are printable */
        while((*p) < lastQuote && retval){
            if(!isprint(line[*p])){
                print_err("illegal char - non printable character in string\n");
                retval = ERR;
            }
            else{
                (*p)++;
                (*strLength)++;
            }
        }
	(*p)++; /* moves past the last " in line */
    }
	return retval;
}


/*
 *	get_last_quote is used by get_string in order to find the position of the last " in line.
 * 	it returns this position. 
 */ 
int get_last_quote(char* line, int i){
	/* goes to the last character in line */
    while(line[i] != '\n')
        i++;
	/* goes back from the end to the first " (from the end) it encounters */
    while(line[i] != '"')
        i--;
    return i;	/* returns the position of the last " */
}



/*
 * checks (in case of a significant line) that it is not longer that 80 characters.
 * Returns 1 if the line length is valid, 0 if not (and prints error messages).
 * In any case moves to the next line in the file (for further analysis of  the file).
 */
int check_line_length(char* line, FILE * file)
{
	int retval = 1, i = 0; /* i is the index in line */
	char ch;	/* assisting character */
	while(line[i] != '\0')
		i++;
	if(line[i-1] != '\n'){ /* if '\n' was not recieved in the first 80 characters in line */
		print_err("line is longer than 80 characters\n");
		retval = 0;
		while((ch = fgetc(file)) != EOF && ch != '\n'); /* moves to next '\n' of EOF */
		/* if EOF was received, moves one char back in file to avoid segmentation fault.
		next line analysis will receive EOF and stop analysis */
		if(ch == EOF)
			fseek(file, 0, SEEK_END);
	}
	return retval;
}


/*
 * Prints errors in a specific format.
 */
void print_err(char* error)
{
    int lineNumber = first_get_line_number();
    printf("in line %d: error: %s", lineNumber, error);
}

/*
 * Prints warnings in a specific format.
 */
void print_warning(char* error)
{
    int lineNumber = first_get_line_number();
    printf("in line %d: warning: %s", lineNumber, error);
}


