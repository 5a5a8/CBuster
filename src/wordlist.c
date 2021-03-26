#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char **read_wordlist(char *filename, uint64_t *free_length, uint64_t *n_lines){
	/* Allocate an initial amount of memory for storing each line */
	uint64_t max_wordlength = 32;
	uint64_t lines_allocated = 1024;
	uint64_t allocation_increment = 1024;

	char **words = malloc(lines_allocated * sizeof(char *));

	FILE *fp = fopen(filename, "r");
	if (fp == NULL){
		fprintf(stderr, "Could not open file: %s\n", filename);
		exit(1);
	}

	uint64_t i;
	for (i=0; 1; ++i){
		/* If needed, allocate more memory */
		if (i >= lines_allocated){
			lines_allocated += allocation_increment;
			words = realloc(words, lines_allocated * sizeof(char *));
		}

		/* Read each line, stripping the newline */
		words[i] = malloc(max_wordlength);
		if (fgets(words[i], max_wordlength-1, fp) == NULL){
			break;
		}

		/* File may be from a windows machine with carriage returns */
		if (words[i][strlen(words[i])-2] == '\r') {
			words[i][strlen(words[i])-2] = '\0';
		} else if (words[i][strlen(words[i])-1] == '\n') {
			words[i][strlen(words[i])-1] = '\0';
		}
	}

	/* The caller is responsible for freeing */
	fclose(fp);
	*free_length = lines_allocated;
	*n_lines = i;
	return words;
}	

void free_words_memory(char **words, uint64_t free_length){
	uint64_t i;
	for (i=0; i<free_length; ++i){
		free(words[i]);
	}
	free(words);
}
