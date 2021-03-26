#include "help.h"
#include <stdio.h>

void print_help(){
	printf("CBuster - A very simple clone of dirbuster written in C.\n");
	printf("./cbuster -[htos] [search_domain]\n\n");

	printf("\th:\tprint this help info and exit\n");
	printf("\tt:\tnumber of threads to use, default is 32\n");
	printf("\to:\toutfile to write result to\n");
	printf("\ts:\ttime delay between calls for each thread (milliseconds)\n\n");

	printf("Curly braces should be used in the search domain.\n");
	printf("The word from the wordlist will be substituted.\n");
	printf("For example https://example.com/{}.php searches for php files.\n");
	printf("https://{}.example.com/ searches for subdomains\n");
}
