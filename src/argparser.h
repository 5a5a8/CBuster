#ifndef FILE_ARGPARSER_SEEN
#define FILE_ARGPARSER_SEEN

#include <stdlib.h>
#include <unistd.h>

struct cmd_args {
	uint8_t help;
	uint16_t threads;
	char domain[256];
	char outfile[256];
	useconds_t delay;
};

struct cmd_args argparser(int argc, char **argv);

#endif
