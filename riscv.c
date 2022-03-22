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
	printf("%s\n",str);
	int count = 0;
	int i = 0;
	int negative = 1;
	if(str[0] == '-'){
		negative*=-1;
		++i;
	}else if(str[0] == '+') ++i;
	int size = arraySize(str);
	//printf("%d\n",size);
	int place;
	while(str[i] != '\0' && str[i] != ' '){
		if(str[i] - '0' > 9) return -1;
		place = power(10,(size-i));
		count+= (str[i] - '0')* place;
		//printf("%c %d\n",str[i],count);
		++i;
	}
	count/=10;
	return count*negative;
}

bool checkInst(int rd, int rs1, int rs2, int imm){
	//check if rd,rs1, and rs2 are not integers or not found in the register
	//for example rs1 should not equal 100 or -1 since they are not in the register
	if(rd == -1 || rd < 0 || rd > 31){
		printf("rd is incorrect | rd = %d\n",rd);
		return false;
	}else if(rs1 == -1 || rs1 < 0 || rs1 > 31){
		printf("rs1 is incorrect | rs1 = %d\n",rs1);
		return false;
	}else if(rs2 == -1 || rs2 < 0 || rs2 > 31){
		printf("rs2 is incorrect | rs2 = %d\n",rs2);
		return false;
	}else if(imm == -1){
		printf("imm is incorrect | imm = %d\n",imm);
		return false;
	}
	return true;
}
/**
 * Fill out this function and use it to read interpret user input to execute RV64 instructions.
 * You may expect that a single, properly formatted RISC-V instruction string will be passed
 * as a parameter to this function.
 */
bool interpret(char* instr){
	char ** tokens = tokenize(instr,' ');
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
		rs1 = integerParser(split_str(source[1],1,arraySize(source[1])-2));
		imm = integerParser(source[0]);
		if(!checkInst(rd,rs1,0,imm)) return false;
		int32_t address = reg[rs1] + imm;
		reg[rd] = read_address(address, mem_file);
		free(source);
	}
	//SW x5 40(x6)
	//from register to memory -> Memory[x6 + 40] = x5
	else if(checkString(tokens[0], "SW") == 0)
	{	
		//tokenizes 40(x6) -> 40 x6)
		char ** source = tokenize(tokens[2], '(');
		rd = integerParser(split_str(tokens[1],1,arraySize(tokens[1])));
		rs1 = integerParser(split_str(source[1],1,arraySize(source[1])-2));
		imm = integerParser(source[0]);
		if(!checkInst(rd,rs1,0,imm)) return false;
		int32_t address = reg[rs1] + imm;
		int32_t data_to_write = reg[rd];
		int32_t write = write_address(data_to_write, address, mem_file);
		free(source);
		if(write == (int32_t) NULL)
		printf("ERROR: Unsucessful write to address %0X\n", address);
	}
	//ADD X6 X0 X29
	else if(checkString(tokens[0], "ADD") == 0)
	{
		rd = integerParser(split_str(tokens[1],1,arraySize(tokens[1])));
		rs1 = integerParser(split_str(tokens[2],1,arraySize(tokens[2])));
		rs2 = integerParser(split_str(tokens[3],1,arraySize(tokens[3])-1));
		//printf("%d %d %d\n",rd,rs1,rs2);
		if(!checkInst(rd,rs1,rs2,0)) return false;
		reg[rd] = reg[rs1] + reg[rs2];
	}
	//ADDI X6 X0 329
	else if(checkString(tokens[0], "ADDI") == 0)
	{
		rd = integerParser(split_str(tokens[1],1,arraySize(tokens[1])));
		rs1 = integerParser(split_str(tokens[2],1,arraySize(tokens[2])));
		imm = integerParser(split_str(tokens[3],0,arraySize(tokens[3])-1));
		if(!checkInst(rd,rs1,0,imm)) return false;
		reg[rd] = reg[rs1] + imm;
	}
	//AND X6 X0 X29
	else if(checkString(tokens[0], "AND") == 0)
	{
		rd = integerParser(split_str(tokens[1],1,arraySize(tokens[1])));
		rs1 = integerParser(split_str(tokens[2],1,arraySize(tokens[2])));
		rs2 = integerParser(split_str(tokens[3],1,arraySize(tokens[3])-1));
		if(!checkInst(rd,rs1,rs2,0)) return false;
		reg[rd] = reg[rs1] & reg[rs2];
	}
	//OR X6 X0 X29
	else if(checkString(tokens[0], "OR") == 0)
	{
		rd = integerParser(split_str(tokens[1],1,arraySize(tokens[1])));
		rs1 = integerParser(split_str(tokens[2],1,arraySize(tokens[2])));
		rs2 = integerParser(split_str(tokens[3],1,arraySize(tokens[3])-1));
		if(!checkInst(rd,rs1,rs2,0)) return false;
		reg[rd] = reg[rs1] | reg[rs2];
	}
	//XOR X6 X0 X29
	else if(checkString(tokens[0], "XOR") == 0)
	{
		rd = integerParser(split_str(tokens[1],1,arraySize(tokens[1])));
		rs1 = integerParser(split_str(tokens[2],1,arraySize(tokens[2])));
		rs2 = integerParser(split_str(tokens[3],1,arraySize(tokens[3])-1));
		if(!checkInst(rd,rs1,rs2,0)) return false;
		reg[rd] = reg[rs1] ^ reg[rs2];
	}else{
		printf("Couldn't identify operation\n");
		return false;
	}
	return true;
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
	
	//printRegistry();
	printf("Please input instruction: ");
	char * str[20];
	//str = "LW x7 1000(x5)";
	//interpret(str);
	
	char * p = fgets(str,20,stdin);

	printf("\n");
	while(p != NULL){
	//printf("%s\n",str);
	bool result = interpret(str);
	if(result) printf("Success\n");
	printf("Please input instruction: ");
	p = fgets(str,20,stdin);
	}
	
	printRegistry();
	return 0;
}
