#include "syscallof.h"
#include "asmdef.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int
getsys_ent (struct syscall_info si, qword target)
{
	syssz_t i;

	if (target > si.sysmax)
		return -1;

	for (i = 0; i < si.syscnt; i++) {
		if (si.systabl[i].val == target)
			return i;
	}

	return -1;
}

void
print_help ()
{
	puts("Usage: syscallof [OPTION] NUMBERS...\n"
	     "Options:\n"
	     "\t--x86_64   use x86_64  syscall table\n"
	     "\t--arm      use arm     syscall table\n"
	     "\t--aarch64  use aarch64 syscall table\n"
	     "\t--x86      use x86     syscall table\n"
	);
}

struct syscall_info
get_sysinfo (char *arg)
{
	struct syscall_info si;
	si.next = 2;
	if (strncmp(arg, "--", 2) == 0) {
		if (strcmp(arg, "--x86_64") == 0)
			goto set_x86_64;
		else if (strcmp(arg, "--arm") == 0)
			goto set_arm;
		else if (strcmp(arg, "--aarch64") == 0)
			goto set_aarch64;
		else if (strcmp(arg, "--x86") == 0)
			goto set_x86;
		else if (strcmp(arg, "--help") == 0) {
			print_help();
			exit(0);
		}
		else {
			printf(
				"Unrecognised option `%s`.\n"
				"Use --help for help\n",
				arg
			);
			exit(1);
		}
	}
	else {
		si.next = 1;
		#if defined __x86_64__
		goto set_x86_64;
		#elif defined __arm__
		goto set_arm;
		#elif defined __aarch64__
		goto set_aarch64;
		#elif defined __i386__
		goto set_x86;
		#endif
	}
	set_x86_64:
		si.sysarch = X64_86;
		si.systabl = (struct syscall *)(ssyscalls_x86_64);
		si.sysmax  = SYSCALLS_X86_64_MAX;
		si.syscnt  = SYSCALLS_X86_64_CNT;
		return(si);
	set_arm:
		si.sysarch = ARM;
		si.systabl = (struct syscall *)(ssyscalls_arm);
		si.sysmax  = SYSCALLS_ARM_MAX;
		si.syscnt  = SYSCALLS_ARM_CNT;
		return(si);
	set_aarch64:
		si.sysarch = AARCH64;
		si.systabl = (struct syscall *)(ssyscalls_aarch64);
		si.sysmax  = SYSCALLS_AARCH64_MAX;
		si.syscnt  = SYSCALLS_AARCH64_CNT;
		return(si);
	set_x86:
		si.sysarch = X86;
		si.systabl = (struct syscall *)(ssyscalls_x86);
		si.sysmax  = SYSCALLS_X86_MAX;
		si.syscnt  = SYSCALLS_X86_CNT;
		return(si);
}

int
parse_sysnums (struct syscall_info si,
               unsigned long *args,
               int argc,
               char **argv)
{
	int i;
	char *whereptr;
	for (i = 0; si.next < argc; si.next++, i++) {
		whereptr = argv[si.next];
		args[i] = atol(whereptr);
		if (args[i] > si.sysmax
		 || getsys_ent(si, args[i]) == -1) {
			printf("Number `%lu` is not a syscall.\n",
				args[i]);
			return(1);
		}
	}
	return(0);
}

void print_syscalls (struct syscall_info si,
                     unsigned long sz,
		     unsigned long *args)
{
	int i;
	for (i = 0; i < (int)(sz); i++) {
		printf("%s\n", si.systabl
			[ getsys_ent(si, args[i]) ].name
		);
	}
}
