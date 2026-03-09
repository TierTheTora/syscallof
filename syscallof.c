#include "syscallof.h"
#include "asmdef.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

int
getsys_ent (struct syscall_info si, qword target)
{
	syssz_t i;

	if (target > si.sysmax)
		return(-1);

	for (i = 0; i < si.syscnt; i++) {
		if (si.systabl[i].val == target)
			return(i);
	}

	return(-1);
}

int
sgetsys_ent (struct syscall_info si, char *target)
{
	syssz_t i;

	for (i = 0; i < strlen(target); i++) {
		target[i] = toupper((int)(target[i]));
	}

	for (i = 0; i < si.syscnt; i++) {
		if (strcmp(si.systabl[i].name, target) == 0)
			return(i);
	}

	return(-1);
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
	     "\n"
	     "\t--help  show this text\n"
	     "\t--name  search by syscall name not number\n"
	);
}

void
set_arch (struct syscall_info *si, enum arch ar)
{
	switch (ar) {
	case X86_64:
		si->sysarch = X86_64;
		si->systabl = (struct syscall *)(ssyscalls_x86_64);
		si->sysmax  = SYSCALLS_X86_64_MAX;
		si->syscnt  = SYSCALLS_X86_64_CNT;
		break;
	case ARM:
		si->sysarch = ARM;
		si->systabl = (struct syscall *)(ssyscalls_arm);
		si->sysmax  = SYSCALLS_ARM_MAX;
		si->syscnt  = SYSCALLS_ARM_CNT;
		break;
	case AARCH64:
		si->sysarch = AARCH64;
		si->systabl = (struct syscall *)(ssyscalls_aarch64);
		si->sysmax  = SYSCALLS_AARCH64_MAX;
		si->syscnt  = SYSCALLS_AARCH64_CNT;
		break;
	case X86:
		si->sysarch = X86;
		si->systabl = (struct syscall *)(ssyscalls_x86);
		si->sysmax  = SYSCALLS_X86_MAX;
		si->syscnt  = SYSCALLS_X86_CNT;
	}
}

int
set_arg (struct syscall_info *si, char *opt, bool *arch_set)
{
	if (strcmp(opt, "--x86_64") == 0) {
		set_arch(si, X86_64);
		*arch_set = true;
		return 0;
	}
	if (strcmp(opt, "--arm") == 0) {
		set_arch(si, ARM);
		*arch_set = true;
		return 0;
	}
	if (strcmp(opt, "--aarch64") == 0) {
		set_arch(si, AARCH64);
		*arch_set = true;
		return 0;
	}
	if (strcmp(opt, "--x86") == 0) {
		set_arch(si, X86);
		*arch_set = true;
		return 0;
	}
	if (strcmp(opt, "--help") == 0) {
		print_help();
		exit(0);
	}
	if (strcmp(opt, "--name") == 0) {
		si->sa.search_name = true;
		return 0;
	}

	return 1;
}

struct syscall_info
get_sysinfo (int argc, char **arg)
{
	struct syscall_info si;
	int i, arg_cntr;
	bool arch_set, fnas;

	arch_set = fnas = false;
	arg_cntr = 0;
	si.sa.search_name = false;

	for (i = 0; i < argc; i++) {
		if (strncmp(arg[i], "--", 2) == 0) {
			if (set_arg(&si, arg[i], &fnas) != 0) {
				printf(
					"Unrecognised option `%s`.\n"
					"Use --help for help\n",
					arg[i]
				);
				exit(1);
			}
			if (fnas == true)
				arch_set = true;
			arg_cntr++;
		}
	}

	si.next = arg_cntr + 1;

	if (arch_set == false) {
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

	return(si);

	set_x86_64:
		set_arch(&si, X86_64);
		return(si);
	set_arm:
		set_arch(&si, ARM);
		return(si);
	set_aarch64:
		set_arch(&si, AARCH64);
		return(si);
	set_x86:
		set_arch(&si, X86);
		return(si);
}

int
parse_sysnums (struct syscall_info si,
               unsigned long *args,
               char **sargs,
               int argc,
               char **argv)
{
	int i;
	char *whereptr;

	for (i = 0; si.next < argc; si.next++, i++) {
		whereptr = argv[si.next];

		if (si.sa.search_name) {
			sargs[i] = whereptr;
			if (sgetsys_ent(si, sargs[i]) == -1) {
				printf("`%s` is not a syscall.\n",
					sargs[i]);
				return(1);
			}
		}
		else {
			args[i] = atol(whereptr);
			if (args[i] > si.sysmax
			 || getsys_ent(si, args[i]) == -1) {
				printf("Number `%lu` is not a syscall.\n",
					args[i]);
				return(1);
			}
		}
	}
	return(0);
}

void print_syscalls (struct syscall_info si,
                     unsigned long sz,
		     unsigned long *args,
                     char **sargs)
{
	int i;
	for (i = 0; i < (int)(sz); i++) {
		if (si.sa.search_name)
			printf("%d\n", si.systabl
				[ sgetsys_ent(si, sargs[i]) ].val
			);
		else
			printf("%s\n", si.systabl
				[ getsys_ent(si, args[i]) ].name
			);
	}
}
