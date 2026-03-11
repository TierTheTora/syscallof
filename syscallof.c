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
	puts("Usage: syscallof [OPTIONS...] NUMBERS...\n"
	     "Options:\n"
	     "\t--x86_64   use x86_64  syscall table\n"
	     "\t--arm      use arm     syscall table\n"
	     "\t--aarch64  use aarch64 syscall table\n"
	     "\t--x86      use x86     syscall table\n"
	     "\n"
	     "\t--help  show this text\n"
	     "\t--name  search by syscall name not number\n"
	);
	exit(0);
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
parse_gnu_opt (struct syscall_info *si, char *opt,
               bool *arch_set)
{
	if (strcmp(opt, "--x86_64") == 0) {
		set_arch(si, X86_64);
		*arch_set = true;
		return(0);
	}
	if (strcmp(opt, "--arm") == 0) {
		set_arch(si, ARM);
		*arch_set = true;
		return(0);
	}
	if (strcmp(opt, "--aarch64") == 0) {
		set_arch(si, AARCH64);
		*arch_set = true;
		return(0);
	}
	if (strcmp(opt, "--x86") == 0) {
		set_arch(si, X86);
		*arch_set = true;
		return(0);
	}
	if (strcmp(opt, "--help") == 0) {
		print_help();
	}
	if (strcmp(opt, "--name") == 0) {
		si->sa.search_name = true;
		return(0);
	}

	/* invalid --%s option */
	if (strncmp(opt, "--", 2) == 0) {
		return(1);
	}

	/* not an option */
	return(2);
}

int
parse_unix_opt (struct syscall_info *si, char *opt,
               bool *arch_set)
{
	size_t i;

	if (opt[0] == '-') {
		for (i = 1; i < strlen(opt); i++) {
			switch (opt[i]) {
			case 'x':
				set_arch(si, X86_64);
				*arch_set = true;
				break;
			case 'a':
				set_arch(si, ARM);
				*arch_set = true;
				break;
			case 'A':
				set_arch(si, AARCH64);
				*arch_set = true;
				break;
			case 'X':
				set_arch(si, X86);
				*arch_set = true;
				break;
			case 'h':
				print_help();
				break;
			case 'n':
				si->sa.search_name = true;
				break;
			default:
				return(1);
			}
		}
		return(0);
	}

	/* not an option */
	return(2);
}

int
set_arg (struct syscall_info *si, char *opt, bool *arch_set)
{
	int gnu_opt_result, unix_opt_result;

	gnu_opt_result = parse_gnu_opt(si, opt, arch_set);

	switch (gnu_opt_result) {
	case 0:
		return(0);
	case 1:
		return(1);
	}

	unix_opt_result = parse_unix_opt(si, opt, arch_set);

	switch (unix_opt_result) {
	case 0:
		return(0);
	case 1:
		return(1);
	}

	return(2);
}

struct syscall_info
get_sysinfo (int argc, char **arg)
{
	struct syscall_info si;
	int i, arg_cntr, set_arg_result;
	bool arch_set, fnas;

	arch_set = fnas = false;
	arg_cntr = 0;
	si.sa.search_name = false;

	for (i = 0; i < argc; i++) {
		set_arg_result =
			set_arg(&si, arg[i], &fnas);

		switch (set_arg_result) {
		case 0:
			if (fnas == true)
				arch_set = true;
			arg_cntr++;
			break;
		case 1:
			printf(
				"Unrecognised option `%s`.\n"
				"Use --help for help\n",
				arg[i]
			);
			exit(1);
		}
	}

	si.next = arg_cntr + 1;

	if (arch_set == false) {
		#if defined __x86_64__
		set_arch(&si, X86_64);
		#elif defined __arm__
		set_arch(&si, ARM);
		#elif defined __aarch64__
		set_arch(&si, AARCH64);
		#elif defined __i386__
		set_arch(&si, X86);
		#endif
	}

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
