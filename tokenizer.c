//
// Created by demares on 2/28/2022.
//function returns the token size of the str
#include <stdio.h>
#include <stdlib.h>
int tokenizerSize(char * str,char deli){
    int count = 2;
    int i = 0;
    while(str[i] != NULL){
        if(str[i] == deli) count++;
        ++i;
    }
    return count;
}

//function splits a string by start to end position
char* split_str(char*str,int start,int end){
    int size = end-start+1;
    char * new_str = malloc(size *sizeof(char));
    int i = start;
    while(i < end){
        new_str[i-start] = str[i];
        ++i;
    }
    new_str[i] = NULL;
    return new_str;
}
/* Returns a freshly allocated zero-terminated vector of freshly allocated
   delimiter-separated tokens from zero-terminated str.
   For example, tokenize("hello world string"), with a character delimiter
   of a space " " would result in:
     tokens[0] = "hello"
     tokens[1] = "world"
     tokens[2] = "string"
     tokens[3] = 0
*/
char** tokenize(char* str, char deli){
    int token_size = tokenizerSize(str,deli);
    char ** tokens= malloc(token_size * sizeof(char *));
    tokens[0] = malloc(6);
    tokens[1] = malloc(6);
    int k = 0;
    int temp = 0;
    int i = 0;
    while(str[k] != NULL)
    {
        if(str[k] == deli)
        {
            tokens[i] = split_str(str,temp,k);
            temp = k+1;
            ++i;
        }
        ++k;
    }
    tokens[i] = split_str(str,temp,k+1);
    tokens[i+1] = NULL;
    return tokens;
}
//

