#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> // malloc & free
#include <stdint.h> // use guaranteed 64-bit integers
#include "tokenizer.h" // Create header file and reference that
#include "memory.h" // built-in functions to read and write to a specific file

int32_t* reg; // Array of 32 32-bit registers

void init_regs();
bool interpret(char* instr);
void write_read_demo();

/**
 * Initialize register array for usage.
 * Malloc space for each 32-bit register then initializes each register to 0.
 * Do not alter this function!
 */
void init_regs(){
	int reg_amount = 32;
	reg = malloc(reg_amount * sizeof(int32_t)); // 32 * 4 bytes
	for(int i = 0; i < 32; i++)
		reg[i] = i;
}


void print_all_tokens(char** tokens){
    int i = 0;
    while(tokens[i] != NULL){
        printf("Token [%d]: %s\n",i,tokens[i]);
        ++i;
    }
    printf("Token[%d]: NULL\n",i);
}

int arraySize(char * str){
	int size = 0;
	while(str[size] != '\0'){
		size+=1;
	}
	return size;
}
int checkString(char * str1, char * str2){	
	if(arraySize(str1) != arraySize(str2)){
		return -1;
	}
	int i = 0;
	
	while(str1[i] != '\0'){

		if(str1[i] != str2[i]){
			return -1;
		}
		i+=1;
	}
	return 0;
}
//represents the power function from math where base ^ exp
int power(int base, int exp){
	int result = base;
	while(exp != 1){
		result*= base;	
		exp-=1;
	}
	return result;
}

//parses a string into an integer
int integerParser(char * str){
	int count = 0;
	int i = 0;
	int negative = 1;
	if(str[0] == '-'){
		negative*=-1;
		++i;
	}
	int size = arraySize(str);
	int place;
	while(str[i] != '\0'){
		place = power(10,(size-i));
		count+= (str[i] - '0')* place;
		++i;
	}
	count/=10;
	return count*negative;
}
/**
 * Fill out this function and use it to read interpret user input to execute RV64 instructions.
 * You may expect that a single, properly formatted RISC-V instruction string will be passed
 * as a parameter to this function.
 */
bool interpret(char* instr){
	char ** tokens = tokenize(instr,' ');
	printf("%s\n",tokens[0]);
	int rd;
	int rs1;
	int rs2;
	int imm;
	char * mem_file = "mem.txt";
	//LW x7 1000(x5)
	//from memory to register -> x7 = Memory[ x7 + 1000]
	if(checkString(tokens[0],"LW") == 0)
	{	
		//tokenizes 1000(x5) -> 1000 x5)
		char ** source = tokenize(tokens[2], '(');
		rd = integerParser(split_str(tokens[1],1,arraySize(tokens[1])));
		rs1 = integerParser(split_str(source[1],1,arraySize(source[1])-1));
		imm = integerParser(source[0]);
		int32_t address = reg[rs1] + imm;
		reg[rd] = read_address(address, mem_file);
		return true;
	}
	//SW x5 40(x6)
	//from register to memory -> Memory[x6 + 40] = x5
	else if(checkString(tokens[0], "SW") == 0)
	{	
		//tokenizes 40(x6) -> 40 x6)
		char ** source = tokenize(tokens[2], '(');
		rd = integerParser(split_str(tokens[1],1,arraySize(tokens[1])));
		rs1 = integerParser(split_str(source[1],1,arraySize(source[1])-1));
		imm = integerParser(source[0]);
		int32_t address = reg[rs1] + imm;
		int32_t data_to_write = reg[rd];
		int32_t write = write_address(data_to_write, address, mem_file);
		return true;
	}
	//ADD X6 X0 X29
	else if(checkString(tokens[0], "ADD") == 0)
	{
		rd = integerParser(tokens[1]);
		rs1 = integerParser(tokens[2]);
		rs2 = integerParser(tokens[3]);
		reg[rd] = reg[rs1] + reg[rs2];
		return true;
	}
	//ADD X6 X0 329
	else if(checkString(tokens[0], "ADDI") == 0)
	{
		rd = integerParser(tokens[1]);
		rs1 = integerParser(tokens[2]);
		imm = integerParser(tokens[3]);
		reg[rd] = reg[rs1] + imm;
		return true;
	}
	//AND X6 X0 X29
	else if(checkString(tokens[0], "AND") == 0)
	{
		rd = integerParser(tokens[1]);
		rs1 = integerParser(tokens[2]);
		imm = integerParser(tokens[3]);
		reg[rd] = reg[rs1] & reg[rs2];
		return true;
	}
	//OR X6 X0 X29
	else if(checkString(tokens[0], "OR") == 0)
	{
		rd = integerParser(tokens[1]);
		rs1 = integerParser(tokens[2]);
		imm = integerParser(tokens[3]);
		reg[rd] = reg[rs1] | reg[rs2];
		return true;
	}
	//XOR X6 X0 X29
	else if(checkString(tokens[0], "XOR") == 0)
	{
		rd = integerParser(tokens[1]);
		rs1 = integerParser(tokens[2]);
		imm = integerParser(tokens[3]);
		reg[rd] = reg[rs1] ^ reg[rs2];
		return true;
	}
	return false;
}


/**
 * Simple demo program to show the usage of read_address() and write_address() found in memory.c
 * Before and after running this program, look at mem.txt to see how the values change.
 * Feel free to change "data_to_write" and "address" variables to see how these affect mem.txt
 * Use 0x before an int in C to hardcode it as text, but you may enter base 10 as you see fit.
 */
void write_read_demo(){
	int32_t data_to_write = 0xFFF; // equal to 4095
	int32_t address = 0x98; // equal to 152
	char* mem_file = "mem.txt";

	// Write 4095 (or "0000000 00000FFF") in the 20th address (address 152 == 0x98)
	int32_t write = write_address(data_to_write, address, mem_file);
	if(write == (int32_t) NULL)
		printf("ERROR: Unsucessful write to address %0X\n", 0x40);
	int32_t read = read_address(address, mem_file);

	printf("Read address %lu (0x%lX): %lu (0x%lX)\n", address, address, read, read); // %lu -> format as an long-unsigned
}

void printRegistry(){
	for(int i = 0; i< 32; ++i){
		printf("x%d : %lu\n",i,reg[i]);
	}
}

/**
 * Your code goes in the main
 *
 */
int main(){
	// Do not write any code between init_regs
	init_regs(); // DO NOT REMOVE THIS LINE

	// Below is a sample program to a write-read. Overwrite this with your own code.
	//write_read_demo();
	
	//char str[20] = {'A','D','D',' ','X','\0'};
	printRegistry();
	printf("Please input instruction: ");
	char * str;
	str = "LW x7 1000(x5)";
	//fgets(str,20,stdin);
	printf("%s\n",str);
	interpret(str);
	printRegistry();
	int32_t address = 1000 + reg[5];
	int32_t read = read_address(address, "mem.txt");

	printf("Read address %lu (0x%lX): %lu (0x%lX)\n", address, address, read, read);
	return 0;
}
