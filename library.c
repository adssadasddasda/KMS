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

word_list *words_from_file(FILE *input_file) {
	const char *delim = " .,;-:0123456789?!\"*+()|&[]#$/%%'\n\t\a\b\r\v\f\\\'";
	char line_buffer[128] = {0};
	int line_buffer_length = 128;
	char *word_buffer;
	word_list *words;
	
	char *line_exists = fgets(line_buffer, line_buffer_length, input_file); //initialize words with the first word of the Book
	word_buffer = strtok(line_buffer, delim);
	words = malloc(sizeof(word));
	words->string = word_buffer;
	
	printf("%s\n", words->string);			//delete someday
	
	while (line_exists!=NULL) {
		while (word_buffer!=NULL) {
			//printf("%s\n", word_buffer);
			words_add_word(words, word_buffer);
			word_buffer = strtok(NULL, delim);
		}
		printf("%s\n", words->string);		//delete someday
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
			new->string = string;
			words->lchild = new;
		}
	} else if (str_comp > 0) {
		if (words->rchild!=NULL) {
			words_add_word(words->rchild, string);
		} else {
			word_list *new = malloc(sizeof(word));
			new->string = string;
			words->rchild = new;
		}
	}
	printf("%s\n", words->string);		//delete someday
}

void words_delete(word_list *words) {
	if (words->lchild!=NULL) words_delete(words->lchild);
	if (words->rchild!=NULL) words_delete(words->rchild);
	free(words);
}
void words_print(word_list *words) {
	if (words->lchild!=NULL) words_delete(words->lchild);
	printf("%s\n", words->string);
	if (words->rchild!=NULL) words_delete(words->rchild);
}

int main (){
	// unsigned char target[HASH_LENGTH] = {0xe6, 0x54, 0x93, 0xcc, 0xde, 0xe9, 0xc4, 0x51, 0x4f, 0xe2, 0x0e, 0x04, 0x04, 0xf3, 0xbc, 0xb8}; "???" - to be used for les_miserables.txt
	//unsigned char target[HASH_LENGTH] = {0x1c, 0x01, 0x43, 0xc6, 0xac, 0x75, 0x05, 0xc8, 0x86, 0x6d, 0x10, 0x27, 0x0a, 0x48, 0x0d, 0xec}; // "Les Miserables" - to be used for les_miserables_preface.txt
	time_t start = time(NULL);
	FILE* f;
	word_list *words;
	
	f = fopen("les_miserables_preface.txt", "r");
	if(f == NULL){
		return 1;
	}
	
	words = words_from_file(f);
	printf("%s\n", words->string);
	
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

	time_t end = time(NULL);
	printf("Execution took ~%fs\n", difftime(end, start));
	return 0;
}
