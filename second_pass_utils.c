/*
 * This file contains many functions that are used for analyzing a row read at the
 * second pass of the assembler. The functions check in a few cases that the line is logically valid and
 * print error messagesthey also fill the data, code and externals tables.
 */


#include "second_pass_utils.h"
#include "binary_data_structure.h"
#include "label_data_structure.h"
#include "external_data_structure.h"
#include "constants.h"

enum{ERR, ADD, SUB, AND, OR, NOR, MOVE, MVHI, MVLO, ADDI, SUBI, ANDI, ORI, NORI, BNE, BEQ,
    BLT, BGT, LB, SB, LW, SW, LH, SH, JMP, LA, CALL, STOP};
enum{DB = 1, DW, DH, ASCIZ, ENTRY, EXTERN};

/*
 * The functions here are very similar to the functions in first_pass_utils.
 * In order not to repeat every explanation, and to maintain the simplicity of the code,
 * we will write fewer and shorter comments. For full explanations for a functions, go to the
 * appropriate function in first_pass_utils.c
 */



/*
 * processes the directive. returns 1 if valid, 0 otherwise.
 */
int second_pass_directive_check(char * line, int *p)
{
    int directive, retval;
    /* gets the directive */
    directive = second_pass_get_directive(line, p);
    /* processes the rest of the line according to the directive read */
    retval = second_pass_process_directive(line, p, directive);
    return retval;
}


/*
 * Processes the parameters of a directive according to the directive received.
 * Returns 1 if valid, 0 otherwise.
 */
int second_pass_process_directive(char* line, int* p, int directive)
{
    int retval = 1;
    /* check if the line is valid and inserts the data from it to the data table */
    if (directive >= DB && directive <= ASCIZ) {
        if (directive == ASCIZ)     /* processes asciz directive */
            second_pass_asciz_process(line, p);
        else        /* processes db, dh, dw directives */
            second_pass_data_storage_process(line, p, directive);
    }
    else if (directive == ENTRY)
        retval = second_pass_entry_process(line, p);    /* processes entry directive */
    return retval;
}


/*
 * Functions used by process_directive for further processing and analyzing of the directive line are:
 * second_pass_asciz_process, second_pass_data_storage_process, second_pass_extern_process
 * and second_pass_entry_process.
 */
/*
 * Analyzes asciz command - receives a string in the format - "string",
 * and adds it to the table, char by char.
 */
void second_pass_asciz_process(char* line, int* p)
{
    int j = 0, strLength = 1; /* strLength - the length of the string */
	int lastQuote;
    char string[MAX_LINE_LENGTH];
    next_char(line, p);
    (*p)++;
	lastQuote = second_pass_get_last_quote(line, *p);
    /* proceeds through the string until " encountered or end of line */
    while((*p) < lastQuote) {
        string[j++] = line[(*p)++];
        strLength++;
    }
    string[j] = '\0';
    /* adds the string received to the data table */
    add_char_array(string);
    /* increments DC by the string length +1 - the memory needed to store the string */
    increment_DC_by(strLength);
}


/*
 * Analyzes .db, .dh, .dw commands - receives numbers in the format - num1, num2  , ... , numN.
 * adds them to the table, one by one along with their type (byte, half word or word).
 */
void second_pass_data_storage_process(char* line, int* p, int directive)
{
    /* numOfNums - holds the number of numbers read, size - memory size to store each number in bits*/
    int numOfNums, size;
    char * type; /* holds the type of the numbers according to the directive */
    long numbers[MAX_LINE_LENGTH];
    if(directive == DB) {
        type = "byte";
        size = BYTE;
    }
    else if(directive == DW) {
        type = "word";
        size = WORD;
    }
    else if(directive == DH) {
        type = "half_word";
        size = HALF_WORD;
    }
    /* gets the number of numbers read in order to calculate the memory needed,
     * and fills "numbers" array with the numbers read */
    numOfNums = second_pass_get_numbers(line, p, numbers);
    /* increments DC by the memory size needed to store the numbers read */
    increment_DC_by(numOfNums*(size/8));
    /* adds the numbers received to the data table, with their type */
    add_integer_array(numbers, type, numOfNums);
}

/*
 * Analyzes entry command - receives a label, and makes sure it is valid. Prints error messages.
 * If valid, adds the feature ",entry" to the label's attribute in the symbol table.
 * Returns 1 if valid, 0 otherwise.
 */
int second_pass_entry_process(char* line, int* p)
{
    int retval = 1;
    char label[MAX_LABEL_LENGTH];
    next_char(line, p);
    second_pass_get_operand_label(line, p, label);
    if(!symbol_exists(label)) {
        printf("In line %d: error: operand label %s for entry directive does not exist in symbol table\n", second_get_line_number(), label);
        retval = ERROR;
    }
    else {
        char attribute[MAX_ATTRIBUTE_LENGTH];
        get_symbol_attributes(label, attribute);
        if (!strcmp(attribute, "external")) {
            printf("In line %d: error: %s was already declared as external and can't be declared as entry\n", second_get_line_number(), label);
            retval = ERROR;
        }
        else
            add_entry_to(label);
    }
    return retval;
}


/*
 * processes the instructive. returns 1 if valid, 0 otherwise.
 */
int second_pass_instructive_check(char * line, int * p)
{
    int instructive, retval;
    /* gets the instructive */
    instructive = second_pass_get_instructive(line, p);
    /* processes the instructive and checks the line is valid */
    retval = second_pass_process_instructive(line, p, instructive);
    increment_IC();
    return retval;
}


/*
 * Processes the parameters of an instructive according to the instructive received.
 * Adds a line to the code table, and fills it according to the specific instructive line read.
 * Instructives are divided to I, J and, R types.
 * returns 1 if valid, 0 otherwise.
 */
int second_pass_process_instructive(char* line, int* p, int instructive)
{
    int opcode, retval = 1;
    /* R instructive process*/
    opcode = get_opcode(instructive);
    if (instructive >= ADD && instructive <= MVLO) {
        int rs, rt, rd, funct;
        funct = get_funct(instructive); /* gets the appropriate funct for the instructive */
        if (instructive >= ADD && instructive <= NOR)    /* R logical/arithmetical instructive process */
            second_pass_R_arithmetic_process(line, p, &rs, &rt, &rd);
        else { /* R copy instructive process */
            second_pass_R_copy_process(line, p, &rs, &rd);
            rt = 0;
        }
        add_R_row(opcode, rs, rt, rd, funct);
    }
    else if (instructive >= ADDI && instructive <= SH) {   /* I instructive process */
        int rs, rt, immed;
        if (instructive >= ADDI && instructive <= NORI) /* I arithmetic instructive process */
            second_pass_I_arithmetic_process(line, p, &rs, &rt, &immed);
        else if (instructive >= BNE && instructive <= BGT) /* I branched instructive process */
            retval = second_pass_I_branched_process(line, p, &rs, &rt, &immed);
        else     /* I memory instructive process */
            second_pass_I_memory_process(line, p, &rs, &rt, &immed);
        add_I_row(opcode, rs, rt, immed);
    }
    else if (instructive >= JMP && instructive <= STOP) {
        int reg, address;
        if (instructive == JMP || instructive == LA || instructive == CALL) /* jmp instructive */
            retval = second_pass_Jmp_La_Call_process(line, p, instructive, &reg, &address);
        else
            stop_process(&reg, &address);
        add_J_row(opcode, reg, address);
    }
    return retval;
}


/*
 * functions used by process_instructive for further processing and analyzing of the instructive line are:
 * (second_pass_)(I_memory_process, I_branched_process, I_arithmetic_process, R_arithmetic_process,
 * R_copy_process, Jmp_La_Call_process), stop_process.
 */
/*
 * Analyzes lb, sb, lw, sw, lh, sh commands.
 * Gets parameters in the format - $register1  ,  immed,  $register2.
 * Fills rs, rt and immed's values.
 */
void second_pass_I_memory_process(char* line, int* p, int* rs, int* rt, int* immed)
{
    next_char(line, p);
    *rs = second_pass_get_register(line, p);
    second_pass_next_parameter(line, p);
    *immed = second_pass_get_immed(line, p);
    second_pass_next_parameter(line, p);
    *rt = second_pass_get_register(line, p);
}


/*
 * Analyzes beq, bne, blt, bgt commands.
 * Gets parameters in the format - $register1  ,  &register2,  LABEL.
 * Fills the values of rs, rt and immed, when immed is the difference between the address of the Label
 * and the address of the current instructive.
 * Prints error messages. returns 1 if valid, 0 otherwise.
 */
int second_pass_I_branched_process(char* line, int* p, int* rs, int* rt, int* immed)
{
    char label[MAX_LABEL_LENGTH];
    int loop, retval = 1;
    next_char(line, p);
    *rs = second_pass_get_register(line, p);
    second_pass_next_parameter(line, p);
    *rt = second_pass_get_register(line, p);
    second_pass_next_parameter(line, p);
    second_pass_get_operand_label(line, p, label);
    if(!symbol_exists(label)) {
        printf("In line %d: error: operand label %s for branching directive does not exist in symbol table\n",
               second_get_line_number(), label);
        retval = ERROR;
    }
    else {
        char attribute[MAX_ATTRIBUTE_LENGTH];
        get_symbol_attributes(label, attribute);
        if (!strcmp(attribute, "external")) {
            printf("In line %d: error: %s is an external label and can't be used in branching instructive\n",
                   second_get_line_number(), label);
            retval = ERROR;
        }
        else {
            /* gets the address of the label read */
            loop = get_symbol_value(label);
            /* calculate immed according to the instructive */
            *immed = loop - get_IC();
        }
    }
    return retval;
}


/*
 * analyzes addi, subi, andi, ori, nori commands.
 * Gets parameters in the format - $register1  ,  immed,  $register3.
 * Fills the values of rs, rt and immed.
 */
void second_pass_I_arithmetic_process(char* line, int* p, int* rs, int* rt, int* immed)
{
    next_char(line, p);
    *rs = second_pass_get_register(line, p);
    second_pass_next_parameter(line, p);
    *immed = second_pass_get_immed(line, p);
    second_pass_next_parameter(line, p);
    *rt = second_pass_get_register(line, p);
}


/*
 * Analyzes add, sub, and, or, nor  commands.
 * Gets parameters in the format - $register1  ,  &register2,  $register3.
 * Fills the value of rs, rt and rd.
 */
void second_pass_R_arithmetic_process(char* line, int* p, int* rs, int* rt, int* rd)
{
    next_char(line, p);
    *rs = second_pass_get_register(line, p);
    second_pass_next_parameter(line, p);
    *rt = second_pass_get_register(line, p);
    second_pass_next_parameter(line, p);
    *rd = second_pass_get_register(line, p);
}


/*
 * Analyzes move, mvhi, mvlo commands.
 * Gets parameters in the format - $register1  ,  &register2 .
 * Fills the values of rs and rt.
 */
void second_pass_R_copy_process(char* line, int* p, int* rs, int* rt)
{
    next_char(line, p);
    *rs = second_pass_get_register(line, p);
    second_pass_next_parameter(line, p);
    *rt = second_pass_get_register(line, p);
}

/*
 * Analyzes jmp, la and calls commands - receives a label (or a register in some cases of jmp).
 * Prints error messages.
 * returns 1 if valid, 0 otherwise.
 */
int second_pass_Jmp_La_Call_process(char* line, int* p, int instructive, int* reg, int* address)
{
    int retval = 1;
    next_char(line, p);
    /* in case the parameter is a register (in a jmp instructive) */
    if(line[*p] == '$') {
        *address = second_pass_get_register(line, p);
        *reg = 1;
    }
        /* if the parameter is a label */
    else {
        char label[MAX_LABEL_LENGTH], attributes[MAX_ATTRIBUTE_LENGTH];
        char* inst;
        *reg = 0;
        /* reads the label into label */
        second_pass_get_operand_label(line, p, label);
        if(!symbol_exists(label)) {
            /* if the operand for the instructive does not exist in table, prints a specific error message */
            if(instructive == LA)
                inst = "la";
            else if(instructive == CALL)
                inst = "call";
            else
                inst = "jmp";
            printf("In line %d: error: operand label %s for %s directive does not exist in symbol table\n",
                   second_get_line_number(), label, inst);
            retval = ERROR;
        }
        else {
            /* gets the attribute of the label read as operand */
            get_symbol_attributes(label, attributes);
            /* in case the label is not external */
            if (strcmp(attributes, "external"))
                *address = get_symbol_value(label);
            else { /* if the label is external */
                *address = 0;
                insert_external(label, get_IC()); /* inserts the label to the externals' table */
            }
        }
    }
    return retval;
}


/*
 * Simply gives the parameters the value 0, according to the stop data line
 */
void stop_process(int* reg, int* address)
{
    *reg = 0;
    *address = 0;
}


/*
 * Reads the directive from line. and returns it's value.
 */
int second_pass_get_directive(char * line, int *p)
{
    int directive, j = 0; /* j - index in dirct_name */
    char dirct_name[MAX_DIRCT_LENGTH+1]; /* dirct_name holds the directive name */
    next_char(line, p);
    /* reads directive into dirct_name array */
    while(isalpha(line[*p]))
        dirct_name[j++] = line[(*p)++];
    dirct_name[j] = '\0'; /* adding terminal to the directive name string */
    directive = second_pass_directive_name(dirct_name); /* gets the directive */
    return directive;
}

/*
 * Reads the instructive from line. and returns it's value.
 */
int second_pass_get_instructive(char * line, int *p)
{
    int instructive, j = 0; /* j - index in inst_name */
    char inst_name[MAX_INST_LENGTH+1];  /* inst_name holds the instructive name */
    next_char(line, p);
    /* reads instructive into inst_name array */
    while(isalpha(line[*p]))
        inst_name[j++] = line[(*p)++];
    inst_name[j] = '\0';    /* adding terminal to the directive name string */
    instructive = second_pass_instructive_name(inst_name); /* gets the instructive */
    return instructive;
}

/*
 * Compares the string received to all of the directive names.
 * If valid, returns the directive received, otherwise returns 0.
*/
int second_pass_directive_name(char* temp)
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
 * Compares the string received to all of the instructive names.
 * If valid returns the instructive received, otherwise returns 0.
*/
int second_pass_instructive_name(char* temp)
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
 * the function gets an instructive, and returns the funct value according to it
 * (specified in the course booklet)
 */
int get_funct(int instructive)
{
    int functs[NUMBER_OF_FUNCTS] = {0, 1, 2, 3, 4 ,5, 1, 2, 3};
    return functs[instructive];
}

/*
 * the function gets an instructive, and returns the opcode value according to it
 * (specified in the course booklet)
 */
int get_opcode(int instructive)
{
    int op[NUMBER_OF_INSTRUCTIVES] = {0, 0, 0, 0, 0 ,0, 1, 1, 1, 10, 11, 12, 13, 14, 15, 16,
                                      17, 18, 19, 20, 21, 22, 23, 24, 30, 31, 32, 63};
    return op[instructive];
}


/*
 * reads and returns an immed value (a number). Act in a similar way to get_num from first_pass.c
 */
int second_pass_get_immed(char* line, int* p)
{
    int j = 0, retNum;       /* j - index in immed, retNum - the number read*/
    int isNegative = 0;
    char immed[MAX_LINE_LENGTH];    /* holds the immed read from line (as a string) */
    char ch; /* an assisting char */

    if(line[*p] == '-') {
        isNegative = 1;
        (*p)++;
    }
    else if(line[*p] == '+')
        (*p)++;
    while(isdigit(immed[j] = ch = line[(*p)])){	/* while reads digits, puts them in temp[] */
        j++;
        (*p)++;
    }
    immed[j] = '\0';
    retNum = atoi(immed); /* atoi returns the value of a number in a string passed to it (as an int)*/
    if(isNegative)
        retNum = -retNum;
    return retNum;	/* returns number read */
}


/*
 * Reads the register from line.
 * Returns the register number
 */
int second_pass_get_register(char* line, int* p)
{
    int j = 0;       /* j - index in la, i index in line*/
    char regist[MAX_LINE_LENGTH]; /* holds the number of register read from line (as a string) */
    (*p)++;
    while (isdigit(regist[j] = line[(*p)])) {    /* while reads digits, puts them in temp[] */
        j++;
        (*p)++;
    }
    regist[j] = '\0';
    return atoi(regist); /* atoi returns the value of a number in a string passed to it (as an int)*/
}


/*
 * Reads an label as an operand of oa command.
 */
void second_pass_get_operand_label(char* line, int* p, char* label)
{
    int j = 0;       /* j - index in label*/
    next_char(line,p);
    while(j < MAX_LABEL_LENGTH && (isalpha(line[*p]) || isdigit(line[*p])))
        label[j++] = line[(*p)++];
    label[j] = '\0'; /* adding terminal sign to the end of the string */
}

/*
* Reads from the line all numbers until the end of the line.
* Returns the number of numbers read.
*/

int second_pass_get_numbers(char line[], int * p, long numbers[])
{
    /* num - value of current number read  */
    int num, retval = 1;
    /*  reads first number into array, and if valid enters condition   */
    second_pass_get_first_number(line, p, numbers);
    while(next_char(line, p)) {
        second_pass_next_parameter(line, p);   /* puts the number that was read in number array */
        num = second_pass_get_num(line, p);
        numbers[retval] = num;
        retval++;
    }
    return retval;
}


/*
 *  Reads the first number.
 */
void second_pass_get_first_number(char line[], int * p, long numbers[])
{
    int num;
    next_char(line, p);
    num = second_pass_get_num(line, p);
    numbers[0] = num;
}


/*
 * Reads the next single number in line for read_set command and returns it.
 */
long second_pass_get_num(char line[], int* p)
{
    int j = 0; /*  j - index in the current number, retNum - the number read */
    long retNum;
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
    retNum = atol(temp);
    if(isNegative)
        retNum = -retNum;
    return retNum;	/* returns number read */
}

/* exactly the same as get_last_quote from first_pass_utils */
int second_pass_get_last_quote(char* line, int i){
    while(line[i] != '\n')
        i++;
    while(line[i] != '"')
        i--;
    return i;
}

/*
 * Proceeds to the next significant character.
 */
int next_char(char* line, int* p)
{
    int charExists = 1;
    char ch;		/* temporary character */
    while((ch = line[*p]) == ' ' || ch == '\t') /* moves through line as long as a space/tab is read */
        (*p)++;
    if(ch == '\0' || ch == '\n')	/* if end of line reached without significant characters, returns 1 */
        charExists = 0;
    return charExists;
}

/* proceeds to the next parameter (skips the next comma and up to the next significant character */
void second_pass_next_parameter(char line[], int* p)
{
    next_char(line, p);
    (*p)++;
    next_char(line,p);
}

/*
 * Prints warnings in a specific format.
 */
void second_pass_print_error(char* error)
{
    int lineNumber = second_get_line_number();
    printf("In line %d: error: %s", lineNumber, error);
}





