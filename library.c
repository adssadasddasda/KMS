// You can use these includes and defines as a hint what functions you might want to use ;)
#define _GNU_SOURCE
#include "md5.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define HASH_LENGTH 16

int num_of_words = 0;

typedef struct _word word_list;
typedef struct _word {
	word_list* lchild;
	word_list* rchild;
	char* string;
} word;

word_list* words_from_file(FILE *input_file);

void words_add_word(word_list *words, char *string);

word_list *create_word(char *string);

void words_delete(word_list* words);

void words_print(word_list* words);

word_list *get_most_left_child(word_list *words);

char *look_for_md5(unsigned char *target, char **word_array, int array_size);

char *combine_strings_with_whitespace(char *string1, char *string2);

word_list *look_in_order(unsigned char *target, word_list *word1, word_list *words);

int compare_md5(unsigned char *code1, unsigned char *code2);

word_list *words_from_file(FILE *input_file) {
	const char *delim = " {}.,;-:0123456789?!\"*+()|&[]#$/%%'\n\t\a\b\r\v\f\\\'";
	char line_buffer[128] = {0};
	int line_buffer_length = 128;
	char *word_buffer;
	word_list *words;
	
	char *line_exists = fgets(line_buffer, line_buffer_length, input_file); //initialize words with the first word of the Book
	word_buffer = strtok(line_buffer, delim);
	words = create_word(word_buffer);
	
	while (line_exists!=NULL) {
		while (word_buffer!=NULL) {
			if (word_buffer[0]!='\0') words_add_word(words, word_buffer);
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
			words->lchild = create_word(string);
		}
	} else if (str_comp > 0) {
		if (words->rchild!=NULL) {
			words_add_word(words->rchild, string);
		} else {
			words->rchild = create_word(string);
		}
	}
}

word_list *create_word(char *string) {
	//printf("-\n");
	if (strlen(string)<1) return NULL;
	//printf("\"%s\"\n", string);
	num_of_words++;
	word_list *new = malloc(sizeof(word));
	char *newstring = malloc(strlen(string)+1);
	memcpy(newstring, string, strlen(string)+1);
	new->string = newstring;
	return new;
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
	char *string = combine_strings_with_whitespace(string1, string2);
	MD5_CTX* ctx = malloc(sizeof(MD5_CTX));
	MD5_Init(ctx);
	MD5_Update(ctx, string, strlen(string));
	MD5_Final(destination, ctx);
	free(string);
	free(ctx);
}

void print_md5(unsigned char *_md5) { //prints md5 to console. Does not add \n!
	int letter;
	for (letter = 0; letter<HASH_LENGTH; letter++) printf("0x%02x ", _md5[letter]);
}

int compare_md5(unsigned char *code1, unsigned char *code2) { //returns 1 if both md5 codes are identical, else it returns 0
	int letter;
	for (letter = 0; letter<HASH_LENGTH; letter++) {
		if (code1[letter]!=code2[letter]) return 0;
	}
	return 1;
}

char *combine_strings_with_whitespace(char *string1, char *string2) {
	int num_of_chars = strlen(string1)+strlen(string2)+2;
	char *string = (char *) malloc(num_of_chars);
	memcpy(string, string1, strlen(string1));
	memcpy(string+strlen(string1), " ", 1);
	memcpy(string+strlen(string1)+1, string2, strlen(string2)+1);
	return string;
}

char *look_for_md5(unsigned char *target, char **word_array, int array_size){
	int index1, index2, found=0;
	unsigned char candidate[HASH_LENGTH];
	char *result = NULL;
	for (index1=0; index1<array_size && found==0; index1++) {
		#pragma omp parallel for
		for (index2=0; index2<array_size && found==0; index2++) { // this expression does not work here. Need workaround!
			#pragma omp flush(found)
			get_md5(word_array[index1], word_array[index2], candidate);
			if (compare_md5(target, candidate)==1) {
				result = combine_strings_with_whitespace(word_array[index1], word_array[index2]);
				found = 1;
				#pragma omp flush(found)
			}
		}
	}
	return result;
}

int word_list_to_array( word_list *words, char **array, int index) { //array must be large enough to contain all elements of words or segmentation fault will occur!
	if (words->lchild!=NULL) index = word_list_to_array(words->lchild, array, index);
	array[index] = words->string;
	index++;
	if (words->rchild!=NULL) index = word_list_to_array(words->rchild, array, index);
	return index;
}

void print_string_array(char **array, int size) {
	int index;
	for (index = 0; index<size; index++) {
		printf("%s\n", array[index]);
	}
}

int main (){
	unsigned char target[HASH_LENGTH] = {0xe6, 0x54, 0x93, 0xcc, 0xde, 0xe9, 0xc4, 0x51, 0x4f, 0xe2, 0x0e, 0x04, 0x04, 0xf3, 0xbc, 0xb8}; //"???" - to be used for les_miserables.txt
	//unsigned char target[HASH_LENGTH] = {0x1c, 0x01, 0x43, 0xc6, 0xac, 0x75, 0x05, 0xc8, 0x86, 0x6d, 0x10, 0x27, 0x0a, 0x48, 0x0d, 0xec}; // "Les Miserables" - to be used for les_miserables_preface.txt
	//unsigned char test[HASH_LENGTH] = {0};
	time_t start = time(NULL);
	FILE* f;
	word_list *words;
	
	f = fopen("les_miserables.txt", "r");
	if(f == NULL){
		return 1;
	}
	
	words = words_from_file(f);
	//words_print(words);	

	printf("%d\n", num_of_words);
	char *word_array[num_of_words];
	word_list_to_array(words, word_array, 0);
	//print_string_array(word_array, num_of_words);
	words_delete(words);
	
	char *target_words = look_for_md5(target, word_array, num_of_words);
	printf("%s\n", target_words);

	//FILE* out = fopen("words.txt", "w");
	//words_print_to_file(words, out);
	//fclose(out);
	
	//words_print(words);
	
	
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
