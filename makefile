assembler: main.o output.o second_pass.o second_pass_utils.o first_pass.o first_pass_utils.o utils.o label_data_structure.o external_data_structure.o binary_data_structure.o
	gcc -g -Wall -ansi -pedantic main.o output.o second_pass.o second_pass_utils.o first_pass.o first_pass_utils.o utils.o label_data_structure.o external_data_structure.o binary_data_structure.o -o assembler -lm

binary_data_structure.o: binary_data_structure.c binary_data_structure.h
	gcc -c -Wall -ansi -pedantic binary_data_structure.c -o binary_data_structure.o

external_data_structure.o: external_data_structure.c external_data_structure.h
	gcc -c -Wall -ansi -pedantic external_data_structure.c -o external_data_structure.o

label_data_structure.o: label_data_structure.c label_data_structure.h
	gcc -c -Wall -ansi -pedantic label_data_structure.c -o label_data_structure.o

utils.o: utils.c utils.h
	gcc -c -Wall -ansi -pedantic utils.c -o utils.o

first_pass_utils.o: first_pass_utils.c first_pass_utils.h
	gcc -c -Wall -ansi -pedantic first_pass_utils.c -o first_pass_utils.o -lm

first_pass.o: first_pass.c first_pass.h
	gcc -c -Wall -ansi -pedantic first_pass.c -o first_pass.o

second_pass_utils.o: second_pass_utils.c second_pass_utils.h
	gcc -c -Wall -ansi -pedantic second_pass_utils.c -o second_pass_utils.o

second_pass.o: second_pass.c second_pass.h
	gcc -c -Wall -ansi -pedantic second_pass.c -o second_pass.o

output.o: output.c output.h
	gcc -c -Wall -ansi -pedantic output.c -o output.o

main.o: main.c first_pass.h output.h utils.h binary_data_structure.h
	gcc -c -Wall -ansi -pedantic main.c -o main.o

