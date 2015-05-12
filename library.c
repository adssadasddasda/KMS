// You can use these includes and defines as a hint what functions you might want to use ;)
#define _GNU_SOURCE
#include "md5.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <omp.h>
#include <time.h>

#define HASH_LENGTH 16

typedef struct _word word_list;
typedef struct _word {
	word_list* lchild;
	word_list* rchild;
	char* string;
} word;

word_list* words_from_file(FILE *input_file);

void words_add_word(word_list *words, char *string);

void words_delete(word_list* words);

void words_print(word_list* words);

word_list *words_from_file(FILE *input_file) {
	const char *delim = " {}.,;-:0123456789?!\"*+()|&[]#$/%%'\n\t\a\b\r\v\f\\\'";
	char line_buffer[128] = {0};
	int line_buffer_length = 128;
	char *word_buffer;
	word_list *words;
	
	char *line_exists = fgets(line_buffer, line_buffer_length, input_file); //initialize words with the first word of the Book
	word_buffer = strtok(line_buffer, delim);
	words = malloc(sizeof(word));
	words->string = (char *) malloc(strlen(word_buffer));
	strcpy(words->string, word_buffer);
	
	while (line_exists!=NULL) {
		while (word_buffer!=NULL) {
			words_add_word(words, word_buffer);
			word_buffer = strtok(NULL, delim);
		}
		line_exists = fgets(line_buffer, line_buffer_length, input_file);
		word_buffer = strtok(line_buffer, delim);
	}
	return words;
}

void words_add_word(word_list *words, char *string) {	//somehow THIS does not work as intended!!!
	int str_comp = strcmp(string, words->string);
	if (str_comp < 0) {
		if (words->lchild!=NULL) {
			words_add_word(words->lchild, string);
		} else {
			word_list *new = malloc(sizeof(word));
			new->string = (char *) malloc(strlen(string));
			strcpy(new->string, string);
			words->lchild = new;
		}
	} else if (str_comp > 0) {
		if (words->rchild!=NULL) {
			words_add_word(words->rchild, string);
		} else {
			word_list *new = malloc(sizeof(word));
			new->string = (char *) malloc(strlen(string));
			strcpy(new->string, string);
			words->rchild = new;
		}
	}
}

void words_delete(word_list *words) {
	if (words->lchild!=NULL) words_delete(words->lchild);
	if (words->rchild!=NULL) words_delete(words->rchild);
	free(words);
}

void words_print(word_list *words) {
	if (words->lchild!=NULL) words_print(words->lchild);
	printf("%s\n", words->string);
	if (words->rchild!=NULL) words_print(words->rchild);
}

void words_print_to_file(word_list *words, FILE* target_file) {
	if (words->lchild!=NULL) words_print_to_file(words->lchild, target_file);
	fputs(words->string, target_file);
	fputs("\n", target_file);
	if (words->rchild!=NULL) words_print_to_file(words->rchild, target_file);
	
}

void get_md5(char *string1, char *string2, unsigned char *destination) {
	char *string = (char *) malloc(strlen(string1));
	strcpy(string, string1);
	strcat(string, " ");
	strcat(string, string2);
	printf("%s\n", string);
	MD5_CTX* ctx = malloc(sizeof(MD5_CTX));
	MD5_Init(ctx);
	MD5_Update(ctx, string, strlen(string));
	MD5_Final(destination, ctx);
}

void print_unsigned_char_array(unsigned char *array, int length) { //needs length of array as parameter cause sizeof(array) does not work inside of a function
	int i;
	for (i = 0; i<length; i++) {
		printf("0x%02x ", array[i]);
	}
	printf("\n");
}

int main (){
	//unsigned char target[HASH_LENGTH] = {0xe6, 0x54, 0x93, 0xcc, 0xde, 0xe9, 0xc4, 0x51, 0x4f, 0xe2, 0x0e, 0x04, 0x04, 0xf3, 0xbc, 0xb8}; "???" - to be used for les_miserables.txt
	unsigned char target[HASH_LENGTH] = {0x1c, 0x01, 0x43, 0xc6, 0xac, 0x75, 0x05, 0xc8, 0x86, 0x6d, 0x10, 0x27, 0x0a, 0x48, 0x0d, 0xec}; // "Les Miserables" - to be used for les_miserables_preface.txt
	unsigned char test[HASH_LENGTH] = {0};
	time_t start = time(NULL);
	FILE* f;
	word_list *words;
	
	f = fopen("les_miserables.txt", "r");
	if(f == NULL){
		return 1;
	}
	
	words = words_from_file(f);
	
	FILE* out = fopen("words.txt", "w");
	words_print_to_file(words, out);
	fclose(out);
	
	get_md5("Les", "Miserables", test); //checks if get_md5 works properly. If both of the codes in console output are identical it works!
	print_unsigned_char_array(test, sizeof(test));
	print_unsigned_char_array(target, sizeof(target));
	
	//words_print(words);
	
	words_delete(words);
	
	/*
	 * - find all unique words from the book
	 * - create the hashes of all two-string combinations
	 * - try to speed it up
	 * Usage example (not complete!):
	 *
	 * unsigned char hash[HASH_LENGTH] = {0};
	 * char buffer[1024];
	 * // put something into buffer to hash it
	 * MD5_CTX md5_ctx;
	 * MD5_Init(&md5_ctx);
	 * MD5_Update(&md5_ctx, buffer, strlen(buffer));
	 * MD5_Final(hash, &md5_ctx);
	 */
	fclose(f);
	time_t end = time(NULL);
	printf("Execution took ~%fs\n", difftime(end, start));
	return 0;
}
