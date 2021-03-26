#ifndef FILE_WORDLIST_SEEN
#define FILE_WORDLIST_SEEN

char **read_wordlist(char *filename, uint64_t *free_length, uint64_t *n_lines);
void **free_words_memory(char **words, uint64_t free_length);

#endif
