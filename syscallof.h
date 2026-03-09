#ifndef SYSCALLOF_H
# define SYSCALLOF_H

#include "asmdef.h"
#include <stdbool.h>

typedef unsigned long syssz_t;

enum arch {
	X86_64,
	ARM,
	AARCH64,
	X86,
};

struct syscall_args {
	bool search_name;
};

struct syscall_info {
	struct syscall *systabl;
	syssz_t sysmax;
	syssz_t syscnt;
	enum arch sysarch;
	int next;
	struct syscall_args sa;
};

int getsys_ent (struct syscall_info si,
                qword target);

int
sgetsys_ent (struct syscall_info si,
             char *target);


void print_help ();

void set_arch (struct syscall_info *si,
               enum arch ar);

int set_arg (struct syscall_info *si,
             char *opt,
             bool *arch_set);

struct syscall_info get_sysinfo (int argc,
                                 char **arg);

int parse_sysnums (struct syscall_info si,
                   unsigned long *args,
                   char **sargs,
                   int argc,
                   char **argv);

void print_syscalls (struct syscall_info si,
                     unsigned long sz,
		     unsigned long *args,
		     char **sargs);

#endif /* SYSCALLOF_H */
