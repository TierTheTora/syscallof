#include "syscallof.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int
main (int argc, char **argv)
{
	int argsleft;
	char *whereptr;
	unsigned long *args;
	struct syscall_info si;

	if (argc < 2)
		goto putusag;

	whereptr = argv[1];
	si = get_sysinfo(whereptr);
	argsleft = argc - si.next;

	if (argsleft <= 0)
		goto putusag;

	args = (unsigned long*)malloc(argsleft *
		sizeof(unsigned long));

	if (parse_sysnums(si, args, argc, argv) != 0)
		return(1);

	print_syscalls(si, argsleft, args);
	free(args);

	return(0);

	putusag:
		puts("Usage: syscallof [OPTION] NUMBERS...");
		return(1);
}
