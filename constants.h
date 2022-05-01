#ifndef CONSTANTS
#define CONSTANTS

#define MASK_16_BITS 65535 /* Mask to take only the 16 least significant bits with */
#define MASK_8_BITS  255 /* Mask to take only the 8 least significant bits with */
#define MAX_LINE_LENGTH 80 /* Maximum allowed length of a line */
#define INITIAL_ADDRESS 100 /* Initial address to count instrucitons from */
#define MAX_LABEL_LENGTH 32  /* Maximum allowed length of a label */
#define MAX_ATTRIBUTE_LENGTH 11 /* Attribute can't be longer than 'code, entry' */
#define ERROR 0 /* Error code */
#define TYPE_SIZE 10 /* Memory allocation upper bound, for code types */

#define NUMBER_OF_INSTRUCTIVES 28
#define NUMBER_OF_FUNCTS 9
#define MAX_INST_LENGTH 5
#define MAX_DIRCT_LENGTH 7

#define BYTE 8
#define WORD 32
#define HALF_WORD 16

#define MAX_REGISTER_NUM 31
#define MAX_IMMED 32768

#endif
