#include "syscallof.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
	
unsigned long *args;
char        **sargs;

void
free_all ()
{
	free(args);
	free(sargs);
}

int
main (int argc, char **argv)
{
	int argsleft;
	struct syscall_info si;

	atexit(free_all);

	if (argc < 2)
		goto putusag;

	si = get_sysinfo(argc - 1, &argv[1]);
	argsleft = argc - si.next;

	if (argsleft <= 0)
		goto putusag;

	args = (unsigned long *)malloc(argsleft *
		sizeof(unsigned long));
	sargs = (char **)malloc(argsleft *
		sizeof(char *));

	if (parse_sysnums(si, args, sargs, argc, argv) != 0)
		return(1);

	print_syscalls(si, argsleft, args, sargs);

	return(0);

	putusag:
		printf("%d\n", argc);
		puts("Usage: syscallof [OPTIONS...] NUMBERS...");
		return(1);
}
