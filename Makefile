CC=gcc
CFLAGS=-c -Wall
USE_TOKENIZER=true# <-- true if using a custom tokenizer, false otherwise
CUSTOM_TOKENIZER= "tokenizer"# <-- put your tokenizer.c file name without the file extension

all: riscv populate

populate: populate.o
	$(CC) populate.o -o populate

populate.o: populate.c
	$(CC) $(CFLAGS) populate.c -o populate.o

riscv: memory.o tokenizer.o riscv.o
	$(CC) riscv.o memory.o tokenizer.o -o riscv


riscv.o: riscv.c
	$(CC) $(CFLAGS) riscv.c -o riscv.o

memory.o: memory.c
	$(CC) $(CFLAGS) memory.c -o memory.o

tokenizer.o: tokenizer.c
	$(CC) $(CFLAGS) tokenizer.c -o tokenizer.o

clean :
	rm riscv.o populate.o memory.o tokenizer.o riscv populate mem.txt
