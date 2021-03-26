#include "argparser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_THREADS 1024

struct cmd_args argparser(int argc, char **argv){
	/* Default arguments */
	struct cmd_args args;
	args.help = 0;
	args.threads = 32;
	args.delay = 0;
	strncpy(args.outfile, "result.csv", 256);
	
	/* Catch help or invalid args */
	if (argc > 1 && strcmp(argv[1], "--help") == 0){
		args.help = 1;
		return args;
	} else if (argc == 1){
		printf("Invalid arguments supplied, use --help for info\n");
		exit(1);
	}

	/* See 'man 3 getopt' */
	int opt;
	while ((opt = getopt(argc, argv, "ht:o:s:")) != -1){
		switch (opt){
			case 'h':
				args.help = 1;
				break;
			case 't':
				args.threads = atoi(optarg);
				if (args.threads > MAX_THREADS){
					args.threads = MAX_THREADS;
				}
				break;
			case 'o':
				strncpy(args.outfile, optarg, 256);
				break;
			case 's':
				args.delay = atoi(optarg) * 1000; //convert to useconds
				break;
			default:
				printf("Invalid arguments supplied, use --help for info\n");
				exit(1);
		}
	}

	/* Target domain should be in the first non-option argument */
	strncpy(args.domain, argv[optind], 256);
		
	return args;	
}
