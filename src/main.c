#include <curl/curl.h>
#include "argparser.h"
#include "help.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordlist.h"

struct t_args {
	char domain[256];
	uint64_t threadnum;
	uint64_t i_low;
	uint64_t i_high;
	char **wordlist;
	char *outfile;
	useconds_t delay;
};

pthread_mutex_t csv_lock;

void *buster(void *args);
char *make_url(char *domain, char *word);
size_t write_data(size_t size, size_t nmemb);
void write_csv(char *filename, char *line);

int main(int argc, char **argv){
	/* Parse the command line arguments. This is kept simple:
		- h: help
		- t: number of threads
		- o: outfile
		- s: delay */
	struct cmd_args args = argparser(argc, argv);
	if (args.help){
		print_help();
		exit(0);
	}

	pthread_mutex_init(&csv_lock, NULL);

	/* Read the wordlist into memory, keep track of how much memory to free */
	uint64_t words_free_length;
	uint64_t num_lines;

	char **words = read_wordlist("words.txt", &words_free_length, &num_lines);

	/* Share the words among the threads */
	uint64_t divs = num_lines / args.threads;
	uint64_t remaining_words = num_lines % args.threads;
	pthread_t threads[args.threads];
	uint64_t prev_i_high = 1;
	uint64_t i;

	/* Start threading */
	for (i=0; i<args.threads; ++i){
		/* Args must be passed to threaded function in a struct */
		struct t_args *t_data = malloc(sizeof(struct t_args));
		strncpy(t_data->domain, args.domain, 256);
		t_data->wordlist = words;
		t_data->outfile = args.outfile;
		t_data->threadnum = i;
		t_data->i_low = prev_i_high;
		t_data->i_high = (t_data->i_low) + divs;
		t_data->delay = args.delay;

		/* If number of words is not divisible by number of threads,
		 * some words will be left over - assign those to threads here */
		if (t_data->i_high > num_lines){
			t_data->i_high = num_lines;
		}
		if (remaining_words){
			++(t_data->i_high);
			--remaining_words;
		}
		prev_i_high = t_data->i_high;

		int rc;
		rc = pthread_create(&threads[i], NULL, buster, t_data);
	}
	for (i=0; i<args.threads; ++i){
		pthread_join(threads[i], NULL);
	}
	
	free_words_memory(words, words_free_length);

	return 0;
}

void *buster(void *args){
	struct t_args t_data = *(struct t_args*) args;
	free(args);

	/* Open a connection to the server - leaving the same connection open */
	/* saves a lot of time.                                               */   
	CURL *curl = curl_easy_init();
	CURLcode result;
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

	/* Some sites think we are a bot (which, we are) or unsupported */
	/* browser if we don't set the user agent.                      */ 
	struct curl_slist *list = NULL;
	char *uagent = "User-Agent: Mozilla/5.0 "
					"(Macintosh; Intel Mac OS X 10.12; rv:55.0) "
					"Gecko/20100101 Firefox/55.0";
	list = curl_slist_append(list, uagent);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
	
	/* Go through the words assigned to this thread and start busting */
	for(; t_data.i_low < t_data.i_high; ++(t_data.i_low)){
		char *request_url = make_url(t_data.domain, 
					t_data.wordlist[t_data.i_low]);

		curl_easy_setopt(curl, CURLOPT_URL, request_url);
		result = curl_easy_perform(curl);

		long http_code;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

		char *out_line = malloc(256);
		snprintf(out_line, 256, "%s;%ld", request_url, http_code);
		write_csv(t_data.outfile, out_line);

		free(out_line);
		free(request_url);

		if (t_data.delay){
			usleep(t_data.delay);
		}
	}

	return NULL;
}

char *make_url(char *domain, char *word){
	/* Makes the substitution word -> {} e.g. domain.com/{}.php becomes    */
	/* domain.com/word.php. This function will break if used incorrectly.  */
	/* So don't be stupid.                                                 */

	char *request_url = malloc(256);
	uint8_t i, j, k;

	/* Copy up to the opening brace */
	for (i=0; i<=255; ++i){
		if (domain[i] == '{'){
			break;
		}
		request_url[i] = domain[i];
	}
	k=i+2;

	/* Now copy over the word */
	for (j=0; i<=255; ++i){
		if (word[j] == '\0'){
			break;
		}
		request_url[i] = word[j];
		++j;
	}

	/* Copy over the part after the opening brace */
	for (; i<=255; ++i){
		if (domain[k] == '\0'){
			request_url[i] = domain[k];
			break;
		}
		request_url[i] = domain[k];
		++k;
	}

	return request_url;
}

size_t write_data(size_t size, size_t nmemb){
	return size * nmemb;
}

void write_csv(char *filename, char *line){
	pthread_mutex_lock(&csv_lock);

	FILE *fp = fopen(filename, "a");
	fprintf(fp, "%s\n", line);

	fclose(fp);
	pthread_mutex_unlock(&csv_lock);
}
